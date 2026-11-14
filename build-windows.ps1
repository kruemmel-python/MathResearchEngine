param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release',
    [string]$BuildDirectory = 'build-windows'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = $PSScriptRoot
$build = Join-Path $root $BuildDirectory
$nativeCmake = Join-Path $env:ProgramFiles 'CMake\bin\cmake.exe'
$cmake = if (Test-Path -LiteralPath $nativeCmake -PathType Leaf) {
    $nativeCmake
} else {
    (Get-Command cmake -ErrorAction Stop).Source
}

& $cmake -S $root -B $build -G 'Visual Studio 17 2022' -A x64
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }

& $cmake --build $build --config $Configuration --parallel
if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }

& $cmake --build $build --config $Configuration --target RUN_TESTS
if ($LASTEXITCODE -ne 0) { throw 'CTest failed.' }

Write-Host "MathResearchEngine build and tests passed: $build\\$Configuration" -ForegroundColor Green
