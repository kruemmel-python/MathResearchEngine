param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$candidates = @(
    (Join-Path $PSScriptRoot 'bin\math_research_ide.exe'),
    (Join-Path $PSScriptRoot "build-windows\$Configuration\math_research_ide.exe")
)
$exe = $candidates | Where-Object { Test-Path -LiteralPath $_ -PathType Leaf } | Select-Object -First 1
if (-not $exe) {
    throw 'IDE executable not found. Run .\build-windows.ps1 first.'
}

Start-Process -FilePath $exe -WorkingDirectory $PSScriptRoot
