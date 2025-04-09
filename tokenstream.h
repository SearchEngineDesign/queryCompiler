#pragma once

#include "utils/string.h"
#include "utils/vector.h"
#include <fstream>

enum TokenType {
    TokenInvalid = -1,
    TokenEOF = 0,
    TokenAnd,
    TokenOr,
    TokenNot,
    TokenOpenParen,
    TokenCloseParen,
    TokenQuote,
    TokenWord,
};

class Token {
    public:
    TokenType getType() const {
        return Type;
    }
    string getValue() const {
        return Value;
    }
    Token() : Value(""), Type(TokenInvalid) {}
    Token(const string& v, TokenType t) :  Value(v), Type(t) {}
    private:
    TokenType Type;
    string Value;
};

class TokenTable {
    public:
    TokenTable() {
        AddToken("AND", TokenAnd);
        AddToken("&", TokenAnd);
        AddToken("&&", TokenAnd);
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
    Token FindToken(const string& s) {
        for (Token t : tokens)
            if (t.getValue() == s)
                return t;
        //not enumerated token, initialize as word.
        return Token(s, TokenWord);
    }
    private:
    void AddToken(const string& token, TokenType type) {
        tokens.push_back(Token(token, type)); 
    }
    vector<Token> tokens;
};

class TokenStream {
    public:
    //default constructor to stop the compiler from complaining, don't use this
    TokenStream() = default;
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
    }
    //get current token
    Token currentToken( ) {
        return currentToken_;
    }
    //take next token -- removes it from the stream
    Token TakeToken( ) {
        if ( !( input >> currentTokenString_ ) )
            return Token( "/0", TokenEOF );
        currentToken_ = Token( table.FindToken( currentTokenString_ ) );
        return currentToken_;
    }
    private:
    std::ifstream input;
    string currentTokenString_;
    Token currentToken_;
    TokenTable table;
};