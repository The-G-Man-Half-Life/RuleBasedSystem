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
    void loadFromFile(const std::string& filename);
    void loadFromString(const std::string& source);
    //Quick consult functions for the interpreter
    bool isFactActive(const std::string& fact) const;
    bool hasVariable(const std::string& varName) const;
    int getVariable(const std::string& varName) const;
    // Actives a new fact. returns 'true' if is new, 'false' if it was already active
    bool activateFact(const std::string& fact);
};