param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = $PSScriptRoot
& (Join-Path $root 'build-windows.ps1') -Configuration $Configuration
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }

$package = Join-Path $root 'dist\MathResearchEngine-v0.3.1-windows-x64'
$distRoot = Join-Path $root 'dist'
if (!(Test-Path -LiteralPath $distRoot)) { New-Item -ItemType Directory -Path $distRoot | Out-Null }
if (Test-Path -LiteralPath $package) { Remove-Item -LiteralPath $package -Recurse -Force }

$nativeCmake = Join-Path $env:ProgramFiles 'CMake\bin\cmake.exe'
$cmake = if (Test-Path -LiteralPath $nativeCmake -PathType Leaf) {
    $nativeCmake
} else {
    (Get-Command cmake -ErrorAction Stop).Source
}
& $cmake --install (Join-Path $root 'build-windows') --config $Configuration --prefix $package
if ($LASTEXITCODE -ne 0) { throw 'CMake install failed.' }

$leanExecutable = & elan which lean
if ($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath $leanExecutable -PathType Leaf)) {
    throw 'Pinned Lean toolchain was not found.'
}
$toolchain = Split-Path -Parent (Split-Path -Parent $leanExecutable)
$runtime = Join-Path $package 'lean-runtime'
New-Item -ItemType Directory -Path (Join-Path $runtime 'lib\lean') -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $toolchain 'bin') -Destination $runtime -Recurse
Copy-Item -LiteralPath (Join-Path $toolchain 'LICENSE') -Destination $runtime
foreach ($pattern in @('*.olean', '*.olean.private', '*.olean.server')) {
    robocopy (Join-Path $toolchain 'lib\lean') (Join-Path $runtime 'lib\lean') $pattern /S /NFL /NDL /NJH /NJS /NP | Out-Null
    if ($LASTEXITCODE -gt 7) { throw "Lean runtime copy failed: $pattern" }
}

$env:MRE_LEAN_EXE = Join-Path $runtime 'bin\lean.exe'
$env:MRE_TEST_CORPUS = Join-Path $package 'tests\corpus'
& (Join-Path $package 'bin\math_research_v03.exe') selftest (Join-Path $package 'selftest')
if ($LASTEXITCODE -ne 0) { throw 'Packaged engine/Lean self-test failed.' }

$zip = "$package.zip"
if (Test-Path -LiteralPath $zip) { Remove-Item -LiteralPath $zip -Force }
tar.exe -a -cf $zip -C $distRoot (Split-Path -Leaf $package)
if ($LASTEXITCODE -ne 0) { throw 'ZIP creation failed.' }

$hash = (Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash
Set-Content -LiteralPath "$zip.sha256.txt" -Value "$hash  $(Split-Path -Leaf $zip)" -Encoding ascii
Write-Host "Package: $zip"
Write-Host "SHA256: $hash"
