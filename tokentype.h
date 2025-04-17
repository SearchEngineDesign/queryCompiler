#pragma once
#include "../utils/searchstring.h"

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

class QueryToken {
    public:
    virtual TokenType GetType() const {
        return T_INVALID;
    }
    virtual string GetValue() const
        {
        return "";
        }
    virtual ~QueryToken() = default;
    QueryToken() = default;
};

class QueryTokenWord : public QueryToken {
    private:
    string word;
    public:
    QueryTokenWord(string val) : word(val) {}
    TokenType GetType() const override {
        return TokenType::T_WORD;
    }   
    string GetValue() const override {
        return word;
    }
};

class TokenEOF : public QueryToken {
    public:
    TokenType GetType() const override {
        return T_EOF;
    }
};

class TokenOr : public QueryToken {
    public:
    TokenType GetType() const override {
        return T_OR;
    }
};

class TokenNot : public QueryToken {
    public:
    TokenType GetType() const override {
        return T_NOT;
    }
};

class TokenOpenParen : public QueryToken {
    public:
    TokenType GetType() const override {
        return T_OPEN_PAREN;
    }
};

class TokenCloseParen : public QueryToken {
    public:
    TokenType GetType() const override {
        return T_CLOSE_PAREN;
    }
};

class TokenQuote : public QueryToken {
    public:
    TokenType GetType() const override {
        return T_QUOTE;
    }
};