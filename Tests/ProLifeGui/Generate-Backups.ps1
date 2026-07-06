# SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
<#
.SYNOPSIS
    One-off (re)generator for Tests\ProLifeGui\puma.backup: restores the real,
    populated legacy backups (Tests\ProLifeApiPostman\{puma,lisa,prolife}.backup)
    into puma_test/lisa_test/prolife_test, boots all three test servers, bakes
    in the 8 ProLifeGui fixture roles/users (fixtures/users.js, via
    scripts/seed-fixture-users.js) on top of the real ~725 existing Puma
    users, then dumps puma_test back out to Tests\ProLifeGui\puma.backup.

    Only puma_test needs a ProLifeGui-specific derived backup: prolife_test's
    Roles/Users/UserGroups/UserSessions are FOREIGN TABLES (postgres_fdw)
    that read live from puma_test at query time - the seeded fixture users
    show up there automatically once puma_test has them, with no local
    prolife_test data to re-dump. Run-CiTests.ps1 therefore restores
    prolife_test/lisa_test from the plain, shared
    Tests\ProLifeApiPostman\{prolife,lisa}.backup and only puma_test from
    this script's output.

    Re-run this whenever fixtures/users.js changes (new fixture user, renamed
    permission set, etc.) to keep puma.backup in sync - seed-fixture-users.js
    is idempotent, so re-running against an already-seeded puma_test is safe,
    but starting from the pristine legacy puma.backup each time (as this
    script does) keeps the output reproducible.

.PARAMETER RepoRoot
    Root of the ProLife checkout. Defaults to the PROLIFEDIR environment
    variable; if unset, falls back to two levels above this script.

.PARAMETER PumaRepoRoot / LisaRepoRoot
    Roots of the Puma/Lisa checkouts. Same sibling-checkout convention as
    Run-CiTests.ps1 (PUMADIR/LISADIR env vars, falling back to siblings of
    RepoRoot).

.PARAMETER BuildConfig
    Name of the Bin\<BuildConfig> folder to look for the three *Test.exe
    servers in.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File Generate-Backups.ps1
#>

[CmdletBinding()]
param(
    [string]$RepoRoot = $(
        $sd = if ($PSScriptRoot) { $PSScriptRoot } else { (Get-Location).Path }
        (Resolve-Path (Join-Path $sd "..\..")).Path
    ),
    [string]$ScriptDir = (Join-Path $RepoRoot "Tests\ProLifeGui"),
    [string]$SharedBackupDir = (Join-Path $RepoRoot "Tests\ProLifeApiPostman"),
    [string]$BuildConfig = "Debug_Qt6_VC17_x64",

    [string]$PumaRepoRoot = $(if ($env:PUMADIR) { $env:PUMADIR } else { Join-Path (Split-Path -Parent $RepoRoot) "Puma" }),
    [string]$LisaRepoRoot = $(if ($env:LISADIR) { $env:LISADIR } else { Join-Path (Split-Path -Parent $RepoRoot) "Lisa" }),

    [string]$ServerExePath = (Join-Path $RepoRoot "Bin\$BuildConfig\ProLifeServerTest.exe"),
    [string]$PumaServerExePath = (Join-Path $PumaRepoRoot "Bin\$BuildConfig\PumaServerPgTest.exe"),
    [string]$LisaServerExePath = (Join-Path $LisaRepoRoot "Bin\$BuildConfig\LisaServerTest.exe"),

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

    [string]$PumaBackupPath = (Join-Path $SharedBackupDir "puma.backup"),
    [string]$LisaBackupPath = (Join-Path $SharedBackupDir "lisa.backup"),
    [string]$ProLifeBackupPath = (Join-Path $SharedBackupDir "prolife.backup"),
    [string]$OutputPumaBackupPath = (Join-Path $ScriptDir "puma.backup"),

    [string]$SuPassword = "1",
    [string]$PsqlPath = "",
    [int]$StartupTimeoutSeconds = 60
)

$ErrorActionPreference = "Stop"
$serverProcess = $null
$pumaProcess = $null
$lisaProcess = $null

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

function Restore-DatabaseFromBackup([string]$name, [string]$backupPath) {
    if (-not (Test-Path $backupPath)) { throw "Backup file not found: $backupPath" }
    $psql = Resolve-PsqlPath
    $pgRestore = Join-Path (Split-Path -Parent $psql) "pg_restore.exe"
    if (-not (Test-Path $pgRestore)) { throw "pg_restore not found next to psql: $pgRestore" }

    $env:PGPASSWORD = $DbPassword
    $previousEap = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    try {
        Write-Step "Resetting database '$name'"
        & $psql -h $DbHost -p $DbPort -U $DbUser -d postgres -v ON_ERROR_STOP=1 -c "SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE datname = '$name' AND pid <> pg_backend_pid();" 2>&1 | Write-Host
        & $psql -h $DbHost -p $DbPort -U $DbUser -d postgres -v ON_ERROR_STOP=1 -c "DROP DATABASE IF EXISTS $name;" 2>&1 | Write-Host
        if ($LASTEXITCODE -ne 0) { throw "Failed to drop database '$name' (exit $LASTEXITCODE)" }

        Write-Step "Creating database '$name'"
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
    # See the identical note in Tests\ProLifeApiPostman\Run-CiTests.ps1:
    # prolife.backup's Roles/Users/UserGroups/UserSessions are postgres_fdw
    # foreign tables, all backed by a single foreign server "PumaServer"
    # whose srvoptions carry the REAL dbname='puma' from the dev export -
    # must be repointed at puma_test before ProLife (or anything reading
    # through it) touches those tables, or reads/writes silently hit the
    # developer's real local "puma" database instead. (There's a dormant
    # "setup_foreign_table_lisaserver()" helper function in the dump, but
    # it's never invoked - no "LisaServer" foreign server actually exists.)
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
        if ($process.HasExited) { throw "$serverLabel exited prematurely (exit code $($process.ExitCode))" }
        if (Test-NetConnection -ComputerName "localhost" -Port $port -WarningAction SilentlyContinue -InformationLevel Quiet) { return }
        Start-Sleep -Milliseconds 500
    }
    throw "$serverLabel did not open port $port within $StartupTimeoutSeconds seconds"
}

try {
    Stop-ServerProcess "ProLifeServerTest"
    Stop-ServerProcess "LisaServerTest"
    Stop-ServerProcess "PumaServerPgTest"

    Restore-DatabaseFromBackup $PumaDbName $PumaBackupPath
    Write-Step "Starting PumaServerPgTest.exe"
    $pumaProcess = Start-Process -FilePath $PumaServerExePath -WorkingDirectory (Split-Path -Parent $PumaServerExePath) -PassThru -WindowStyle Hidden
    Wait-ForPort "PumaServerPgTest.exe" $pumaProcess $PumaHttpPort
    Write-Host "Puma up on $PumaHttpPort"

    Restore-DatabaseFromBackup $LisaDbName $LisaBackupPath
    Write-Step "Starting LisaServerTest.exe"
    $lisaProcess = Start-Process -FilePath $LisaServerExePath -WorkingDirectory (Split-Path -Parent $LisaServerExePath) -PassThru -WindowStyle Hidden
    Wait-ForPort "LisaServerTest.exe" $lisaProcess $LisaHttpPort
    Write-Host "Lisa up on $LisaHttpPort"

    Restore-DatabaseFromBackup $DbName $ProLifeBackupPath
    Repair-ProLifeForeignServers
    Write-Step "Starting ProLifeServerTest.exe"
    $serverProcess = Start-Process -FilePath $ServerExePath -WorkingDirectory (Split-Path -Parent $ServerExePath) -PassThru -WindowStyle Hidden
    Wait-ForPort "ProLifeServerTest.exe" $serverProcess $HttpPort
    Write-Host "ProLife up on $HttpPort"

    Write-Step "Bootstrapping 'su' superuser (safety net - may already exist in the legacy export)"
    $body = @{ query = 'mutation { CreateSuperuser(input: { password: "' + $SuPassword + '", mail: "su@prolifegui.test", name: "Super User" }) { success message } }' } | ConvertTo-Json -Compress
    $r = Invoke-RestMethod -Uri "http://localhost:$HttpPort/ProLife/graphql" -Method Post -ContentType "application/json" -Body $body
    Write-Host "CreateSuperuser: $($r.data.CreateSuperuser | ConvertTo-Json -Compress)"

    Write-Step "Seeding ProLifeGui fixture roles/users (scripts/seed-fixture-users.js)"
    Push-Location $ScriptDir
    try {
        & node "scripts\seed-fixture-users.js" "http://localhost:$HttpPort" | Out-Host
        if ($LASTEXITCODE -ne 0) { throw "seed-fixture-users.js failed (exit $LASTEXITCODE)" }
    }
    finally {
        Pop-Location
    }
}
finally {
    if ($serverProcess -and -not $serverProcess.HasExited) { Stop-Process -Id $serverProcess.Id -Force -ErrorAction SilentlyContinue }
    if ($lisaProcess -and -not $lisaProcess.HasExited) { Stop-Process -Id $lisaProcess.Id -Force -ErrorAction SilentlyContinue }
    if ($pumaProcess -and -not $pumaProcess.HasExited) { Stop-Process -Id $pumaProcess.Id -Force -ErrorAction SilentlyContinue }
}

Write-Step "Dumping seeded puma_test to $OutputPumaBackupPath"
$psql = Resolve-PsqlPath
$pgDump = Join-Path (Split-Path -Parent $psql) "pg_dump.exe"
if (-not (Test-Path $pgDump)) { throw "pg_dump not found next to psql: $pgDump" }
$env:PGPASSWORD = $DbPassword
$previousEap = $ErrorActionPreference
$ErrorActionPreference = "Continue"
try {
    if (Test-Path $OutputPumaBackupPath) { Remove-Item $OutputPumaBackupPath -Force }
    & $pgDump -h $DbHost -p $DbPort -U $DbUser -d $PumaDbName -F c -f $OutputPumaBackupPath 2>&1 | Write-Host
    if ($LASTEXITCODE -ne 0) { throw "pg_dump failed for '$PumaDbName' (exit $LASTEXITCODE)" }
}
finally {
    $ErrorActionPreference = $previousEap
    Remove-Item Env:\PGPASSWORD -ErrorAction SilentlyContinue
}

Write-Host "`nDone. Wrote $OutputPumaBackupPath" -ForegroundColor Green
