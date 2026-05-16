//Pragma for evite ove reading the same file
#pragma once

//include of the required files
#include "AST.hpp"

//Include of all the required tools
#include <vector>
#include <unordered_map>
#include <set>
#include <string>

class StaticAnalyzer{
public:
    //Reads the AST and returns a vector with the message in the exact format required
    std::vector<std::string> analyze(const ProgramNode* program);

private:
    //Converts a condition of the AST into a string. Facilitating the comparisons
    std::string stringifyCondition(const ConditionNode* cond);

};


