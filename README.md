# MathResearchEngine

MathResearchEngine is a C++20 research environment for generating, testing and auditing mathematical hypotheses. It combines numerical counterexample searches, SQLite-backed research runs, optional Lean 4 verification and a native Windows research IDE.

## Build on Windows

Requirements:

- Windows 10 or 11, x64
- Visual Studio 2022 with **Desktop development with C++**
- CMake 3.21 or newer available on `PATH`
- PowerShell 5.1 or newer
- Python 3 is optional and enables the integration tests

Clone and build:

```powershell
git clone https://github.com/kruemmel-python/MathResearchEngine.git
Set-Location MathResearchEngine
.\build-windows.ps1
```

The script configures the complete project, compiles all current CMake targets and runs CTest. Binaries are written to `build-windows\Release`.

Start the native IDE:

```powershell
.\run-ide.ps1
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

## Optional Lean 4 integration

Install Lean with `elan` and make `lean` available on `PATH`. The engine also recognizes:

- `MRE_LEAN_EXE`: full path to the Lean executable
- `MRE_LEAN_PATH`: semicolon-separated Lean library roots on Windows
- `LEAN_PATH`: standard Lean search path
- `MRE_LEAN_TIMEOUT_MS`: verification timeout

The files in `lean/` implement the engine's Lean extraction and semantic-probe helpers. Lean decides elaboration and proof success; missing modules are reported as infrastructure errors rather than mathematical failures.

## Optional services

- Gemini integration reads `GEMINI_API_KEY` from the environment or from the IDE's RAM-only password field. Keys are not stored in this repository.
- LM Studio can be selected as a local OpenAI-compatible endpoint.
- An external KR compiler can be selected with `MRE_KR_COMPILER`. Without it, normal MSVC/CMake builds remain available.

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
