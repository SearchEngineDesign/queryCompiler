#include "tokenstream.h"
#include "tokentype.h"
#include <iostream>

static inline bool reserved(char c)
    {
    return c == '|' || c == '!' || c == '(' || c == ')' || c == '"' || isspace(c);
    }

QueryToken* TokenStream::TakeToken( ) 
    {
    TokenType type = ReadTokenType();
    char* test = input;
    std::cout << "Test: " << test << std::endl;
   switch (type)
    {
    case T_OR:
        if (input[1] && input[0] == 'O' && input[1] == 'R')
            input += 2;
        else if (input[1] && input[0] == '|' && input[1] == '|')
            input += 2;
        else
            input++;
        return new TokenOr();
    case T_NOT:
        if (input[1] && input[2] && input[0] == 'N' && input[1] == 'O' && input[2] == 'T')
            input += 3;
        else
            input++;
        return new TokenNot();
    case T_OPEN_PAREN:
        input++;
        return new TokenOpenParen();
    case T_CLOSE_PAREN:
        input++;
        return new TokenCloseParen();
    case T_QUOTE:
        input++;
        return new TokenQuote();
    case T_WORD:
        string t;
        while ( *input && !reserved(*input) )
            {
            t.push_back(*input);
            input++;
            }
        QueryToken* token = new QueryTokenWord(t);
        currentToken = token;
        currentTokenString = t;
        std::cout << "Found token: " << t << std::endl;
        return token;
    }
    return nullptr;
    }

//read what the token type is at input
TokenType TokenStream::ReadTokenType()
    {
    //get rid of leading whitespace (leading whitespace is never useful)
    while ( *input && isspace( *input ) )
        input++;
    if (!input || !*input)
        return T_EOF;
    //checks for OR and NOT
    if (input[1] && input[0] == 'O' && input[1] == 'R')
        return T_OR;
    if (input[1] && input[2] && input[0] == 'N' && input[1] == 'O' && input[2] == 'T')
        return T_NOT;
    //checks for single character tokens
    char c = *input;
    if (c == '|')
        return T_OR;
    if (c == '!')
        return T_NOT;
    if (c == '(')
        return T_OPEN_PAREN;
    if (c == ')')
        return T_CLOSE_PAREN;
    if (c == '"')
        return T_QUOTE;
    return T_WORD;
    }