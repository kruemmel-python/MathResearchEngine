#include "formula_analysis.hpp"
#include "numeric_evaluator.hpp"

#include <map>

namespace mre {
namespace {

std::string withoutBom(std::string value) {
    if (value.size() >= 3 && static_cast<unsigned char>(value[0]) == 0xef &&
        static_cast<unsigned char>(value[1]) == 0xbb && static_cast<unsigned char>(value[2]) == 0xbf) {
        value.erase(0, 3);
    }
    return value;
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

} // namespace

FormulaDocument loadFormulaDocument(const fs::path& path) {
    check(fs::is_regular_file(path), "Formeldatei nicht gefunden: " + utf8(path));
    check(fs::file_size(path) <= 1024U * 1024U, "Formeldatei ist groesser als 1 MiB");

    std::ifstream input(path, std::ios::binary);
    check(static_cast<bool>(input), "Formeldatei kann nicht gelesen werden");

    std::string line;
    check(static_cast<bool>(std::getline(input, line)), "Formeldatei ist leer");
    if (!line.empty() && line.back() == '\r') line.pop_back();
    line = withoutBom(trim(line));
    check(line == "MRE_FORMULA_1", "Erste Zeile muss MRE_FORMULA_1 sein");

    std::map<std::string, std::string> fields;
    size_t line_number = 1;
    while (std::getline(input, line)) {
        ++line_number;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const std::string clean = trim(line);
        if (clean.empty() || clean[0] == '#') continue;
        const size_t separator = clean.find(':');
        check(separator != std::string::npos, "Zeile " + std::to_string(line_number) + ": Schluessel: Wert erwartet");
        const std::string key = lower(trim(clean.substr(0, separator)));
        const std::string value = trim(clean.substr(separator + 1));
        check(!key.empty() && !value.empty(), "Zeile " + std::to_string(line_number) + ": Leerer Schluessel oder Wert");
        check(key == "name" || key == "formula" || key == "domain" || key == "seed",
              "Zeile " + std::to_string(line_number) + ": Unbekanntes Feld " + key);
        check(fields.emplace(key, value).second, "Feld doppelt vorhanden: " + key);
    }

    FormulaDocument document;
    document.name = fields.count("name") ? fields.at("name") : path.stem().string();
    check(fields.count("formula") != 0, "Pflichtfeld formula fehlt");
    document.formula = fields.at("formula");
    if (fields.count("domain")) document.domain = lower(fields.at("domain"));
    check(document.domain == "real", "Derzeit wird nur domain: real unterstuetzt");
    if (fields.count("seed")) {
        size_t parsed = 0;
        try {
            document.seed = std::stoull(fields.at("seed"), &parsed);
        } catch (...) {
            throw std::runtime_error("seed muss eine nichtnegative Ganzzahl sein");
        }
        check(parsed == fields.at("seed").size(), "seed enthaelt ungueltige Zeichen");
    }
    check(document.name.size() <= 200, "name ist laenger als 200 Zeichen");
    check(document.formula.size() <= 4096, "formula ist laenger als 4096 Zeichen");
    return document;
}

std::string analyzeFormulaDocument(const FormulaDocument& document, int& exit_code) {
    const size_t equality = document.formula.find('=');
    check(equality != std::string::npos, "formula muss genau eine Gleichung LHS = RHS enthalten");
    check(document.formula.find('=', equality + 1) == std::string::npos,
          "formula darf nur ein Gleichheitszeichen enthalten");
    const std::string lhs = trim(document.formula.substr(0, equality));
    const std::string rhs = trim(document.formula.substr(equality + 1));
    check(!lhs.empty() && !rhs.empty(), "Beide Seiten der Gleichung muessen belegt sein");

    NumericEvaluator evaluator;
    const NumericResult result = evaluator.testHypothesis(lhs, rhs, document.seed);

    std::ostringstream report;
    report << "MRE FORMELPRUEFUNG\n"
           << "============================================================\n"
           << "Name: " << document.name << "\n"
           << "Formel: " << document.formula << "\n"
           << "Definitionsbereich: reelle Zahlen (numerische Stichprobe)\n"
           << "Variablen: x, y\n"
           << "Seed: " << document.seed << "\n\n";

    if (result.verdict == NumericVerdict::Counterexample) {
        exit_code = 1;
        report << "ERGEBNIS: WIDERLEGT DURCH GEGENBEISPIEL\n"
               << "Gegenbeispiel: " << result.counterexample_point << "\n"
               << "Bis zum Fund gepruefte Punkte: " << result.points_tested << "\n";
    } else if (result.verdict == NumericVerdict::Survived) {
        exit_code = 0;
        report << "ERGEBNIS: NUMERISCHE TESTS UEBERSTANDEN\n"
               << "Gepruefte Punkte: " << result.points_tested << "\n"
               << "WICHTIG: Das ist kein formaler mathematischer Beweis.\n"
               << "Es wurde in einem festen Gitter und mit reproduzierbaren Zufallspunkten\n"
               << "nach Gegenbeispielen gesucht.\n";
    } else if (result.verdict == NumericVerdict::DomainError) {
        exit_code = 3;
        report << "ERGEBNIS: DEFINITIONSBEREICHSFEHLER\n";
    } else {
        exit_code = 2;
        report << "ERGEBNIS: FORMEL NICHT NUMERISCH AUSWERTBAR\n";
    }
    report << "Details: " << result.details << "\n\n"
           << "Unterstuetzte Syntax: x, y, Dezimalzahlen, +, -, *, /, ^ und Klammern.\n"
           << "Potenzen werden als a^b geschrieben; Multiplikationszeichen muessen explizit sein (2*x).\n";
    return report.str();
}

int runFormulaAnalysis(const fs::path& input_path, const fs::path& output_path) {
    const FormulaDocument document = loadFormulaDocument(input_path);
    int exit_code = 2;
    const std::string report = analyzeFormulaDocument(document, exit_code);
    std::cout << report;
    if (!output_path.empty()) {
        if (output_path.has_parent_path()) fs::create_directories(output_path.parent_path());
        std::ofstream output(output_path, std::ios::binary | std::ios::trunc);
        check(static_cast<bool>(output), "Ergebnisdatei kann nicht geschrieben werden");
        output << report;
        check(static_cast<bool>(output), "Ergebnisdatei konnte nicht vollstaendig geschrieben werden");
    }
    return exit_code;
}

} // namespace mre
