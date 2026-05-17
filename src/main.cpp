//Include of all the required programs
#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/Environment.hpp"
#include "../include/Interpreter.hpp"
#include "../include/StaticAnalyzer.hpp"

//Include of all the required tools
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>


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
//To clean the screen when choosing options
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
    //List for keeping all of them
    std::vector<std::string> cases;
    //Direction where all the data are at
    fs::path dataDir = findDataDir();
    if (!fs::exists(dataDir)) {
        fs::create_directories(dataDir); // Create folder if it does not exist
    }
    for (const auto& entry : fs::directory_iterator(dataDir)) {
        //Will save all of the txt files as these are the cases
        if (entry.path().extension() == ".txt") {
            cases.push_back(entry.path().filename().string());
        }
    }
    std::sort(cases.begin(), cases.end()); // alphabetical order
    return cases;
}

// Extract rules and facts from the unified file
bool parseUnifiedFile(const std::string& filepath, std::string& rulesOut, std::string& stateOut) {
    //Direction of the data
    fs::path dataDir = findDataDir();
    //Turn to string what is found in that file
    std::ifstream file((dataDir / filepath).string());
    //In case file is not open indicates that it cant be accessed
    if (!file.is_open()) return false;

    //Iterator for lines
    std::string line;
    int currentMode = 0; // 0=Searching, 1=Reading rules, 2=Reading state

    while (std::getline(file, line)) {
        //To separate parts of the program between rules and states and using the indicator
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
    //The lexer with all of the gramar rules
    Lexer lexer(rules);
    //Turn all of the source code into tokens
    std::vector<Token> tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        //In case the program has a token of type error it will indicate it
        if (token.type == TokenType::ERROR) {
            std::cerr << "[!] Lexical error. Stoping execution.\n";
            return;
        }
    }
    //For parsing all the tokens with each other to actually make sense
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();

    if (!ast) {
        //In case the order of tokens does not make sense return this error
        std::cerr << "[!] Sintactical Error stoping execution.\n";
        return;
    }

    //Load all content from the environment
    Environment env;
    env.loadFromString(state);

    //The AST tree to make all the analyzation of the code to find any errors and execute code
    StaticAnalyzer analyzer;
    std::vector<std::string> staticAnalysisMessages = analyzer.analyze(ast.get(), env);

    //Takes care of understanding the code and execute it
    Interpreter interpreter(ast.get(), env);
    //Execute cycle of interpretation of the data
    interpreter.executeCycle(); 
    
    // Show results
    printSeparator();
    std::cout << "Execution output:\n";
    printSeparator();
    //Show the activated facts
    std::set<std::string> activatedFacts = interpreter.getActivatedFacts();
    //In case there were not any facts activated return no output
    if (activatedFacts.empty()) {
        std::cout << "(no output)\n";
    } else { //else indicate the activated facts
        for (const auto& fact : activatedFacts) {
            std::cout << fact << "\n";
        }
    }
    //These are all the error messages or failed logical message ones
    if (!staticAnalysisMessages.empty()) {
        std::cout << "\nStatic analysis:\n";
        for (const auto& msg : staticAnalysisMessages) {
            std::cout << msg << "\n";
        }
    }
    printSeparator();
}

// --- Menu management ---

//This function will take care of showing all test cases in the data folder 
/*In case the user wishes to see one specific. It must choose according to the number
and the program will show it*/
/*In case the user doesn't want to watch any case just by clicking enter will return it
back to the beginning menu*/
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
    //This is for showing everything inside the choosen case
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

/*This function will be the one executing the cases but first doing the verification
that there are cases to try out or that the chosen case actually has something to try out*/
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

    //Recceiving the option the user wants to see
    std::cout << "\nWrite down the number of the case to execute\n(Or simply write enter to turn back): ";
    std::string input;
    std::getline(std::cin, input);

    //In case of empty input just return to menu
    if (input.empty()) return;

        //Displaying everything inside the case 
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
        //Watching the actual code execution
            printHeader("Executing: " + cases[choice-1]);
            executeEngine(rules, state);
        } else {
            std::cout << "Error reading the file.\n";
        }
    }
    } catch (...) {}
    
    waitForEnter();
}


//This function will help by allowing the user to enter a 
void handleAddNewCase() {
    clearScreen();
    printHeader("Create a new case");

    std::cout << "Write down the name for the new file (ex. caso7):\n> ";
    std::string filename;
    std::getline(std::cin, filename);
    
    if (filename.empty()) return;
    if (filename.find(".txt") == std::string::npos) filename += ".txt";

    //Waiting for user to enter the wished rule
    std::cout << "\n=== ENTER THE RULES ===\n";
    //Example of how to enter it
    std::cout << "Write down your rules down there like:\n";
    std::cout << "rule r1:\n";
    std::cout << "if temp > 30 AND humidity < 50 then alert\n";
    std::cout << "When finished, write down 'END' in a new line and press ENTER.\n";
    printSeparator();
    
    std::string rules = "", line;
    while (std::getline(std::cin, line)) {
        //Break line for receiving condition
        if (line == "END" ) break;
        rules += line + "\n";
    }

    //Receiving initial state works the sane as receiving the rules
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
    //Separators for future management
    file << "=== RULES ===\n";
    file << rules;
    file << "=== STATE ===\n";
    file << state;
    file.close();

    // Show a resume of what was saved so the user is conscious about it
    clearScreen();
    printHeader("Case Summary: " + filename);
    
    std::cout << "\n=== RULES SECTION ===\n";
    std::cout << (rules.empty() ? "(Without rules)\n" : rules);
    
    std::cout << "\n=== INITIAL STATE SECTION ===\n";
    std::cout << (state.empty() ? "(Without initial state)\n" : state);
    
    printSeparator();
    std::cout << "\n[!] Case saved succesfully as '" << filename << "'.\n";
    std::cout << "To execute, go to option 2 in the main menu.\n";

    waitForEnter();
}

//Where everything is executed
int main() {
    //Logic of the menu
    while (true) {
        //Clearing screen every cycle for a more fitting view
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

        //Show the cases
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