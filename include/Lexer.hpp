//Pragma for eviting over reading the same file
#pragma once

//Include of external files
#include "Token.hpp"

//Include of other required elements
#include <string>
#include <vector>
#include <unordered_map>

class Lexer{
public:
    //Constructor to receive source code
    explicit Lexer(const std::string& source); // Used explicit to evite automatic conversion to strings

    //Tokenize takes care of taking the source code and turn it into a list of tokens
    std::vector<Token> Tokenize();

private:
    std::string source;
    int start = 0;      //Start of the lexeme being identified right now
    int current = 0;    //Current character being evaluated
    int line = 1;       //Current line being reviewed in the file

    //Dictionary to look fast if a word is a key word
    std::unordered_map<std::string, TokenType> keywords; 
    //Using unorderedMap to match quickly the apparitions with its token type

    //Auxiliar methods to process the chars
    void scanToken(std::vector<Token>& tokens); 
    char advance(); //Reads the current character returns it and advances
    char peek() const;  //Reads the next character without consuming it
    bool isAtEnd() const; //Verifies if we are at the end of the code

    //Validation functions
    bool isNumber(char c) const; //Verifies if is number
    bool isAlpha(char c) const; //verifies if is a letter
    bool isAlphaNumeric(char c) const; //Verifies if is a combination of numbers and letters
    
    //Specific token processors
    void processNumber(std::vector<Token>& tokens); 
    void processIdentifier(std::vector<Token>& tokens); 
    void addToken(std::vector<Token>& tokens, TokenType type);
};


