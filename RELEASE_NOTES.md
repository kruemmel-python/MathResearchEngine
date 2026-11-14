# MathResearchEngine v0.3.0

First clean developer release of the current CMake source tree.

## Included

- Central `math_research_v03` CLI
- Native Windows Research IDE
- Formula counterexample analysis
- SQLite-backed hypothesis and research workflows
- Bundled Lean 4.34.1 runtime, extraction, semantic probes and formal verification
- K4D, semantic, multi-relation, application-discovery and software-lab targets
- Reproducible Visual Studio 2022 build script and Windows CI
- Example `.mreformula` files and Lean helper scripts

## Verification

- Complete Visual Studio 2022 x64 Release build: passed
- CTest: 18/18 passed
- Generated Win32 GUI candidate build/self-test: passed
- Repository credential and local absolute-path scan: passed

## Clean publication scope

Generated databases, historical result folders, cached dependencies, local build trees, API credentials, old repair reports and machine-specific compiler paths are intentionally excluded.

## Requirements

The binary package targets Windows x64 and includes the Lean runtime needed by the engine. Building from source requires Visual Studio 2022 with Desktop C++, CMake 3.21 or newer and internet access for the pinned Lean setup. Gemini/LM Studio and the KR compiler remain optional integrations described in `README.md`.

No open-source license has been selected yet; standard copyright rules apply.
