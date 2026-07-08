# SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
<#
.SYNOPSIS
    CI entry point for the PatTokenApi Postman suite: restores puma_test
    from a backup, starts PumaServerPgTest.exe (auth provider - CreateSuperuser/
    Authorization/RoleAdd/UserAdd all proxy to it over HTTP), resets the
    prolife_test database, starts ProLifeServerTest.exe, runs newman against
    it, and tears both servers back down. Intended to be invoked as a single
    TeamCity build step (PowerShell runner). Mirrors
    Tests\DeskTicketApiPostman\Run-CiTests.ps1 (same Puma dependency); unlike
    an earlier version of this script, it no longer assumes some other
    process already has a Puma server listening on -PumaHttpPort - it starts
    and tears down its own, so it's safe to run standalone or back-to-back
    with ProLifeApiPostman/DeskTicketApiPostman in either order.

.PARAMETER RepoRoot
    Root of the ProLife checkout. Defaults to the PROLIFEDIR environment
    variable; if PROLIFEDIR isn't set, falls back to two levels above this
    script (Tests\PatTokenApiPostman\..\..).

.PARAMETER PumaRepoRoot
    Root of the Puma checkout. Defaults to the PUMADIR environment variable;
    if unset, falls back to a directory named "Puma" next to RepoRoot
    (Puma/ProLife are checked out as siblings, e.g. under Git\).

.PARAMETER BuildConfig
    Name of the Bin\<BuildConfig> folder to look for ProLifeServerTest.exe
    in (e.g. "Release_Qt6_VC17_x64"). Ignored if -ServerExePath is passed
    explicitly. Also used to locate PumaServerPgTest.exe under
    -PumaRepoRoot unless -PumaServerExePath is passed explicitly.

.PARAMETER ServerExePath
    Full path to ProLifeServerTest.exe. Overrides -BuildConfig when set.

.PARAMETER PumaServerExePath
    Full path to PumaServerPgTest.exe. Overrides -BuildConfig when set.

.PARAMETER HttpPort
    Port the test server listens on (must match ProLifeServerTest.acc's
    HttpPort attribute, currently 17778 - same test server ProLifeApiPostman
    uses).

.PARAMETER PumaHttpPort
    Port the Puma test server listens on (must match PumaServerPgTest.acc's
    HttpPort attribute, currently 17788 - same value ProLifeServerTest.acc's
    PumaHttpPort points at).

.PARAMETER DbName / DbHost / DbPort / DbUser / DbPassword
    Postgres connection used to drop/recreate the test database before
    each run.

.PARAMETER PumaDbName
    Puma test database name (default: puma_test), restored from
    -PumaBackupPath before every run - see Tests\ProLifeApiPostman\Run-CiTests.ps1's
    -PumaBackupPath docs for why a populated backup is used instead of an
    empty schema.

.PARAMETER PumaBackupPath
    Path to the pg_dump custom-format backup used to (re)create puma_test
    before each run. Defaults to Tests\ProLifeApiPostman\puma.backup (shared
    with that suite and DeskTicketApiPostman rather than duplicated here).

.PARAMETER PsqlPath
    Full path to psql.exe. If omitted, resolved from PATH, then from the
    default PostgreSQL install locations (highest version wins). Also used
    to resolve pg_restore.exe (same bin folder).

.PARAMETER JUnitReportPath
    Where newman writes the JUnit XML report. Point TeamCity's "XML Report
    Processing" (JUnit) build feature at this same path.

.PARAMETER IncludeOptInFolders
    When set, also runs folders "90 Exploratory - Omitted Input" and
    "91 Auth-Cache Staleness" (see README - the first can crash a Debug
    build via Q_ASSERT on omitted input, the second needs a real 5-minute
    wait to be meaningful). Off by default; not intended for a normal CI run.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1 `
        -BuildConfig "Release_Qt6_VC17_x64" -DbPassword "%db.password%"
#>

[CmdletBinding()]
param(
    [string]$RepoRoot = $(
        $collectionRelPath = "Tests\PatTokenApiPostman\PatTokenApi.postman_collection.json"
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
    [string]$ScriptDir = (Join-Path $RepoRoot "Tests\PatTokenApiPostman"),
    [string]$BuildConfig = "Release_Qt6_VC17_x64",

    # Puma/ProLife are checked out as siblings (e.g. D:\...\Git\ProLife,
    # D:\...\Git\Puma) - same convention PUMADIR/PROLIFEDIR already encode
    # in Tests\ProLifeApiPostman\Run-CiTests.ps1.
    [string]$PumaRepoRoot = $(if ($env:PUMADIR) { $env:PUMADIR } else { Join-Path (Split-Path -Parent $RepoRoot) "Puma" }),

    [string]$ServerExePath = "",
    [string]$PumaServerExePath = "",
    [int]$HttpPort = 17778,
    [int]$PumaHttpPort = 17788,
    [string]$DbName = "prolife_test",
    [string]$PumaDbName = "puma_test",
    [string]$DbHost = "localhost",
    [int]$DbPort = 5432,
    [string]$DbUser = "postgres",
    [string]$DbPassword = "root",
    [string]$PumaBackupPath = (Join-Path $RepoRoot "Tests\ProLifeApiPostman\puma.backup"),
    [string]$PsqlPath = "",
    [string]$CollectionPath = (Join-Path $ScriptDir "PatTokenApi.postman_collection.json"),
    [string]$EnvironmentPath = (Join-Path $ScriptDir "PatTokenApi-Dev.postman_environment.json"),
    [string]$JUnitReportPath = (Join-Path $ScriptDir "junit-report.xml"),
    [string]$JsonReportPath = (Join-Path $ScriptDir "run-report.json"),
    [int]$StartupTimeoutSeconds = 60,
    [switch]$IncludeOptInFolders
)

$ErrorActionPreference = "Stop"
$serverProcess = $null
$pumaProcess = $null
$exitCode = 1

if ([string]::IsNullOrWhiteSpace($ServerExePath)) {
    $ServerExePath = Join-Path $RepoRoot "Bin\$BuildConfig\ProLifeServerTest.exe"
}
if ([string]::IsNullOrWhiteSpace($PumaServerExePath)) {
    $PumaServerExePath = Join-Path $PumaRepoRoot "Bin\$BuildConfig\PumaServerPgTest.exe"
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
    # (Puma) restore a backup into it afterwards; ProLifeServerTest.exe is
    # expected to create prolife_test (and run its migrations) on startup
    # when it doesn't already exist, same as ProLifeApiPostman's own CI
    # script assumes for that database.
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
    # PumaServerPgTest.exe would happily create-and-migrate an EMPTY
    # database from scratch on startup, but CreateSuperuser/RoleAdd/UserAdd
    # (called by this suite's own "00 Bootstrap" folder) need a Puma
    # instance backed by a real, populated export to behave the same way
    # confirmed working against ProLifeApiPostman/DeskTicketApiPostman - see
    # Tests\ProLifeApiPostman\Run-CiTests.ps1's -ProLifeBackupPath docs for
    # the full rationale (shared here since -PumaBackupPath defaults to that
    # suite's puma.backup).
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

function Wait-ForPort([string]$serverLabel, [System.Diagnostics.Process]$process, [int]$port, [string]$stdoutFile = "", [string]$stderrFile = "") {
    $deadline = (Get-Date).AddSeconds($StartupTimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        if ($process.HasExited) {
            Write-Host "`n--- $serverLabel stdout ---" -ForegroundColor Yellow
            if ($stdoutFile -and (Test-Path $stdoutFile)) { Get-Content $stdoutFile | Write-Host }
            else { Write-Host "(no stdout captured)" }
            Write-Host "`n--- $serverLabel stderr ---" -ForegroundColor Yellow
            if ($stderrFile -and (Test-Path $stderrFile)) { Get-Content $stderrFile | Write-Host }
            else { Write-Host "(no stderr captured)" }
            Write-Host "---" -ForegroundColor Yellow
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
    $script:pumaStdout = Join-Path $env:TEMP "puma_stdout_$PID.log"
    $script:pumaStderr = Join-Path $env:TEMP "puma_stderr_$PID.log"
    $script:pumaProcess = Start-Process -FilePath $PumaServerExePath -WorkingDirectory $workDir -PassThru -WindowStyle Hidden -RedirectStandardOutput $script:pumaStdout -RedirectStandardError $script:pumaStderr
    Write-Host "Started PID $($script:pumaProcess.Id)"
    Wait-ForPort "PumaServerPgTest.exe" $script:pumaProcess $PumaHttpPort $script:pumaStdout $script:pumaStderr
    Write-Host "Puma test server is accepting connections on port $PumaHttpPort"
}

function Start-TestServer {
    Write-Step "Starting ProLifeServerTest.exe"
    if (-not (Test-Path $ServerExePath)) {
        throw "Server executable not found: $ServerExePath"
    }
    $workDir = Split-Path -Parent $ServerExePath
    $script:serverStdout = Join-Path $env:TEMP "prolife_stdout_$PID.log"
    $script:serverStderr = Join-Path $env:TEMP "prolife_stderr_$PID.log"
    $script:serverProcess = Start-Process -FilePath $ServerExePath -WorkingDirectory $workDir -PassThru -WindowStyle Hidden -RedirectStandardOutput $script:serverStdout -RedirectStandardError $script:serverStderr
    Write-Host "Started PID $($script:serverProcess.Id)"
    Wait-ForPort "ProLifeServerTest.exe" $script:serverProcess $HttpPort $script:serverStdout $script:serverStderr
    Write-Host "Server is accepting connections on port $HttpPort"
}

function Install-NewmanIfNeeded {
    $newmanCmd = Join-Path $ScriptDir "node_modules\.bin\newman.cmd"
    if (Test-Path $newmanCmd) {
        return $newmanCmd
    }

    Write-Step "Installing newman (npm install in $ScriptDir)"
    Push-Location $ScriptDir
    try {
        & npm install --no-audit --no-fund | Out-Host
        if ($LASTEXITCODE -ne 0) { throw "npm install failed (exit $LASTEXITCODE)" }
    }
    finally {
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
    # NOT .../ProLife/graphql — the collection templates every URL as "{{baseUrl}}/graphql"
    $baseUrl = "http://localhost:$HttpPort/ProLife"

    # Folders 90/91 are deliberately excluded by default - see README
    # ("Known Limitations / Opt-in folders"): 90 demonstrates omitted required input arg error,
    # and 91 only means anything after a real ~5 minute wait between two of its requests.
    $defaultFolders = @(
        "00 Bootstrap",
        "01 CreateToken - Validation and Success",
        "02 GetToken and GetTokenList",
        "03 ValidateToken",
        "04 RevokeToken",
        "05 DeleteToken",
        "06 Scope Intersection - User Permissions $([char]0x2229) Token Scopes",
        "07 Ownership and Cross-User Access (no enforcement, source-confirmed)",
        "08 Comma-in-Scope Storage Edge Case",
        "99 Cleanup"
    )
    $optInFolders = @(
        "90 Exploratory - Omitted Input (informational, opt-in, never in default run)",
        "91 Auth-Cache Staleness (slow, opt-in, never in default run)"
    )
    $folders = if ($IncludeOptInFolders) { $defaultFolders + $optInFolders } else { $defaultFolders }

    $newmanArgs = @("run", $CollectionPath, "-e", $EnvironmentPath, "--env-var", "baseUrl=$baseUrl")
    foreach ($f in $folders) { $newmanArgs += @("--folder", $f) }
    $newmanArgs += @(
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
Write-Host "ScriptDir:         $ScriptDir"
Write-Host "ServerExePath:     $ServerExePath"
Write-Host "PumaServerExePath: $PumaServerExePath"
Write-Host "PumaBackupPath:    $PumaBackupPath"
Write-Host "CollectionPath:    $CollectionPath"
Write-Host "EnvironmentPath:   $EnvironmentPath"

try {
    # Stop stale processes from a previous, possibly-crashed run before
    # touching any database - ProLifeServerTest talks to Puma over HTTP for
    # CreateSuperuser/Authorization/RoleAdd/UserAdd (this suite's own
    # bootstrap), so a stale Puma left bound to the port would make the new
    # instance fail to start rather than the new one being used.
    Stop-ServerProcess "ProLifeServerTest"
    Stop-ServerProcess "PumaServerPgTest"

    Restore-DatabaseFromBackup $PumaDbName $PumaBackupPath
    Start-PumaTestServer

    Reset-Database $DbName
    Start-TestServer

    $exitCode = Invoke-NewmanSuite
}
finally {
    # Tear down in reverse start order: ProLife depends on Puma being up,
    # not the other way round, so it's safe (and cleanest) to stop it first.
    if ($serverProcess -and -not $serverProcess.HasExited) {
        Write-Step "Stopping ProLifeServerTest.exe (PID $($serverProcess.Id))"
        Stop-Process -Id $serverProcess.Id -Force -ErrorAction SilentlyContinue
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
