// Implementation of Environment member functions
#include "../include/Environment.hpp"

#include <string>
#include <fstream>
#include <sstream>

void Environment::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Trim leading/trailing whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (line.empty()) continue;
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string varName = line.substr(0, pos);
            std::string valStr = line.substr(pos + 1);
            varName.erase(varName.find_last_not_of(" \t") + 1);
            valStr.erase(0, valStr.find_first_not_of(" \t"));
            variables[varName] = std::stoi(valStr);
        } else {
            activeFacts.insert(line);
        }
    }
}

void Environment::loadFromString(const std::string& source) {
    std::istringstream stream(source);
    std::string line;
    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (line.empty()) continue;
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string varName = line.substr(0, pos);
            std::string valStr = line.substr(pos + 1);
            varName.erase(varName.find_last_not_of(" \t") + 1);
            valStr.erase(0, valStr.find_first_not_of(" \t"));
            variables[varName] = std::stoi(valStr);
        } else {
            activeFacts.insert(line);
        }
    }
}

bool Environment::isFactActive(const std::string& fact) const {
    return activeFacts.find(fact) != activeFacts.end();
}

bool Environment::hasVariable(const std::string& varName) const {
    return variables.find(varName) != variables.end();
}

int Environment::getVariable(const std::string& varName) const {
    return variables.at(varName);
}

bool Environment::activateFact(const std::string& fact) {
    auto result = activeFacts.insert(fact);
    return result.second;
}