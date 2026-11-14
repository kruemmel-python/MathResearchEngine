#pragma once

#include "common.hpp"

namespace mre {

struct FormulaDocument {
    std::string name;
    std::string formula;
    std::string domain = "real";
    uint64_t seed = 42;
};

FormulaDocument loadFormulaDocument(const fs::path& path);
std::string analyzeFormulaDocument(const FormulaDocument& document, int& exit_code);
int runFormulaAnalysis(const fs::path& input_path, const fs::path& output_path = {});

} // namespace mre
