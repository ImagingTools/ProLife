# SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
<#
.SYNOPSIS
    CI entry point for the ProLife Api Postman suite: starts the
    PumaServerPgTest.exe / LisaServerTest.exe dependencies (ProLifeServerTest
    talks to both over HTTP - see ProLifeServerTest.acc's PumaHttpPort/
    LisaHttpPort), restores puma_test/lisa_test/prolife_test from real
    pg_dump backups (a populated dev-environment export, not an empty
    schema - see -ProLifeBackupPath below for why), starts
    ProLifeServerTest.exe, runs newman against it, and tears all three
    servers back down in reverse order. Intended to be invoked as a single
    TeamCity build step (PowerShell runner). Mirrors
    Puma\Tests\TenantApiPostman\Run-CiTests.ps1.

.PARAMETER RepoRoot
    Root of the ProLife checkout. Defaults to the PROLIFEDIR environment
    variable (the convention already used across this repo, see
    Impl\ProLifeServer\QMake\ProLifeServer.pri's $(PROLIFEDIR)); if
    PROLIFEDIR isn't set, falls back to two levels above this script
    (Tests\ProLifeApiPostman\..\..).

.PARAMETER PumaRepoRoot
    Root of the Puma checkout. Defaults to the PUMADIR environment variable;
    if unset, falls back to a directory named "Puma" next to RepoRoot
    (Lisa/Puma/ProLife are checked out as siblings, e.g. under Git\).

.PARAMETER LisaRepoRoot
    Root of the Lisa checkout. Defaults to the LISADIR environment variable;
    if unset, falls back to a directory named "Lisa" next to RepoRoot, same
    sibling-checkout convention as -PumaRepoRoot.

.PARAMETER BuildConfig
    Name of the Bin\<BuildConfig> folder to look for ProLifeServerTest.exe
    in (e.g. "Release_Qt6_VC17_x64"). Ignored if -ServerExePath is passed
    explicitly. Also used to locate PumaServerPgTest.exe / LisaServerTest.exe
    under -PumaRepoRoot / -LisaRepoRoot unless -PumaServerExePath /
    -LisaServerExePath are passed explicitly.

.PARAMETER ServerExePath
    Full path to ProLifeServerTest.exe. Overrides -BuildConfig when set.
    Pass this explicitly on CI if the build output lives somewhere other
    than RepoRoot\Bin\<BuildConfig>.

.PARAMETER PumaServerExePath
    Full path to PumaServerPgTest.exe. Overrides -BuildConfig when set.

.PARAMETER LisaServerExePath
    Full path to LisaServerTest.exe. Overrides -BuildConfig when set.

.PARAMETER HttpPort
    Port the ProLife test server listens on (must match
    ProLifeServerTest.acc's HttpPort attribute, currently 17778).

.PARAMETER PumaHttpPort
    Port the Puma test server listens on (must match PumaServerPgTest.acc's
    HttpPort attribute, currently 17788 - same value ProLifeServerTest.acc's
    PumaHttpPort points at).

.PARAMETER LisaHttpPort
    Port the Lisa test server listens on (must match LisaServerTest.acc's
    DefaultHttpPort attribute, currently 17776 - same value
    ProLifeServerTest.acc's LisaHttpPort points at).

.PARAMETER DbName / DbHost / DbPort / DbUser / DbPassword
    Postgres connection used to drop/recreate the test database before
    each run (must match ProLifeServerSettings.acc's
    ProLifeDatabaseAccessSettings / DatabaseEngine defaults).

.PARAMETER PumaDbName / LisaDbName
    Puma/Lisa test database names (must match PumaServerPgTest.acc's DbName
    "puma_test" / LisaServerTest.acc's DbName "lisa_test"). Both are
    restored from a backup before every run (see -PumaBackupPath /
    -LisaBackupPath) rather than left for the server to create/migrate from
    scratch.

.PARAMETER PumaBackupPath / LisaBackupPath / ProLifeBackupPath
    Paths to the pg_dump custom-format backups used to (re)create the
    puma_test / lisa_test / prolife_test databases before each run. Default
    to puma.backup / lisa.backup / prolife.backup next to this script -
    copies of the same-named files under Tests\backend\backups\, kept local
    to this suite so it doesn't depend on the legacy Docker fixture layout.
    These are real, populated dev-environment exports (not empty schemas):
    DeviceAdd/SoftwareProductAdd need catalog/type records
    (deviceTypeId/softwareCatalogProductId/licenseDefinitionId - see the
    environment variables below) that no GraphQL mutation in this codebase
    can create from scratch, so an empty prolife_test can never actually
    exercise those fields. Restoring the real export is the only way to get
    working catalog references; it also brings realistic Accounts/Orders/
    Devices/SoftwareInstances data and, in Puma's case, a working "su"
    account and hundreds of real users/roles alongside it - all harmless
    for this suite since its own assertions fetch objects it just created
    by id rather than relying on exact list counts/positions.

.PARAMETER PsqlPath
    Full path to psql.exe. If omitted, resolved from PATH, then from the
    default PostgreSQL install locations (highest version wins). Pass
    this explicitly if the CI agent has Postgres installed somewhere
    non-standard. Also used to resolve pg_restore.exe (same bin folder).

.PARAMETER JUnitReportPath
    Where newman writes the JUnit XML report. Point TeamCity's "XML Report
    Processing" (JUnit) build feature at this same path.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1 `
        -BuildConfig "Release_Qt6_VC17_x64" -DbPassword "%db.password%"
#>

[CmdletBinding()]
param(
    # Resolving the repo root purely from PowerShell's automatic script-path
    # variables ($PSScriptRoot / $PSCommandPath / $MyInvocation) is NOT
    # reliable across every CI runner - see the identical rationale in
    # Puma's Run-CiTests.ps1. TeamCity does reliably set the working
    # directory to the checkout root for build steps, so that's tried
    # first, self-validated against a file we know must exist at a fixed
    # relative location. PROLIFEDIR (this org's own checkout-location
    # convention, matching PUMADIR for Puma) is tried before that; the
    # fragile automatic variables are the last resort, kept for
    # convenience when invoking the script directly during local dev.
    [string]$RepoRoot = $(
        $collectionRelPath = "Tests\ProLifeApiPostman\ProLifeApi.postman_collection.json"
        if ($env:PROLIFEDIR -and (Test-Path (Join-Path $env:PROLIFEDIR $collectionRelPath))) {
            $env:PROLIFEDIR
        }
        elseif (Test-Path (Join-Path (Get-Location).Path $collectionRelPath)) {
            (Get-Location).Path
        }
        else {
            $sd = if ($PSScriptRoot) { $PSScriptRoot }
                  elseif ($PSCommandPath) { Split-Path -Parent $PSCommandPath }
                  elseif ($MyInvocation.MyCommand.Path) { Split-Path -Parent $MyInvocation.MyCommand.Path }
                  else { $null }
            if ($sd) {
                (Resolve-Path (Join-Path $sd "..\..")).Path
            }
            else {
                throw "Unable to determine the repo checkout root (PROLIFEDIR unset/stale, working directory isn't the checkout root, and this script's own path could not be determined). Pass -RepoRoot explicitly."
            }
        }
    ),
    [string]$ScriptDir = (Join-Path $RepoRoot "Tests\ProLifeApiPostman"),
    [string]$BuildConfig = "Release_Qt6_VC17_x64",

    # Lisa/Puma/ProLife are checked out as siblings (e.g. D:\...\Git\Lisa,
    # D:\...\Git\Puma, D:\...\Git\ProLife) - same convention PUMADIR/
    # PROLIFEDIR already encode, extended here with LISADIR.
    [string]$PumaRepoRoot = $(if ($env:PUMADIR) { $env:PUMADIR } else { Join-Path (Split-Path -Parent $RepoRoot) "Puma" }),
    [string]$LisaRepoRoot = $(if ($env:LISADIR) { $env:LISADIR } else { Join-Path (Split-Path -Parent $RepoRoot) "Lisa" }),

    [string]$ServerExePath = "",
    [string]$PumaServerExePath = "",
    [string]$LisaServerExePath = "",

    [int]$HttpPort = 17778,
    [int]$PumaHttpPort = 17788,
    [int]$LisaHttpPort = 17776,

    [string]$DbName = "prolife_test",
    [string]$PumaDbName = "puma_test",
    [string]$LisaDbName = "lisa_test",
    [string]$DbHost = "localhost",
    [int]$DbPort = 5432,
    [string]$DbUser = "postgres",
    [string]$DbPassword = "root",

    [string]$PumaBackupPath = (Join-Path $ScriptDir "puma.backup"),
    [string]$LisaBackupPath = (Join-Path $ScriptDir "lisa.backup"),
    [string]$ProLifeBackupPath = (Join-Path $ScriptDir "prolife.backup"),

    [string]$PsqlPath = "",
    [string]$CollectionPath = (Join-Path $ScriptDir "ProLifeApi.postman_collection.json"),
    [string]$EnvironmentPath = (Join-Path $ScriptDir "ProLifeApi-Dev.postman_environment.json"),
    [string]$JUnitReportPath = (Join-Path $ScriptDir "junit-report.xml"),
    [string]$JsonReportPath = (Join-Path $ScriptDir "run-report.json"),
    [int]$StartupTimeoutSeconds = 60
)

$ErrorActionPreference = "Stop"
$serverProcess = $null
$pumaProcess = $null
$lisaProcess = $null
$exitCode = 1

if ([string]::IsNullOrWhiteSpace($ServerExePath)) {
    $ServerExePath = Join-Path $RepoRoot "Bin\$BuildConfig\ProLifeServerTest.exe"
}
if ([string]::IsNullOrWhiteSpace($PumaServerExePath)) {
    $PumaServerExePath = Join-Path $PumaRepoRoot "Bin\$BuildConfig\PumaServerPgTest.exe"
}
if ([string]::IsNullOrWhiteSpace($LisaServerExePath)) {
    $LisaServerExePath = Join-Path $LisaRepoRoot "Bin\$BuildConfig\LisaServerTest.exe"
}

function Write-Step($message) {
    Write-Host "`n=== $message ===" -ForegroundColor Cyan
}

function Resolve-PsqlPath {
    if (-not [string]::IsNullOrWhiteSpace($PsqlPath)) {
        if (-not (Test-Path $PsqlPath)) { throw "psql not found at explicit path: $PsqlPath" }
        return $PsqlPath
    }

    $onPath = Get-Command psql.exe -ErrorAction SilentlyContinue
    if ($onPath) { return $onPath.Source }

    $candidates = Get-ChildItem "$env:ProgramFiles\PostgreSQL\*\bin\psql.exe" -ErrorAction SilentlyContinue |
        Sort-Object { [int]([regex]::Match($_.Directory.Parent.Name, '\d+').Value) } -Descending
    if ($candidates) { return $candidates[0].FullName }

    throw "Could not locate psql.exe. Pass -PsqlPath explicitly or add PostgreSQL\bin to PATH on this agent."
}

function Stop-ServerProcess([string]$processName) {
    Write-Step "Stopping any running $processName.exe"
    Get-Process -Name $processName -ErrorAction SilentlyContinue | ForEach-Object {
        Write-Host "Killing PID $($_.Id)"
        Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue
    }
    Start-Sleep -Seconds 2
}

function Reset-Database([string]$name) {
    # Only drops the database if present. Callers that need seeded data
    # (Lisa) restore a backup into it afterwards; callers that don't
    # (Puma, ProLife) rely on their server exe creating it (and running
    # migrations) on startup when it doesn't already exist.
    Write-Step "Resetting database '$name'"
    $psql = Resolve-PsqlPath
    Write-Host "Using psql: $psql"
    $env:PGPASSWORD = $DbPassword
    # psql writes routine NOTICEs (e.g. "database does not exist, skipping")
    # to stderr. Under $ErrorActionPreference = "Stop" (set globally in this
    # script), PowerShell 5.1 turns *any* stderr line from a native command
    # into a terminating NativeCommandError regardless of the process's own
    # exit code - so this must run under "Continue" and be judged solely by
    # $LASTEXITCODE.
    $previousEap = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    try {
        & $psql -h $DbHost -p $DbPort -U $DbUser -d postgres -v ON_ERROR_STOP=1 -c "SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE datname = '$name' AND pid <> pg_backend_pid();" 2>&1 | Write-Host
        & $psql -h $DbHost -p $DbPort -U $DbUser -d postgres -v ON_ERROR_STOP=1 -c "DROP DATABASE IF EXISTS $name;" 2>&1 | Write-Host
        if ($LASTEXITCODE -ne 0) { throw "Failed to drop database '$name' (exit $LASTEXITCODE)" }
    }
    finally {
        $ErrorActionPreference = $previousEap
        Remove-Item Env:\PGPASSWORD -ErrorAction SilentlyContinue
    }
}

function Restore-DatabaseFromBackup([string]$name, [string]$backupPath) {
    # PumaServerPgTest.exe/LisaServerTest.exe/ProLifeServerTest.exe would all
    # happily create-and-migrate an EMPTY database from scratch on startup,
    # but an empty prolife_test can never exercise DeviceAdd/
    # SoftwareProductAdd - those need catalog/type records
    # (deviceTypeId/softwareCatalogProductId/licenseDefinitionId) that no
    # mutation in this codebase can create, only inherit from a real export.
    # So every database this suite touches is restored from a real,
    # populated backup instead of left empty.
    if (-not (Test-Path $backupPath)) {
        throw "Backup file not found: $backupPath"
    }

    Reset-Database $name

    Write-Step "Creating database '$name'"
    $psql = Resolve-PsqlPath
    $pgRestore = Join-Path (Split-Path -Parent $psql) "pg_restore.exe"
    if (-not (Test-Path $pgRestore)) { throw "pg_restore not found next to psql: $pgRestore" }

    # Same stderr/NativeCommandError pitfall as Reset-Database above -
    # pg_restore --verbose writes its progress log to stderr, which
    # $ErrorActionPreference = "Stop" would otherwise turn into a spurious
    # terminating error even on a clean, zero-exit-code restore.
    $env:PGPASSWORD = $DbPassword
    $previousEap = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    try {
        & $psql -h $DbHost -p $DbPort -U $DbUser -d postgres -v ON_ERROR_STOP=1 -c "CREATE DATABASE $name OWNER $DbUser;" 2>&1 | Write-Host
        if ($LASTEXITCODE -ne 0) { throw "Failed to create database '$name' (exit $LASTEXITCODE)" }

        Write-Step "Restoring '$name' from $backupPath"
        & $pgRestore -h $DbHost -p $DbPort -U $DbUser -d $name --verbose $backupPath 2>&1 | Write-Host
        if ($LASTEXITCODE -ne 0) { throw "pg_restore failed for '$name' (exit $LASTEXITCODE)" }
    }
    finally {
        $ErrorActionPreference = $previousEap
        Remove-Item Env:\PGPASSWORD -ErrorAction SilentlyContinue
    }
}

function Repair-ProLifeForeignServers {
    # prolife.backup's Roles/Users/UserGroups/UserSessions tables are FOREIGN
    # TABLES (postgres_fdw) backed by a single foreign server, "PumaServer"
    # (confirmed against the restored prolife_test and the pg_restore
    # --verbose log - there's a dormant "setup_foreign_table_lisaserver()"
    # helper function but it's never invoked, so no "LisaServer" foreign
    # server actually exists in this dump; Lisa integration goes over HTTP
    # instead). "PumaServer"'s srvoptions carried dbname='puma' (the real,
    # non-test database this dev export was taken from), NOT puma_test.
    # Left uncorrected, any query ProLifeServerTest issues against these
    # foreign tables would transparently hit whatever real "puma" database
    # exists on the local Postgres instance - on a dev machine that also has
    # a real Puma checkout, that's a silent cross-contamination risk (reads
    # AND, if the FDW mapping allows writes, potentially writes) against the
    # developer's real data. This must run immediately after every
    # prolife_test restore, before ProLifeServerTest.exe is started.
    Write-Step "Repointing prolife_test's FDW server at puma_test"
    $psql = Resolve-PsqlPath
    # Passing a double-quoted SQL identifier (needed for the mixed-case
    # "PumaServer" server name) through -c as a native-exe argument is
    # unreliable under PowerShell 5.1's command-line escaping - the quotes
    # can get stripped en route, so Postgres folds the identifier to
    # lowercase ("pumaserver") and then fails to find it. Writing the SQL to
    # a temp file and using -f sidesteps command-line quoting entirely.
    $sqlFile = New-TemporaryFile
    try {
        @"
ALTER SERVER "PumaServer" OPTIONS (SET host '$DbHost', SET dbname '$PumaDbName');
"@ | Set-Content -Path $sqlFile -Encoding ASCII

        $env:PGPASSWORD = $DbPassword
        $previousEap = $ErrorActionPreference
        $ErrorActionPreference = "Continue"
        try {
            & $psql -h $DbHost -p $DbPort -U $DbUser -d $DbName -v ON_ERROR_STOP=1 -f $sqlFile.FullName 2>&1 | Write-Host
            if ($LASTEXITCODE -ne 0) { throw "Failed to repoint FDW servers (exit $LASTEXITCODE)" }
        }
        finally {
            $ErrorActionPreference = $previousEap
            Remove-Item Env:\PGPASSWORD -ErrorAction SilentlyContinue
        }
    }
    finally {
        Remove-Item $sqlFile -Force -ErrorAction SilentlyContinue
    }
}

function Wait-ForPort([string]$serverLabel, [System.Diagnostics.Process]$process, [int]$port) {
    $deadline = (Get-Date).AddSeconds($StartupTimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        if ($process.HasExited) {
            throw "$serverLabel exited prematurely (exit code $($process.ExitCode))"
        }
        $probe = Test-NetConnection -ComputerName "localhost" -Port $port -WarningAction SilentlyContinue -InformationLevel Quiet
        if ($probe) { return }
        Start-Sleep -Milliseconds 500
    }
    throw "$serverLabel did not open port $port within $StartupTimeoutSeconds seconds"
}

function Start-PumaTestServer {
    Write-Step "Starting PumaServerPgTest.exe"
    if (-not (Test-Path $PumaServerExePath)) {
        throw "Server executable not found: $PumaServerExePath"
    }
    $workDir = Split-Path -Parent $PumaServerExePath
    $script:pumaProcess = Start-Process -FilePath $PumaServerExePath -WorkingDirectory $workDir -PassThru -WindowStyle Hidden
    Write-Host "Started PID $($script:pumaProcess.Id)"
    Wait-ForPort "PumaServerPgTest.exe" $script:pumaProcess $PumaHttpPort
    Write-Host "Puma test server is accepting connections on port $PumaHttpPort"
}

function Start-LisaTestServer {
    Write-Step "Starting LisaServerTest.exe"
    if (-not (Test-Path $LisaServerExePath)) {
        throw "Server executable not found: $LisaServerExePath"
    }
    $workDir = Split-Path -Parent $LisaServerExePath
    $script:lisaProcess = Start-Process -FilePath $LisaServerExePath -WorkingDirectory $workDir -PassThru -WindowStyle Hidden
    Write-Host "Started PID $($script:lisaProcess.Id)"
    Wait-ForPort "LisaServerTest.exe" $script:lisaProcess $LisaHttpPort
    Write-Host "Lisa test server is accepting connections on port $LisaHttpPort"
}

function Start-TestServer {
    Write-Step "Starting ProLifeServerTest.exe"
    if (-not (Test-Path $ServerExePath)) {
        throw "Server executable not found: $ServerExePath"
    }
    $workDir = Split-Path -Parent $ServerExePath
    $script:serverProcess = Start-Process -FilePath $ServerExePath -WorkingDirectory $workDir -PassThru -WindowStyle Hidden
    Write-Host "Started PID $($script:serverProcess.Id)"
    Wait-ForPort "ProLifeServerTest.exe" $script:serverProcess $HttpPort
    Write-Host "Server is accepting connections on port $HttpPort"
}

function Install-NewmanIfNeeded {
    # newman is pinned as a local devDependency in this folder's package.json
    # rather than assumed to be installed globally on the agent - a fresh or
    # pooled/reprovisioned TeamCity agent otherwise has no way to run it.
    $newmanCmd = Join-Path $ScriptDir "node_modules\.bin\newman.cmd"
    if (Test-Path $newmanCmd) {
        return $newmanCmd
    }

    Write-Step "Installing newman (npm install in $ScriptDir)"
    # npm writes deprecation warnings to stderr, which $ErrorActionPreference
    # = "Stop" (set globally in this script) would otherwise turn into a
    # spurious terminating NativeCommandError under PowerShell 5.1 even
    # though npm's own exit code is 0 - same pitfall as psql/pg_restore
    # above, judged solely by $LASTEXITCODE instead.
    $previousEap = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    Push-Location $ScriptDir
    try {
        & npm install --no-audit --no-fund 2>&1 | Out-Host
        if ($LASTEXITCODE -ne 0) { throw "npm install failed (exit $LASTEXITCODE)" }
    }
    finally {
        $ErrorActionPreference = $previousEap
        Pop-Location
    }

    if (-not (Test-Path $newmanCmd)) {
        throw "npm install completed but newman binary still not found at: $newmanCmd"
    }
    return $newmanCmd
}

function Invoke-NewmanSuite {
    $newmanCmd = Install-NewmanIfNeeded

    Write-Step "Running newman suite"
    # NOT .../ProLife/graphql - every request in the collection templates its
    # URL as "{{baseUrl}}/graphql" already (confirmed against
    # ProLifeApi.postman_collection.json), so including the segment here
    # doubled it into .../ProLife/graphql/graphql, a 405 that the
    # collection's own JSON-parsing test script then reported as a confusing
    # "JSONError" rather than a clear "wrong URL/405" - caught only once this
    # suite was actually run to completion end-to-end (every prior run
    # apparently stopped short of reading the full newman output).
    $baseUrl = "http://localhost:$HttpPort/ProLife"
    $newmanArgs = @(
        "run", $CollectionPath,
        "-e", $EnvironmentPath,
        "--env-var", "baseUrl=$baseUrl",
        "--reporters", "cli,junit,json",
        "--reporter-junit-export", $JUnitReportPath,
        "--reporter-json-export", $JsonReportPath
    )
    & $newmanCmd @newmanArgs | Out-Host
    return $LASTEXITCODE
}

$repoRootSource = if ($PSBoundParameters.ContainsKey('RepoRoot')) { 'explicit -RepoRoot' }
    elseif ($env:PROLIFEDIR -eq $RepoRoot) { 'PROLIFEDIR env var' }
    elseif ((Get-Location).Path -eq $RepoRoot) { 'working directory' }
    else { 'script-relative fallback' }

Write-Step "Resolved paths"
Write-Host "RepoRoot:          $RepoRoot  (source: $repoRootSource)"
Write-Host "PumaRepoRoot:      $PumaRepoRoot"
Write-Host "LisaRepoRoot:      $LisaRepoRoot"
Write-Host "ScriptDir:         $ScriptDir"
Write-Host "ServerExePath:     $ServerExePath"
Write-Host "PumaServerExePath: $PumaServerExePath"
Write-Host "LisaServerExePath: $LisaServerExePath"
Write-Host "PumaBackupPath:    $PumaBackupPath"
Write-Host "LisaBackupPath:    $LisaBackupPath"
Write-Host "ProLifeBackupPath: $ProLifeBackupPath"
Write-Host "CollectionPath:    $CollectionPath"
Write-Host "EnvironmentPath:   $EnvironmentPath"

try {
    # Stop stale processes from a previous, possibly-crashed run before
    # touching any database - ProLifeServerTest talks to Puma/Lisa over
    # HTTP, so a stale Puma/Lisa left bound to the port would make the new
    # instance fail to start rather than the new one being used.
    Stop-ServerProcess "ProLifeServerTest"
    Stop-ServerProcess "LisaServerTest"
    Stop-ServerProcess "PumaServerPgTest"

    Restore-DatabaseFromBackup $PumaDbName $PumaBackupPath
    Start-PumaTestServer

    Restore-DatabaseFromBackup $LisaDbName $LisaBackupPath
    Start-LisaTestServer

    Restore-DatabaseFromBackup $DbName $ProLifeBackupPath
    Repair-ProLifeForeignServers
    Start-TestServer

    $exitCode = Invoke-NewmanSuite
}
finally {
    # Tear down in reverse start order: ProLife depends on Lisa/Puma being
    # up, not the other way round, so it's safe (and cleanest) to stop it
    # first.
    if ($serverProcess -and -not $serverProcess.HasExited) {
        Write-Step "Stopping ProLifeServerTest.exe (PID $($serverProcess.Id))"
        Stop-Process -Id $serverProcess.Id -Force -ErrorAction SilentlyContinue
    }
    if ($lisaProcess -and -not $lisaProcess.HasExited) {
        Write-Step "Stopping LisaServerTest.exe (PID $($lisaProcess.Id))"
        Stop-Process -Id $lisaProcess.Id -Force -ErrorAction SilentlyContinue
    }
    if ($pumaProcess -and -not $pumaProcess.HasExited) {
        Write-Step "Stopping PumaServerPgTest.exe (PID $($pumaProcess.Id))"
        Stop-Process -Id $pumaProcess.Id -Force -ErrorAction SilentlyContinue
    }
}

if ($exitCode -eq 0) {
    Write-Host "`nAll tests passed." -ForegroundColor Green
} else {
    Write-Host "`nTest run failed (newman exit code $exitCode)." -ForegroundColor Red
}

exit $exitCode
