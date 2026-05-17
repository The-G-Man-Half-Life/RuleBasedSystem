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
    //For saving the variables
    std::unordered_map<std::string, int> variables;
    //For saving all the facts that are activated
    std::unordered_set<std::string> activeFacts;

    //Reads the file and extracts what is under state for the menu
    void loadFromString(const std::string& source);
    //Quick consult functions for the interpreter
    bool isFactActive(const std::string& fact) const;
    //Verifies that the program has variables to woork with
    bool hasVariable(const std::string& varName) const;
    //Obtain the variable to work with it
    int getVariable(const std::string& varName) const;
    // Actives a new fact. returns 'true' if is new, 'false' if it was already active
    bool activateFact(const std::string& fact);
};