#pragma once

#include "../utils/string.h"
#include "../utils/vector.h"
#include "tokentype.h"

class TokenStream {
    public:
    TokenStream(const char* input) : input(input) {}
    TokenStream(string& input) : input(input.c_str()) {}
    //take next token -- removes it from the stream
    Token* TakeToken( );
    //returns current token
    Token* CurrentToken( ) {
        return currentToken;
    }
    //returns current token string
    string CurrentTokenString( ) {
        return currentTokenString;
    }
    //matches token, takes if matched
    bool match (TokenType type) {
        if (ReadTokenType() == type)
            {
            currentToken = TakeToken();
            currentTokenString = currentToken->GetValue();
            return true;
            }
        return false;
    }
    //matches token
    private:
    //requires input to be a c-string, null terminated
    const char* input;
    //not the token at input, but the token that was taken last
    Token* currentToken;
    //undefined behavior if currentToken is not a word
    string currentTokenString;
    TokenType ReadTokenType();
};

/*
 //initialize from string
    TokenStream(const string& content, bool string) {
        if (string)
            {
            std::ofstream f("temp.txt");
            f << content;
            f.close();
            input.open("temp.txt");
            }
        else
            input.open(content.c_str());
    }
    //initialize from file
    TokenStream(const string& filename) {
        input.open(filename.c_str());
    }
    //initialize from ifstream
    TokenStream ( std::ifstream &stream ) {
        input = std::move(stream);
    }*/


   /*
   class Token {
    public:
    TokenType getType() const {
        return Type;
    }
    string getValue() const {
        return Value;
    }
    //default constructor
    Token() : Value(""), Type(TokenInvalid) {}
    //constructor
    Token(const string& v, TokenType t) :  Value(v), Type(t) {}
    private:
    TokenType Type;
    string Value;
};*/

/*
class TokenTable {
    public:
    TokenTable() {
        AddToken("OR", TokenOr);
        AddToken("|", TokenOr);
        AddToken("||", TokenOr);
        AddToken("NOT", TokenNot);
        AddToken("!", TokenNot);
        AddToken("(", TokenOpenParen);
        AddToken(")", TokenCloseParen);
        AddToken("\"", TokenQuote);
        AddToken("\'", TokenQuote);
    }
    Token FindToken(const string& s) 
        {
        for (Token t : tokens)
            if (t.getValue() == s)
                return t;
        //not enumerated token, initialize as word.
        return Token(s, TokenWord);
        }
    private:
    void AddToken(const string& token, TokenType type) 
        {
        tokens.push_back( Token( token, type ) ); 
        }
    vector<Token> tokens;
};*/