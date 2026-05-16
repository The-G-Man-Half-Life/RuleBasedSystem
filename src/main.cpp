#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/Environment.hpp"
#include "../include/Interpreter.hpp"
#include "../include/StaticAnalyzer.hpp"

namespace fs = std::filesystem;

// Helper to locate the data directory from common working directories
static fs::path findDataDir() {
    fs::path candidates[] = { fs::path("data"), fs::path("..") / "data", fs::path("..") / ".." / "data" };
    for (const auto& c : candidates) {
        if (fs::exists(c)) return c;
    }
    // Default to ../data when running from build/
    return fs::path("..") / "data";
}

// Statistical and Utility functions

void clearScreen() {
    #if defined(_WIN32)
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader(const std::string& title) {
    std::cout << "\n===================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "===================================================\n";
}

void printSeparator() {
    std::cout << "---------------------------------------------------\n";
}

void waitForEnter() {
    std::cout << "\n[Press enter to return to the main menu...]";
    std::string temp;
    std::getline(std::cin, temp);
}

// Scans the fold data and returns a list of all the txts
std::vector<std::string> getAvailableCases() {
    std::vector<std::string> cases;
    fs::path dataDir = findDataDir();
    if (!fs::exists(dataDir)) {
        fs::create_directories(dataDir); // Create folder if it does not exist
    }
    for (const auto& entry : fs::directory_iterator(dataDir)) {
        if (entry.path().extension() == ".txt") {
            cases.push_back(entry.path().filename().string());
        }
    }
    std::sort(cases.begin(), cases.end()); // alphabetical order
    return cases;
}

// Extract rules and facts from the unified file
bool parseUnifiedFile(const std::string& filepath, std::string& rulesOut, std::string& stateOut) {
    fs::path dataDir = findDataDir();
    std::ifstream file((dataDir / filepath).string());
    if (!file.is_open()) return false;

    std::string line;
    int currentMode = 0; // 0=Searching, 1=Reading rules, 2=Reading state

    while (std::getline(file, line)) {
        if (line.find("=== RULES ===") != std::string::npos) {
            currentMode = 1;
            continue;
        } else if (line.find("=== STATE ===") != std::string::npos) {
            currentMode = 2;
            continue;
        }

        if (currentMode == 1) rulesOut += line + "\n";
        else if (currentMode == 2) stateOut += line + "\n";
    }
    return true;
}

// Execution core

void executeEngine(const std::string& rules, const std::string& state) {
    Lexer lexer(rules);
    std::vector<Token> tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        if (token.type == TokenType::ERROR) {
            std::cerr << "[!] Lexical error. Stoping execution.\n";
            return;
        }
    }

    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();

    if (!ast) {
        std::cerr << "[!] Sintactical Error stoping execution.\n";
        return;
    }

    Environment env;
    env.loadFromString(state);

    StaticAnalyzer analyzer;
    std::vector<std::string> staticAnalysisMessages = analyzer.analyze(ast.get());

    Interpreter interpreter(ast.get(), env);
    interpreter.executeCycle(); 
    
    // Show results
    printSeparator();
    std::cout << "Execution output:\n";
    printSeparator();
    
    std::set<std::string> activatedFacts = interpreter.getActivatedFacts();
    if (activatedFacts.empty()) {
        std::cout << "(no output)\n";
    } else {
        for (const auto& fact : activatedFacts) {
            std::cout << fact << "\n";
        }
    }

    if (!staticAnalysisMessages.empty()) {
        std::cout << "\nStatic analysis:\n";
        for (const auto& msg : staticAnalysisMessages) {
            std::cout << msg << "\n";
        }
    }
    printSeparator();
}

// --- Menu management ---

void handleViewCases() {
    clearScreen();
    printHeader("Show possible test cases");
    
    auto cases = getAvailableCases();
    if (cases.empty()) {
        std::cout << "There are not cases in the folder 'data/'.\n";
        waitForEnter();
        return;
    }

    for (size_t i = 0; i < cases.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << cases[i] << "\n";
    }

    std::cout << "\nWrite down The number to see the case you wish\n(Or press enter to turn back): ";
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) return;

    try {
        int choice = std::stoi(input);
        if (choice > 0 && static_cast<std::size_t>(choice) <= cases.size()) {
            std::string rules, state;
            if (parseUnifiedFile(cases[choice-1], rules, state)) {
                clearScreen();
                printHeader("Watching case: " + cases[choice-1]);
                
                std::cout << "\n=== RULES SECTION  ===\n";
                std::cout << (rules.empty() ? "(Without rules)\n" : rules);
                
                std::cout << "\n=== INITIAL STATE SECTION ===\n";
                std::cout << (state.empty() ? "(Without initial state)\n" : state);
            }
        }
    } catch (...) {
        // If a number is not valid return back to the origin
    }
    waitForEnter();
}

void handleExecuteCase() {
    clearScreen();
    printHeader("Execute a case");
    
    auto cases = getAvailableCases();
    if (cases.empty()) {
        std::cout << "No cases to execute.\n";
        waitForEnter();
        return;
    }

    for (size_t i = 0; i < cases.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << cases[i] << "\n";
    }

    std::cout << "\nWrite down the number of the case to execute\n(Or simply write enter to turn back): ";
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) return;

        try {
            int choice = std::stoi(input);
            if (choice > 0 && static_cast<std::size_t>(choice) <= cases.size()) {
                std::string rules, state;
                if (parseUnifiedFile(cases[choice-1], rules, state)) {
                    clearScreen();
                    try {
            int choice = std::stoi(input);
            if (choice > 0 && static_cast<std::size_t>(choice) <= cases.size()) {
                std::string rules, state;
                if (parseUnifiedFile(cases[choice-1], rules, state)) {
                    clearScreen();
                    printHeader("Watching case: " + cases[choice-1]);
                    
                    std::cout << "\n=== RULES SECTION  ===\n";
                    std::cout << (rules.empty() ? "(Without rules)\n" : rules);
                    
                    std::cout << "\n=== INITIAL STATE SECTION ===\n";
                    std::cout << (state.empty() ? "(Without initial state)\n" : state);
                }
            }
        } catch (...) {
            // If a number is not valid return back to the origin
        }
            printHeader("Executing: " + cases[choice-1]);
            executeEngine(rules, state);
        } else {
            std::cout << "Error reading the file.\n";
        }
    }
    } catch (...) {}
    
    waitForEnter();
}

void handleAddNewCase() {
    clearScreen();
    printHeader("Create a new case");

    std::cout << "Write down the name for the new file (ex. caso7):\n> ";
    std::string filename;
    std::getline(std::cin, filename);
    
    if (filename.empty()) return;
    if (filename.find(".txt") == std::string::npos) filename += ".txt";

    std::cout << "\n=== ENTER THE RULES ===\n";
    std::cout << "Write down your rules down there.\n";
    std::cout << "When finished, write down 'END' in a new line and press ENTER.\n";
    printSeparator();
    
    std::string rules = "", line;
    while (std::getline(std::cin, line)) {
        if (line == "END" ) break;
        rules += line + "\n";
    }

    std::cout << "\n=== ENTER INITIAL STATE ===\n";
    std::cout << "Write down the variables (ex. temp = 30) and initial states\n";
    std::cout << "When finished, write down 'END' in a new line and press ENTER.\n";
    printSeparator();
    
    std::string state = "";
    while (std::getline(std::cin, line)) {
        if (line == "END") break;
        state += line + "\n";
    }

    // Saves everything in a unified file
    fs::path dataDir = findDataDir();
    if (!fs::exists(dataDir)) fs::create_directories(dataDir);
    std::ofstream file((dataDir / filename).string());
    
    file << "=== RULES ===\n";
    file << rules;
    file << "=== STATE ===\n";
    file << state;
    file.close();

    // -------------------------------------------------------------
    // NUEVO: Mostrar resumen de lo que se acaba de guardar
    // -------------------------------------------------------------
    clearScreen();
    printHeader("Case Summary: " + filename);
    
    std::cout << "\n=== RULES SECTION ===\n";
    std::cout << (rules.empty() ? "(Without rules)\n" : rules);
    
    std::cout << "\n=== INITIAL STATE SECTION ===\n";
    std::cout << (state.empty() ? "(Without initial state)\n" : state);
    
    printSeparator();
    std::cout << "\n[!] Case saved succesfully as '" << filename << "'.\n";
    std::cout << "To execute, go to option 2 in the main menu.\n";
    // -------------------------------------------------------------

    waitForEnter();
}

// --- main function ---

int main() {
    while (true) {
        clearScreen();
        printHeader("Rules based system");
        std::cout << "  1. See possible cases\n";
        std::cout << "  2. Execute a case\n";
        std::cout << "  3. Add a new case\n";
        std::cout << "  4. End program\n";
        printSeparator();
        std::cout << "Choose an option (1-4): ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "1") {
            handleViewCases();
        } else if (input == "2") {
            handleExecuteCase();
        } else if (input == "3") {
            handleAddNewCase();
        } else if (input == "4") {
            clearScreen();
            std::cout << "End of the program\n";
            break;
        }
    }
    return 0;
}