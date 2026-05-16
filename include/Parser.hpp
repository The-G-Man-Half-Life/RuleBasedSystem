//Pragma for eviting over reading the file
#pragma once

//Include of the required programs
#include "Token.hpp"
#include "AST.hpp"

//Include of other required tools
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    //Constructor for receiving Lexer tokens
    explicit Parser(const std::vector<Token>& tokens);

    //Entry point
    std::unique_ptr<ProgramNode> parse();

private:
    std::vector<Token> tokens;
    int current = 0; // Current token

    //Recursive descending functions
    std::unique_ptr<RuleNode> parseRule();
    std::unique_ptr<ConditionNode> parseCondition();
    std::unique_ptr<ConditionNode> parseAtom();
    std::unique_ptr<ActionNode> parseAction();

    //Utility functions
    Token advance();//Consumes the actual token and advances
    Token peek() const;//Watches the current token order without affecting it
    Token previous() const;//Obtains the last consumed token
    bool isAtEnd() const;//Makes sure we have finished reading
    bool match(TokenType type); //if the current token is of TokenType consumes it and returns true,
    
    // This functions reads the consumed tokens in case it does not belong there it wil kill the process
    Token consume(TokenType type, const std::string& errorMessage);
};
