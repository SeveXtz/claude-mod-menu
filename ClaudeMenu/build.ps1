# Compila o ClaudeMenu em Release. Uso:  .\build.ps1   (ou  .\build.ps1 -Arch Win32  para jogos 32-bit)
param([ValidateSet('x64', 'Win32')] [string]$Arch = 'x64')
$ErrorActionPreference = 'Stop'

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { throw 'Visual Studio / Build Tools 2022 nao encontrado.' }
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw 'Instale o workload "Desenvolvimento para desktop com C++".' }

$cmake = Join-Path $vs 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
if (-not (Test-Path $cmake)) { $cmake = (Get-Command cmake -ErrorAction Stop).Source }

$build = Join-Path $PSScriptRoot "build\$Arch"
& $cmake -S $PSScriptRoot -B $build -G 'Visual Studio 17 2022' -A $Arch
if ($LASTEXITCODE) { exit $LASTEXITCODE }
& $cmake --build $build --config Release
if ($LASTEXITCODE) { exit $LASTEXITCODE }

$exe = Join-Path $build 'Release\ClaudeMenu.exe'
Write-Host "`nPronto: $exe" -ForegroundColor Green
