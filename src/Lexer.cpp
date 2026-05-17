//Include of all the required files
#include "../include/Lexer.hpp"

//Include of other required tools
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source) {
    //Map for keywords
    keywords["rule"] = TokenType::RULE;
    keywords["if"] = TokenType::IF;
    keywords["then"] = TokenType::THEN;
    keywords["AND"] = TokenType::AND;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    //Reading until there are no more characters
    while (!isAtEnd()){
        start = current; //The beginning of the next token is our current position
        scanToken(tokens);
    }

    //Adding an END_OF_FILE token as an indicator of the end of the file
    tokens.push_back(Token(TokenType::END_OF_FILE, "", line));

    return tokens;
}

void Lexer::scanToken(std::vector<Token>& tokens){
    char character = advance();

    switch (character){
        //Single element chars
        case ':': addToken(tokens, TokenType::COLON); break;
        case '>': addToken(tokens, TokenType::GREATER); break;
        case '<': addToken(tokens, TokenType::LESS); break;
        case '=': addToken(tokens, TokenType::EQUAL); break;

        //Elements that will be evited to be read
        case ' ': // space
        case '\r': // carriage return
        case '\t': //horizontal tabulation
            break;

        //This line feed will be used to increase the line counter and keep reading tokens or identifying errors
        case '\n':
            line++; //Increase line counter
            break;

        default:
            /*Sequence to identify keywords, ids, or number.
            And in case is neither of those indicate an error*/ 
            if(isNumber(character) == true){
                processNumber(tokens);
            }
            //In case it begins with a letter it might indicate a keyword or id
            else if(isAlpha(character) == true){
                processIdentifier(tokens);
            }
            else{
                //In case of a strange element found in the grammar, indicate error
                std::cerr <<"Error in line " << line << ": Unexepected character '" << character << "'\n";
                //Indicating that the element just read is an error
                addToken(tokens, TokenType::ERROR);
            }
            break;
    }
}

// Useful methods
//Continue reading next character
char Lexer::advance(){
    return source[current++];
}

//Watching the source code 1 step ahead
char Lexer::peek() const{
    if(isAtEnd()){
        return '\0';
    }
    else{
        return  source[current];
    }
}

//Verifying if we are at the end
bool Lexer::isAtEnd() const{
    return current >= source.length();
}

bool Lexer::isNumber(char character) const{
     return character >= '0' && character <= '9';
}

bool Lexer::isAlpha(char character) const{
    /*As the document indicates all identifiers must be 
    written in lower case letter but as we must also identify
    and 'AND' we will read both lower case and capital case
    letters and later identify which one are we dealing with*/
    bool verifyLowerCaseLetter = character >= 'a' && character <= 'z';
    bool verifyCapitalCaseLetter = character >= 'A' && character <= 'Z';
    //Extra verification of this character as it is used on test cases
    bool verifyUnderScore = character == '_'; 

    return verifyLowerCaseLetter || verifyCapitalCaseLetter || verifyUnderScore;
}

bool Lexer:: isAlphaNumeric(char character) const {
    return isAlpha(character) || isNumber(character);
}

//Specific Processors
void Lexer::processNumber(std::vector<Token>& tokens){
    while (isNumber(peek())){
        advance();
    }

    addToken(tokens, TokenType::INTEGER);
}

void Lexer::processIdentifier(std::vector<Token>& tokens){
 // Read all the words
    while (isAlphaNumeric(peek())) {
        advance();
    }

    std::string text = source.substr(start, current - start);
    TokenType type;

    // Search for a keyword
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        type = it->second; 
    } else {
        // If is not a keyword then must be an id
        type = TokenType::ID; 
                
        // 1. Must begin with lower case
        if (text[0] < 'a' || text[0] > 'z') {
            std::cerr << "Lexical error " << line << ": The id '" << text << "' must begin with a lower case letter.\n";
            type = TokenType::ERROR;
        } 
        // 2. The rest of the text can't contain capital case letter
        else {
            for (char c : text) {
                if (c >= 'A' && c <= 'Z') {
                    std::cerr << "Lexical error in line " << line << ": The identifier '" << text << "' contains capital case letters.\n";
                    type = TokenType::ERROR;
                    break;
                }
            }
        }
    }

    addToken(tokens, type);}

//Add the identifier or error
void Lexer::addToken(std::vector<Token>& tokens, TokenType type){
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, line));
}