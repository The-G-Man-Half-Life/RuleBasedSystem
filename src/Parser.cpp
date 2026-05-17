//Include of all the required programs
#include  "../include/Parser.hpp"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

//Program -> RuleList
std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();

    try {
        //While we are not at the end of the file
        while (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
            //RuleList -> Rule RuleList | ε
            program->rules.push_back(parseRule());
        }
    } 
    catch (const std::runtime_error& error) {
        //In case there is an error we catch it here
        std::cerr << "Sintactical error: " << error.what() << std::endl;
        return nullptr; //Return an empty AST in case of failure
    }

    return program;
}

//Rule -> rule id : if condition then action
std::unique_ptr<RuleNode> Parser::parseRule(){
    //Error situation in case 'rule' is not mentioned
    consume(TokenType::RULE, "The reserved keyword 'rule' was expected");

    //Error situation where the rule name is not proper
    Token idToken = consume(TokenType::ID, "An identifier was expected for the rule");
    std::string ruleId = idToken.lexeme;

    //Error where the colon was expected but not received
    consume(TokenType::COLON, "The reserved keyword ':' was expected after the rule name");

    //Error where the 'if' was expected but not received'
    consume(TokenType::IF, "The reserved word 'if' was not received");

    //parsing the condition and jumping to  the next function
    auto condition = parseCondition();

    //Error where the 'then' was expected but not received'
    consume(TokenType::THEN, "The reserved word 'then' was expected but not received after the condition");

    //Parsing the action
    auto action = parseAction();

    //In case everything goes fine we wrap up and return the node
    return std::make_unique<RuleNode>(ruleId, std::move(condition), std::move(action));
}

//Transformation LL(1): Cond -> Atom (AND Cond)
std::unique_ptr<ConditionNode> Parser::parseCondition() {
    //First we must take 1 condition (simple condition)
    auto leftNode = parseAtom();

    //If the next token is 'AND' we consume it and parse the right condition
    if (match(TokenType::AND)) {
        //Recursion by the right
        auto rightNode = parseCondition(); 
        
        //Join the left and right condition with an AND
        return std::make_unique<AndConditionNode>(std::move(leftNode), std::move(rightNode));
    }

    //In case there wasn't an AND we return only 1 condition
    return leftNode;
}

//Atom -> id RelOp value | id
std::unique_ptr<ConditionNode> Parser::parseAtom() {
    //Error when making a comparison there isn't an identifier
    Token idToken = consume(TokenType::ID, "An id was expected in the condition.");
    
    //Now we watch if there is a boolen operation in the condition
    if (match(TokenType::GREATER) || match(TokenType::LESS) || match(TokenType::EQUAL)) {
        Token opToken = previous(); //save which identifier was
        
        //The expected value for comparisons is an integer in case there is not we return an error
        Token valueToken = consume(TokenType::INTEGER, "An integer value was expected after the operation.");
        int value = std::stoi(valueToken.lexeme); //Convert string to number

        return std::make_unique<ComparisonNode>(idToken.lexeme, opToken.type, value);
    }

    //In case there was not an id, it wasnt a boolean operation but an id (a fact)
    return std::make_unique<FactConditionNode>(idToken.lexeme);
}

//Action -> id
std::unique_ptr<ActionNode> Parser::parseAction() {
    //Indicating the action to be executed if the requirement is fulfilled and in case there is not show error
    Token idToken = consume(TokenType::ID, "An identifier was expected for the action.");
    return std::make_unique<ActionNode>(idToken.lexeme);
}

//Implementation of functional utilities

//Advance in case not at the end
Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

//Watch current token
Token Parser::peek() const {
    return tokens[current];
}

//Watch previous token
Token Parser::previous() const {
    return tokens[current - 1];
}

/*Watch if we are at the end of the file by using the total amount of tokens
as indication or by using the specialized END_OF_FILE TokenType*/
bool Parser::isAtEnd() const {
    return current >= tokens.size() || tokens[current].type == TokenType::END_OF_FILE;
}

bool Parser::match(TokenType type) {
    if (isAtEnd()){
        return false;
    }
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& errorMessage) {
    if (!isAtEnd() && peek().type == type) {
        return advance();
    }
    //In case the received token is not what expected return an error message indicating line and reason
    std::string fullError = errorMessage + " (Line " + std::to_string(peek().line) + ")";
    throw std::runtime_error(fullError);
}