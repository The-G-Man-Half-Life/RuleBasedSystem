//Pragma for eviting reading the file more than once
#pragma once

//Include of required files
#include "Token.hpp"

//Include of other required tools
#include <string>
#include <vector>
#include <memory>


//Base class
/*All nodes will inherit this class and will
use the destroyer for a clean management of memory*/
struct ASTNode {
    virtual ~ASTNode() = default;
};

//Condition class for polimorphism and all nodes related to conditions
struct ConditionNode : public ASTNode {
    virtual ~ConditionNode() = default;
};

//Atomical condition: Fact
struct FactConditionNode : public ConditionNode {
    //Atributes
    std::string factId;

    //Usage of std::move for more efficiency to transfer the string
    //Constructor
    explicit FactConditionNode(std::string id) : factId(std::move(id)) {}
};

//Atomical condition: Comparison
struct ComparisonNode : public ConditionNode {
    //Atributes
    std::string id;
    TokenType operation; //Either less than or greater than
    int value;
    
    //Constructor
    ComparisonNode(std::string id, TokenType operation, int value)
        : id(std::move(id)), operation(operation), value(value) {}
};

// Composed condition: AND 
struct AndConditionNode : public ConditionNode {
    //This composed condition is made out of 2 singular conditions
    //We will use a unique pointer as the memory size is unknown
    
    //Atributes
    std::unique_ptr<ConditionNode> leftCondition;
    std::unique_ptr<ConditionNode> rightCondition;

    //constructor
    AndConditionNode(std::unique_ptr<ConditionNode> leftCondition, std::unique_ptr<ConditionNode> rightCondition)
        : leftCondition(std::move(leftCondition)), rightCondition(std::move(rightCondition)) {} 
};

//Action class
// It contains a fact that must be activated
struct ActionNode : public ASTNode {
    //Atribute
    std::string factId;

    //constructor
    explicit ActionNode(std::string id) : factId(std::move(id)) {}
};

//Rule class
struct RuleNode : public ASTNode{
    //Atributes
    std::string ruleId; //rule <ID> : if <cond> then <action>
    std::unique_ptr<ConditionNode> condition; ////rule <id> : if <COND> then <action>
    std::unique_ptr<ActionNode> action; //rule <id> : if <cond> then <ACTION>

    //Constructor
    RuleNode(std::string id, std::unique_ptr<ConditionNode> cond, std::unique_ptr<ActionNode> act)
        : ruleId(std::move(id)), condition(std::move(cond)), action(std::move(act)) {}
};

//Class Root program
//Is a list of rules
struct ProgramNode : public ASTNode {
    //Atributes
    std::vector<std::unique_ptr<RuleNode>> rules;
};