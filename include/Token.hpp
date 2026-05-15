//Pragma for eviting over reading the same file
#pragma once

//Include of required elements
#include <string>

//All possible tokens that this language could have
enum class TokenType {
    //Keywords (Basic elements for boolean logic)
    RULE,   //Establishes a set of of requirements and depending on its completition it will return an output or not
    IF,     //Verifier of conditions
    THEN,   //In case an if is fulfilled it will execute what proceeds
    AND,    //Concatenates statements
    
    //Boolean operations (Operate statements of true and false)
    GREATER,//>
    LESS,   //<

    //Declarators (Help declare variables and rules)
    COLON, //":" (Used for declaring rules)
    EQUAL, //"=" (Used for declaring variables aka identifiers)

    //Data types
    ID,    //Identifier of a variable
    INTEGER, //Used to denote integers

    //Control tokens
    END_OF_FILE, //To identify the end of a file
    ERROR //To indicate incorrect values
};

//Structure that represents a token
struct Token{
    TokenType type;      //Token type
    std::string lexeme;  //Text formed by that token
    int line;            //Will be used to identify wher errors are at

    //Constructor
    Token(TokenType type, std::string lexeme, int line):
        type(type), lexeme(std::move(lexeme)), line(line) {}
};
