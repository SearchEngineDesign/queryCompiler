#pragma once
#include "utils/string.h"

enum TokenType {
    T_INVALID = -1,
    T_EOF = 0,
    T_OR,
    T_NOT,
    T_OPEN_PAREN,
    T_CLOSE_PAREN,
    T_QUOTE,
    T_WORD,
};

class Token {
    public:
    virtual TokenType GetType() const {
        return T_INVALID;
    }
    virtual string GetValue() const
        {
        return "";
        }
    virtual ~Token() = default;
    Token() = default;
};

class TokenWord : public Token {
    private:
    string word;
    public:
    TokenWord(string val) : word(val) {}
    TokenType GetType() const override {
        return TokenType::T_WORD;
    }   
    string GetValue() const override {
        return word;
    }
};

class TokenEOF : public Token {
    public:
    TokenType GetType() const override {
        return T_EOF;
    }
};

class TokenOr : public Token {
    public:
    TokenType GetType() const override {
        return T_OR;
    }
};

class TokenNot : public Token {
    public:
    TokenType GetType() const override {
        return T_NOT;
    }
};

class TokenOpenParen : public Token {
    public:
    TokenType GetType() const override {
        return T_OPEN_PAREN;
    }
};

class TokenCloseParen : public Token {
    public:
    TokenType GetType() const override {
        return T_CLOSE_PAREN;
    }
};

class TokenQuote : public Token {
    public:
    TokenType GetType() const override {
        return T_QUOTE;
    }
};