Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = $PSScriptRoot
$bundled = Join-Path $root 'lean-runtime\bin\lean.exe'
if (Test-Path -LiteralPath $bundled -PathType Leaf) {
    $env:MRE_LEAN_EXE = $bundled
    Write-Host "Using bundled Lean: $bundled"
    exit 0
}

$toolchain = (Get-Content -LiteralPath (Join-Path $root 'lean-toolchain') -Raw).Trim()
$elan = Get-Command elan -ErrorAction SilentlyContinue
if (-not $elan) {
    $installer = Join-Path ([System.IO.Path]::GetTempPath()) 'mre-elan-init.exe'
    Invoke-WebRequest -Uri 'https://github.com/leanprover/elan/releases/latest/download/elan-x86_64-pc-windows-msvc.exe' -OutFile $installer
    & $installer -y --default-toolchain $toolchain
    if ($LASTEXITCODE -ne 0) { throw 'elan installation failed.' }
    $elanPath = Join-Path $env:USERPROFILE '.elan\bin'
    $env:PATH = "$elanPath;$env:PATH"
    $elan = Get-Command elan -ErrorAction Stop
}

& $elan.Source toolchain install $toolchain
if ($LASTEXITCODE -ne 0) { throw "Lean toolchain installation failed: $toolchain" }

$lean = Join-Path $env:USERPROFILE '.elan\bin\lean.exe'
if (!(Test-Path -LiteralPath $lean -PathType Leaf)) { throw "Lean executable not found: $lean" }
$env:MRE_LEAN_EXE = $lean
& $lean --version
