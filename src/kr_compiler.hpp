#pragma once

#include "common.hpp"

namespace mre {

struct KrBuildInvocation {
    bool available = false;
    std::string command;
    std::string provider;
    std::string diagnostic;
};

inline std::string krEnvironmentValue(const char* name) {
#ifdef _WIN32
    char* value = nullptr;
    size_t length = 0;
    if (_dupenv_s(&value, &length, name) != 0) return {};
    std::string result = value ? value : "";
    std::free(value);
    return result;
#else
    const char* value = std::getenv(name);
    return value ? value : "";
#endif
}

inline fs::path normalizeKrRoot(fs::path configured) {
    if (configured.filename() == "toolchain") configured = configured.parent_path();
    return configured;
}

inline KrBuildInvocation resolveKrBuildInvocation(const fs::path& project) {
    KrBuildInvocation invocation;
#ifdef _WIN32
    std::string configuredRoot = krEnvironmentValue("MRE_KR_HOME");
    if (configuredRoot.empty()) configuredRoot = krEnvironmentValue("KR_COMPILER_HOME");
    if (configuredRoot.empty()) {
        const std::string compatibility = krEnvironmentValue("MRE_KR_COMPILER");
        if (!compatibility.empty() && fs::is_directory(compatibility)) configuredRoot = compatibility;
    }

    if (!configuredRoot.empty()) {
        const fs::path root = normalizeKrRoot(fs::path(configuredRoot));
        const fs::path java = root / "runtime" / "jdk" / "bin" / "java.exe";
        const fs::path jar = root / "dist" / "KR-Compiler-IDE.jar";
        const fs::path stamp = root / "toolchain" / "kr-toolchain.stamp";
        if (!fs::is_regular_file(java) || !fs::is_regular_file(jar) ||
            !fs::is_regular_file(stamp)) {
            invocation.diagnostic = "Invalid KR compiler root: expected runtime/jdk/bin/java.exe, "
                "dist/KR-Compiler-IDE.jar and toolchain/kr-toolchain.stamp under " + utf8(root);
            return invocation;
        }
        invocation.available = true;
        invocation.provider = "KR Java CLI (" + utf8(root) + ")";
        invocation.command = shellQuote(utf8(java)) + " -cp " + shellQuote(utf8(jar)) +
            " kr.compile.KrCliMain " + shellQuote(utf8(fs::absolute(project))) + " WINDOWS";
        return invocation;
    }

    const std::string legacy = krEnvironmentValue("MRE_KR_COMPILER");
    if (!legacy.empty() && fs::is_regular_file(legacy)) {
        invocation.available = true;
        invocation.provider = "legacy KR wrapper";
        invocation.command = shellQuote(legacy) + " --build " +
            shellQuote(utf8(fs::absolute(project))) + " --target WINDOWS --configuration Release";
        return invocation;
    }
#else
    (void)project;
#endif
    invocation.diagnostic = "KR compiler missing. Set MRE_KR_HOME or KR_COMPILER_HOME to the "
        "portable KR compiler root (the directory containing runtime, dist and toolchain).";
    return invocation;
}

inline fs::path findKrArtifact(const fs::path& project) {
    const fs::path output = project / "KR_BUILD";
    if (!fs::is_directory(output)) return {};
    std::vector<fs::path> executables;
    for (const auto& entry : fs::recursive_directory_iterator(
             output, fs::directory_options::skip_permission_denied)) {
        if (entry.is_regular_file() && entry.path().extension() == ".exe") {
            const std::string pathText = utf8(entry.path());
            const std::string filename = utf8(entry.path().filename());
            if (filename == "a.exe" || filename == "programm.exe" ||
                pathText.find("\\cmake_build\\") != std::string::npos ||
                pathText.find("\\.cache\\") != std::string::npos ||
                pathText.find("\\CMakeFiles\\") != std::string::npos) {
                continue;
            }
            executables.push_back(entry.path());
        }
    }
    std::sort(executables.begin(), executables.end(), [](const fs::path& a, const fs::path& b) {
        std::error_code aError, bError;
        const auto aSize = fs::file_size(a, aError);
        const auto bSize = fs::file_size(b, bError);
        if (!aError && !bError && aSize != bSize) return aSize > bSize;
        return a < b;
    });
    return executables.empty() ? fs::path{} : executables.front();
}

} // namespace mre
