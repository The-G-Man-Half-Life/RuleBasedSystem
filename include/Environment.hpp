//Pragma for not over reading the file
#pragma once

//Include of all the required tools
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>

class Environment{
public:
    std::unordered_map<std::string, int> variables;
    std::unordered_set<std::string> activeFacts;

    //Reads the initial file state and loads it in the memory
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if(file.is_open() == false){
            return;
        }

        std::string line;
        while(std::getline(file, line)){
            //Clean the empty spaces at the end of beginning
            line.erase(0, line.find_first_not_of("\t\r\n"));
            line.erase(line.find_first_not_of("\t\r\n") + 1);

            if(line.empty() == true) {
                continue;
            }
        
            //If the line contains a '=' is a variable
            size_t pos = line.find('=');
            if (pos != std::string::npos){
                std::string varName = line.substr(0, pos);
                std::string valStr = line.substr(pos+1);

                //Clean spaces
                varName.erase(varName.find_first_not_of(" \t") + 1);
                valStr.erase(0, valStr.find_first_not_of(" \t"));

                variables[varName] = std::stoi(valStr);
            }
            else{
                //If it does not have a '=', es is an initial active fact
                activeFacts.insert(line);
            }
        }
    }

    //Quick consult functions for the interpreter
    bool isFactActive(const std::string& fact) const {
        return activeFacts.find(fact) != activeFacts.end();
    }

    bool hasVariable(const std::string& varName) const {
        return variables.find(varName) != variables.end();
    }

    int getVariable(const std::string& varName) const {
        return variables.at(varName);
    }

    // Actives a new fact. returns 'true' if is new, 'false' if it was already active
    bool activateFact(const std::string& fact) {
        auto result = activeFacts.insert(fact);
        return result.second; // result.second is true if it wass inserted, false if it already existed
    }
};