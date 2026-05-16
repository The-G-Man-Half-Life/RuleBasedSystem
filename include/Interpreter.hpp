//Pragma once for eviting over readding the same file
#pragma once

//Include of all the required programs
#include <AST.hpp>
#include <Environment.hpp>

//Include of all the required tools
#include <set>
#include <string>
#include <iostream>

class Interpreter{
public:
    //Receives the tree (program) and the states (start variables and states)
    Interpreter(const ProgramNode* program, Environment& env);

    //Execute the fixed point cycle
    void executeCycle();

    //Returns the facts that were activated during the execution (except start ones)
    //Usage of std::set because it automatically sorts everything alphabetically
    std::set<std::string> getActivatedFacts() const;

private:
    const ProgramNode* program;
    Environment& env;

    //At here all the outputs from the program will be saved
    std::set<std::string> newlyActivatedFacts;

    //Recursive function to evaluate if a function is true or false
    bool evaluateCondition(const ConditionNode* cond) const; 
};