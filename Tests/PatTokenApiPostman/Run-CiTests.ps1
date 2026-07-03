# SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
<#
.SYNOPSIS
    CI entry point for the PatTokenApi Postman suite: resets the
    prolife_test database, starts ProLifeServerTest.exe, runs newman
    against it, and tears the server back down. Intended to be invoked as
    a single TeamCity build step (PowerShell runner). Mirrors
    Tests\ProLifeApiPostman\Run-CiTests.ps1 (same isolated test server/DB;
    run this as its own sequential CI step, not concurrently with
    ProLifeApiPostman's own CI run).

.PARAMETER RepoRoot
    Root of the ProLife checkout. Defaults to the PROLIFEDIR environment
    variable; if PROLIFEDIR isn't set, falls back to two levels above this
    script (Tests\PatTokenApiPostman\..\..).

.PARAMETER BuildConfig
    Name of the Bin\<BuildConfig> folder to look for ProLifeServerTest.exe
    in (e.g. "Release_Qt6_VC17_x64"). Ignored if -ServerExePath is passed
    explicitly.

.PARAMETER ServerExePath
    Full path to ProLifeServerTest.exe. Overrides -BuildConfig when set.

.PARAMETER HttpPort
    Port the test server listens on (must match ProLifeServerTest.acc's
    HttpPort attribute, currently 17778 - same test server ProLifeApiPostman
    uses).

.PARAMETER DbName / DbHost / DbPort / DbUser / DbPassword
    Postgres connection used to drop/recreate the test database before
    each run.

.PARAMETER PsqlPath
    Full path to psql.exe. If omitted, resolved from PATH, then from the
    default PostgreSQL install locations (highest version wins).

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
    [string]$ServerExePath = "",
    [int]$HttpPort = 17778,
    [string]$DbName = "prolife_test",
    [string]$DbHost = "localhost",
    [int]$DbPort = 5432,
    [string]$DbUser = "postgres",
    [string]$DbPassword = "root",
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
$exitCode = 1

if ([string]::IsNullOrWhiteSpace($ServerExePath)) {
    $ServerExePath = Join-Path $RepoRoot "Bin\$BuildConfig\ProLifeServerTest.exe"
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

function Stop-TestServer {
    Write-Step "Stopping any running ProLifeServerTest.exe"
    Get-Process -Name "ProLifeServerTest" -ErrorAction SilentlyContinue | ForEach-Object {
        Write-Host "Killing PID $($_.Id)"
        Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue
    }
    Start-Sleep -Seconds 2
}

function Reset-TestDatabase {
    # Only drops the database if present. ProLifeServerTest.exe is expected
    # to create it (and run its migrations) on startup when it doesn't
    # already exist, same as ProLifeApiPostman's own CI script assumes.
    Write-Step "Resetting database '$DbName'"
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
        & $psql -h $DbHost -p $DbPort -U $DbUser -d postgres -v ON_ERROR_STOP=1 -c "SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE datname = '$DbName' AND pid <> pg_backend_pid();" 2>&1 | Write-Host
        & $psql -h $DbHost -p $DbPort -U $DbUser -d postgres -v ON_ERROR_STOP=1 -c "DROP DATABASE IF EXISTS $DbName;" 2>&1 | Write-Host
        if ($LASTEXITCODE -ne 0) { throw "Failed to drop database '$DbName' (exit $LASTEXITCODE)" }
    }
    finally {
        $ErrorActionPreference = $previousEap
        Remove-Item Env:\PGPASSWORD -ErrorAction SilentlyContinue
    }
}

function Start-TestServer {
    Write-Step "Starting ProLifeServerTest.exe"
    if (-not (Test-Path $ServerExePath)) {
        throw "Server executable not found: $ServerExePath"
    }
    $workDir = Split-Path -Parent $ServerExePath
    $script:serverProcess = Start-Process -FilePath $ServerExePath -WorkingDirectory $workDir -PassThru -WindowStyle Hidden
    Write-Host "Started PID $($script:serverProcess.Id)"

    $deadline = (Get-Date).AddSeconds($StartupTimeoutSeconds)
    $ready = $false
    while ((Get-Date) -lt $deadline) {
        if ($script:serverProcess.HasExited) {
            throw "ProLifeServerTest.exe exited prematurely (exit code $($script:serverProcess.ExitCode))"
        }
        $probe = Test-NetConnection -ComputerName "localhost" -Port $HttpPort -WarningAction SilentlyContinue -InformationLevel Quiet
        if ($probe) { $ready = $true; break }
        Start-Sleep -Milliseconds 500
    }
    if (-not $ready) {
        throw "Server did not open port $HttpPort within $StartupTimeoutSeconds seconds"
    }
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
    $baseUrl = "http://localhost:$HttpPort/ProLife/graphql"

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
Write-Host "RepoRoot:       $RepoRoot  (source: $repoRootSource)"
Write-Host "ScriptDir:      $ScriptDir"
Write-Host "ServerExePath:  $ServerExePath"
Write-Host "CollectionPath: $CollectionPath"
Write-Host "EnvironmentPath: $EnvironmentPath"

try {
    Stop-TestServer
    Reset-TestDatabase
    Start-TestServer
    $exitCode = Invoke-NewmanSuite
}
finally {
    if ($serverProcess -and -not $serverProcess.HasExited) {
        Write-Step "Stopping ProLifeServerTest.exe (PID $($serverProcess.Id))"
        Stop-Process -Id $serverProcess.Id -Force -ErrorAction SilentlyContinue
    }
}

if ($exitCode -eq 0) {
    Write-Host "`nAll tests passed." -ForegroundColor Green
} else {
    Write-Host "`nTest run failed (newman exit code $exitCode)." -ForegroundColor Red
}

exit $exitCode
