# MathResearchEngine

MathResearchEngine is a C++20 research environment for generating, testing and auditing mathematical hypotheses. It combines numerical counterexample searches, SQLite-backed research runs, required Lean 4 verification and a native Windows research IDE.

## Build on Windows

Requirements:

- Windows 10 or 11, x64
- Visual Studio 2022 with **Desktop development with C++**
- CMake 3.21 or newer available on `PATH`
- PowerShell 5.1 or newer
- Python 3 is optional and enables the integration tests
- Internet access for the first source build, so the pinned Lean 4.34.1 toolchain can be installed

Clone and build:

```powershell
git clone https://github.com/kruemmel-python/MathResearchEngine.git
Set-Location MathResearchEngine
.\build-windows.ps1
```

The script installs the version from `lean-toolchain` when necessary, configures the complete project, compiles all current CMake targets and runs CTest. All 18 engine tests, including the four real Lean invocations, are mandatory. Binaries are written to `build-windows\Release`.

Start the native IDE:

```powershell
.\run-ide.ps1
```

Create the redistributable Windows ZIP, including the reduced Lean runtime:

```powershell
.\package-windows.ps1
```

Run the CLI self-test directly:

```powershell
.\build-windows\Release\math_research_v03.exe selftest .\build-windows\selftest
```

Show CLI usage:

```powershell
.\build-windows\Release\math_research_v03.exe
```

## Formula analysis

The repository contains documented examples in `formula_examples`:

```powershell
.\build-windows\Release\math_research_v03.exe formula-check `
  .\formula_examples\binomische_formel.mreformula `
  .\work\binomische_formel_report.txt
```

`formula-check` performs numerical samples and counterexample searches. A successful numerical run is not presented as a formal proof.

## Lean 4 verification

The Windows release contains a reduced Lean 4.34.1 runtime in `lean-runtime`; no separate installation is needed. Source builds use `setup-lean.ps1` and the pinned `lean-toolchain`. The engine recognizes:

- `MRE_LEAN_EXE`: full path to the Lean executable
- `MRE_LEAN_PATH`: semicolon-separated Lean library roots on Windows
- `LEAN_PATH`: standard Lean search path
- `MRE_LEAN_TIMEOUT_MS`: verification timeout

The files in `lean/` implement the engine's Lean extraction and semantic-probe helpers. Lean decides elaboration and proof success; missing modules are reported as infrastructure errors rather than mathematical failures. Large local Mathlib/OAI caches are not embedded in the release; workflows that import those libraries must provide their roots through `MRE_LEAN_PATH` or `LEAN_PATH`.

## Optional services

- Gemini integration reads `GEMINI_API_KEY` from the environment or from the IDE's RAM-only password field. Keys are not stored in this repository.
- LM Studio can be selected as a local OpenAI-compatible endpoint.
- The portable KR compiler can be selected with `MRE_KR_HOME` or `KR_COMPILER_HOME`. Point it to the directory containing `runtime`, `dist` and `toolchain`; a supplied `toolchain` subdirectory is normalized to its parent. The engine invokes `runtime\jdk\bin\java.exe -cp dist\KR-Compiler-IDE.jar kr.compile.KrCliMain`. `MRE_KR_COMPILER` remains supported only for legacy wrapper executables.

Validate or use the configured KR compiler:

```powershell
$env:MRE_KR_HOME = 'C:\Tools\KR-Compiler-Windows-x64'
.\build-windows\Release\math_research_v03.exe kr-check .
.\build-windows\Release\math_research_v03.exe kr-build C:\Path\To\KrProject
```

The repository also provides a validation wrapper which accepts either the KR root or its `toolchain` subdirectory:

```powershell
.\test-kr-integration.ps1 -KrHome C:\Tools\KR-Compiler-Windows-x64\toolchain
```

The multi-gigabyte KR distribution is maintained as its own dependency and is not duplicated inside the MathResearchEngine release. Its internal version markers should be checked independently; MathResearchEngine validates the required runtime, JAR and toolchain stamp instead of inferring compatibility from the enclosing directory name. Without KR, normal MSVC/CMake builds remain available.

## Main targets

- `math_research_v03`: central CLI and research engine
- `math_research_ide`: native Windows IDE
- `math_k4d_research`: standalone K4D research runner
- `math_application_discovery`, `math_application_evaluator`
- `math_autonomous_research`, `math_application_language`
- `math_open_goal_lab`, `math_meta_goal_lab`, `math_open_purpose_v14`
- `math_semantic_catalog`, `math_semantic_discovery`, `math_multi_relation`
- `math_research_audit`, `math_software_lab`, `math_application_designer`

## Repository hygiene

The repository intentionally excludes generated databases, local research results, build directories, cached dependencies, API credentials, temporary candidate projects and historical repair reports. The source tree represents the targets currently declared by `CMakeLists.txt`.

## Scientific scope

The engine distinguishes numerical evidence, counterexample searches, type-correct Lean elaboration and formal proof status. Generated hypotheses and simulations remain model-dependent and are not automatically empirical claims about nature.

## License

No open-source license has been selected yet. The source is published for inspection and reproducible development; standard copyright rules apply until a license is added by the project owner.
