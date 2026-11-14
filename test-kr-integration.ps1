param(
    [Parameter(Mandatory = $true)]
    [string]$KrHome,
    [string]$ProjectDirectory
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = $PSScriptRoot
$configured = [System.IO.Path]::GetFullPath($KrHome)
if ((Split-Path -Leaf $configured) -eq 'toolchain') {
    $configured = Split-Path -Parent $configured
}

foreach ($required in @(
    'runtime\jdk\bin\java.exe',
    'dist\KR-Compiler-IDE.jar',
    'toolchain\kr-toolchain.stamp'
)) {
    $path = Join-Path $configured $required
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "KR component missing: $path"
    }
}

$engine = Join-Path $root 'build-windows\Release\math_research_v03.exe'
if (!(Test-Path -LiteralPath $engine -PathType Leaf)) {
    throw 'MathResearchEngine is not built. Run .\build-windows.ps1 first.'
}

$env:MRE_KR_HOME = $configured
& $engine kr-check $root
if ($LASTEXITCODE -ne 0) { throw 'KR configuration check failed.' }

if ($ProjectDirectory) {
    $project = [System.IO.Path]::GetFullPath($ProjectDirectory)
    & $engine kr-build $project
    if ($LASTEXITCODE -ne 0) { throw 'KR project build failed.' }
}

Write-Host "KR integration passed: $configured" -ForegroundColor Green
