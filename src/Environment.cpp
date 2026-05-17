// Include of all the required programs
#include "../include/Environment.hpp"

//Include of all the required tools
#include <string>
#include <fstream>
#include <sstream>


void Environment::loadFromString(const std::string& source) {
    std::istringstream stream(source);
    std::string line;
    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue;

        std::string itemName;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            itemName = line.substr(0, pos);
            std::string valStr = line.substr(pos + 1);
            
            // blank space cleaning
            itemName.erase(itemName.find_last_not_of(" \t") + 1);
            valStr.erase(0, valStr.find_first_not_of(" \t"));
            
            // "Integers are base-10 and may be positive or zero."
            bool isValidNumber = true;
            for (char c : valStr) {
                if (c < '0' || c > '9') {
                    isValidNumber = false;
                    break;
                }
            }

            if (!isValidNumber) {
                std::cerr << "[!] Error in the state the variables must be values from 0 to infinite: '" 
                            << itemName << " = " << valStr << "'\n";
                continue; // Ignore this variable and continue
            }

            variables[itemName] = std::stoi(valStr);
        } else {
            itemName = line;
            activeFacts.insert(itemName);
        }

        // Validación del nombre (que empiece con minúscula)
        if (!itemName.empty() && (itemName[0] < 'a' || itemName[0] > 'z')) {
            std::cerr << "[Advertencia]: El estado inicial contiene un identificador invalido ('" 
                        << itemName << "'). Deberia empezar con minuscula.\n";
        }
    }
}

//Verificates if a fact is active to see it should be added to the list
bool Environment::isFactActive(const std::string& fact) const {
    return activeFacts.find(fact) != activeFacts.end();
}
//Verify if there are variables for error and output management
bool Environment::hasVariable(const std::string& varName) const {
    return variables.find(varName) != variables.end();
}
//Obtain the variables
int Environment::getVariable(const std::string& varName) const {
    return variables.at(varName);
}
//Activate a facct 
bool Environment::activateFact(const std::string& fact) {
    auto result = activeFacts.insert(fact);
    return result.second;
}