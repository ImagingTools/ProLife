# SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
<#
.SYNOPSIS
    CI entry point for the ProLifeGui Playwright suite: starts the
    PumaServerPgTest.exe / LisaServerTest.exe dependencies (ProLifeServerTest
    talks to both over HTTP - see ProLifeServerTest.acc's PumaHttpPort/
    LisaHttpPort), restores puma_test/lisa_test/prolife_test from real
    pg_dump backups (see -PumaBackupPath/-LisaBackupPath/-ProLifeBackupPath
    below for why an empty schema isn't enough), starts ProLifeServerTest.exe
    (which also serves the ProLife WASM app this suite drives via
    Playwright), runs the Playwright suite against it, and tears all three
    servers back down in reverse order. Intended to be invoked as a single
    TeamCity build step (PowerShell runner). Mirrors
    Tests\ProLifeApiPostman\Run-CiTests.ps1 (same dependency startup/
    teardown; swaps newman for Playwright) - run this as its own sequential
    CI step, not concurrently with ProLifeApiPostman's own CI run (both use
    the same test servers/ports/databases).

.PARAMETER RepoRoot
    Root of the ProLife checkout. Defaults to the PROLIFEDIR environment
    variable; if PROLIFEDIR isn't set, falls back to two levels above this
    script (Tests\ProLifeGui\..\..).

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

.PARAMETER ServerExePath / PumaServerExePath / LisaServerExePath
    Full paths to ProLifeServerTest.exe / PumaServerPgTest.exe /
    LisaServerTest.exe. Each overrides -BuildConfig when set.

.PARAMETER HttpPort / PumaHttpPort / LisaHttpPort
    Ports the ProLife/Puma/Lisa test servers listen on (must match
    ProLifeServerTest.acc's HttpPort, PumaServerPgTest.acc's HttpPort and
    LisaServerTest.acc's DefaultHttpPort - currently 17778/17788/17776).
    $HttpPort also becomes the PROLIFE_BASE_URL Playwright's
    playwright.config.js / global-setup.js drive the app at.

.PARAMETER DbName / PumaDbName / LisaDbName / DbHost / DbPort / DbUser / DbPassword
    Postgres connection and database names for prolife_test/puma_test/
    lisa_test (must match ProLifeServerSettings.acc / PumaServerPgTest.acc /
    LisaServerTest.acc's DatabaseAccessSettings defaults). All three are
    restored from a backup before every run (see -ProLifeBackupPath/
    -PumaBackupPath/-LisaBackupPath) rather than left for the server to
    create/migrate from scratch.

.PARAMETER ProLifeBackupPath / PumaBackupPath / LisaBackupPath
    Paths to the pg_dump custom-format backups used to (re)create
    prolife_test / puma_test / lisa_test before each run.
    -ProLifeBackupPath and -LisaBackupPath default to the plain,
    real-dev-export backups shared with Tests\ProLifeApiPostman
    (Tests\ProLifeApiPostman\prolife.backup / lisa.backup) - a real,
    populated database is required because DeviceCollectionPage/
    SoftwareCollectionPage/etc. tests need actual rows to select, and no
    GraphQL mutation in this codebase can create the catalog/type records
    (device types, software catalog entries) ProLife's own DeviceAdd/
    SoftwareProductAdd depend on - see the identical, more detailed note in
    Tests\ProLifeApiPostman\Run-CiTests.ps1.
    -PumaBackupPath instead defaults to Tests\ProLifeGui\puma.backup - a
    ProLifeGui-specific derived backup (see Generate-Backups.ps1) that
    layers the 8 fixture roles/users from fixtures/users.js on top of the
    same real Puma export, so global-setup.js only ever needs to log in
    (never create them via GraphQL - see fixtures/seed.js's header comment).
    Regenerate it with Generate-Backups.ps1 whenever fixtures/users.js
    changes. prolife_test doesn't need its own derived copy: its Roles/
    Users/UserGroups/UserSessions are postgres_fdw FOREIGN TABLES that read
    live from puma_test at query time (see Repair-ProLifeForeignServers
    below), so the fixture users seeded into puma_test are visible from
    prolife_test automatically, with nothing to bake in on the ProLife side.

.PARAMETER SuPassword
    Password used to bootstrap the "su" superuser (via the CreateSuperuser
    GraphQL mutation) once ProLifeServerTest.exe is up - a safety net only.
    Tests\ProLifeGui\puma.backup already contains a working "su" account
    (baked in by the same Generate-Backups.ps1 pipeline, alongside the 8
    fixture users), so this call is expected to no-op with "Superuser
    already exists" in normal operation; it only matters if -PumaBackupPath
    is pointed at a backup that doesn't have "su" yet. Must match the `su`
    entry's password in fixtures/users.js (currently "1") - global-setup.js
    logs in with that hardcoded value, not this parameter, so the two need
    to be kept in sync if either changes.

.PARAMETER PsqlPath
    Full path to psql.exe. If omitted, resolved from PATH, then from the
    default PostgreSQL install locations (highest version wins). Pass
    this explicitly if the CI agent has Postgres installed somewhere
    non-standard. Also used to resolve pg_restore.exe (same bin folder).

.PARAMETER JUnitReportPath
    Where Playwright's junit reporter writes its XML report (see
    playwright.config.js - active whenever the CI env var is set, which
    this script sets before invoking Playwright). Point TeamCity's "XML
    Report Processing" (JUnit) build feature at this same path.

.PARAMETER PlaywrightArgs
    Extra arguments appended verbatim to "npx playwright test" - e.g. a spec
    path to scope the run to one page (-PlaywrightArgs
    "tests/devices.collection.multiuser.test.js") or a project filter
    (-PlaywrightArgs "--project=hardwareManager"). Empty by default, which
    runs the full suite across every ACTIVE project (see -AllUsers). The
    environment (Puma/Lisa/ProLife servers, databases) is always brought up
    in full regardless of this filter.

.PARAMETER AllUsers
    By default only fixtures/users.js's DEFAULT_USER_KEYS (su + fullAccess)
    get a Playwright project/storageState - fast for iterative runs, but
    doesn't exercise per-user permission restrictions
    (accountsViewer/noAccess/etc. seeing less than fullAccess). Pass
    -AllUsers to run the complete multi-user permission matrix (sets
    PROLIFE_GUI_ALL_USERS=1, which playwright.config.js and global-setup.js
    both read).

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1 `
        -BuildConfig "Release_Qt6_VC17_x64" -DbPassword "%db.password%"
#>

[CmdletBinding()]
param(
    # See the identical rationale in Tests\ProLifeApiPostman\Run-CiTests.ps1
    # for why this isn't resolved purely from $PSScriptRoot/etc.
    [string]$RepoRoot = $(
        $configRelPath = "Tests\ProLifeGui\playwright.config.js"
        if ($env:PROLIFEDIR -and (Test-Path (Join-Path $env:PROLIFEDIR $configRelPath))) {
            $env:PROLIFEDIR
        }
        elseif (Test-Path (Join-Path (Get-Location).Path $configRelPath)) {
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
    [string]$ScriptDir = (Join-Path $RepoRoot "Tests\ProLifeGui"),
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

    [string]$ProLifeBackupPath = (Join-Path $RepoRoot "Tests\ProLifeApiPostman\prolife.backup"),
    [string]$PumaBackupPath = (Join-Path $ScriptDir "puma.backup"),
    [string]$LisaBackupPath = (Join-Path $RepoRoot "Tests\ProLifeApiPostman\lisa.backup"),

    # Safety net only - Tests\ProLifeGui\puma.backup already has a working
    # "su" account baked in (see -PumaBackupPath above). Login is always
    # "su" (hardcoded both here and in fixtures/users.js); only the password
    # is configurable.
    [string]$SuPassword = "1",

    [string]$PsqlPath = "",
    [string]$JUnitReportPath = (Join-Path $ScriptDir "junit-report.xml"),
    [int]$StartupTimeoutSeconds = 60,

    # Extra args appended verbatim to "npx playwright test", e.g. a spec path to scope the run to one
    # page's tests (-PlaywrightArgs 'tests/devices.collection.multiuser.test.js') or a project filter
    # (-PlaywrightArgs '--project=hardwareManager'). Empty by default -> full suite, active projects only.
    [string[]]$PlaywrightArgs = @(),

    # By default only fixtures/users.js's DEFAULT_USER_KEYS (su + fullAccess) get a Playwright project -
    # fast for iterative runs, but doesn't validate per-user restrictions. Pass -AllUsers to set
    # PROLIFE_GUI_ALL_USERS=1 and run the complete multi-user permission matrix instead.
    [switch]$AllUsers
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
    # but the collection-page tests (DeviceCollectionPage, SoftwareCollectionPage,
    # etc.) need real rows to select/filter/sort, and ProLife's own DeviceAdd/
    # SoftwareProductAdd need catalog/type records no mutation in this
    # codebase can create - so every database this suite touches is
    # restored from a real, populated backup instead of left empty.
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
    # TABLES (postgres_fdw) backed by a single foreign server, "PumaServer",
    # whose srvoptions carry the REAL dbname='puma' from the dev export -
    # must be repointed at puma_test before ProLife (or anything reading
    # through it, including this suite's fixture-user logins) touches those
    # tables, or reads/writes silently hit a developer's real local "puma"
    # database instead. See the identical, more detailed note in
    # Tests\ProLifeApiPostman\Run-CiTests.ps1. Must run immediately after
    # every prolife_test restore, before ProLifeServerTest.exe is started.
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
            if ($LASTEXITCODE -ne 0) { throw "Failed to repoint FDW server (exit $LASTEXITCODE)" }
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

function New-SuperuserIfNeeded {
    Write-Step "Bootstrapping 'su' superuser via CreateSuperuser"
    $body = @{
        query = 'mutation CreateSuperuser { CreateSuperuser(input: { password: "' + $SuPassword + '", mail: "su@prolifegui.test", name: "Super User" }) { success message } }'
    } | ConvertTo-Json -Compress

    $uri = "http://localhost:$HttpPort/ProLife/graphql"
    $response = Invoke-RestMethod -Uri $uri -Method Post -ContentType "application/json" -Body $body

    if ($response.errors) {
        throw "CreateSuperuser GraphQL call failed: $($response.errors | ConvertTo-Json -Compress)"
    }

    $result = $response.data.CreateSuperuser
    # "Superuser already exists" is expected/harmless on a re-run against a
    # database this script didn't itself just create (e.g. -DbName pointed
    # at a warm DB) - only an unexpected failure should stop the run.
    if (-not $result.success -and $result.message -notmatch "already exists") {
        throw "CreateSuperuser did not succeed: $($result.message)"
    }
    Write-Host "CreateSuperuser: $($result.message)"
}

function Install-PlaywrightIfNeeded {
    # @playwright/test is a local devDependency (package.json) rather than
    # assumed to be installed globally on the agent - a fresh or pooled/
    # reprovisioned TeamCity agent otherwise has no way to run it. Browser
    # binaries are cached outside node_modules (%USERPROFILE%\AppData\
    # Local\ms-playwright by default), so "playwright install" is safe/fast
    # to call unconditionally - it no-ops when already present.
    # npm/npx write routine progress and deprecation warnings to stderr,
    # which $ErrorActionPreference = "Stop" (set globally in this script)
    # would otherwise turn into a spurious terminating NativeCommandError
    # under PowerShell 5.1 even when the process's own exit code is 0 - same
    # pitfall as psql/pg_restore in Run-CiTests.ps1's sibling suites, judged
    # solely by $LASTEXITCODE instead.
    $previousEap = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    try {
        if (-not (Test-Path (Join-Path $ScriptDir "node_modules\@playwright\test"))) {
            Write-Step "Installing Playwright (npm install in $ScriptDir)"
            Push-Location $ScriptDir
            try {
                & npm install --no-audit --no-fund 2>&1 | Out-Host
                if ($LASTEXITCODE -ne 0) { throw "npm install failed (exit $LASTEXITCODE)" }
            }
            finally {
                Pop-Location
            }
        }

        Write-Step "Ensuring Playwright's Chromium browser is installed"
        Push-Location $ScriptDir
        try {
            & npx playwright install chromium 2>&1 | Out-Host
            if ($LASTEXITCODE -ne 0) { throw "playwright install failed (exit $LASTEXITCODE)" }
        }
        finally {
            Pop-Location
        }
    }
    finally {
        $ErrorActionPreference = $previousEap
    }
}

function Invoke-PlaywrightSuite {
    Install-PlaywrightIfNeeded

    Write-Step "Running Playwright suite"
    Push-Location $ScriptDir
    try {
        # playwright.config.js switches its reporter to junit (writing
        # "junit-report.xml" in $ScriptDir, i.e. $JUnitReportPath with the
        # default parameter) whenever CI is set, and also turns on
        # forbidOnly/retries=1 - matching how this suite is meant to run
        # unattended.
        $env:CI = "true"
        $env:PROLIFE_BASE_URL = "http://localhost:$HttpPort"
        if ($AllUsers) { $env:PROLIFE_GUI_ALL_USERS = "1" }
        try {
            & npx playwright test @PlaywrightArgs | Out-Host
            return $LASTEXITCODE
        }
        finally {
            Remove-Item Env:\CI -ErrorAction SilentlyContinue
            Remove-Item Env:\PROLIFE_BASE_URL -ErrorAction SilentlyContinue
            Remove-Item Env:\PROLIFE_GUI_ALL_USERS -ErrorAction SilentlyContinue
        }
    }
    finally {
        Pop-Location
    }
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
Write-Host "JUnitReportPath:   $JUnitReportPath"

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
    New-SuperuserIfNeeded

    $exitCode = Invoke-PlaywrightSuite
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
    Write-Host "`nTest run failed (Playwright exit code $exitCode)." -ForegroundColor Red
}

exit $exitCode
