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
   switch (type)
        {
        case T_EOF:
            currentToken = new TokenEOF();
            currentTokenString = "\0";
            return currentToken;
        case T_OR:
            if (input[1] && input[0] == 'O' && input[1] == 'R')
                input += 2;
            else if (input[1] && input[0] == '|' && input[1] == '|')
                input += 2;
            else
                input++;
            currentToken = new TokenOr();
            currentTokenString = "\0";
            return currentToken;
        case T_AND:
            if (input[1] && input[2] && input[0] == 'A' && input[1] == 'N' && input[2] == 'D')
                input += 3;
            else if (input[1] && input[0] == '&' && input[1] == '&')
                input += 2;
            else
                input++;
            currentToken = new TokenAnd();
            currentTokenString = "\0";
            return currentToken;
        case T_NOT:
            if (input[1] && input[2] && input[0] == 'N' && input[1] == 'O' && input[2] == 'T')
                input += 3;
            else
                input++;
            currentToken = new TokenNot();
            currentTokenString = "\0";
            return currentToken;
        case T_OPEN_PAREN:
            input++;
            currentToken = new TokenOpenParen();
            currentTokenString = "\0";
            return currentToken;
        case T_CLOSE_PAREN:
            input++;
            currentToken = new TokenCloseParen();
            currentTokenString = "\0";
            return currentToken;
        case T_QUOTE:
            input++;
            currentToken = new TokenQuote();
            currentTokenString = "\0";
            return currentToken;
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
            //std::cout << "Found token: " << t << std::endl;
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
    if (!input || *input == '\0')
        return T_EOF;
    //checks for OR, NOT and AND
    if (input[1] && input[0] == 'O' && input[1] == 'R')
        return T_OR;
    if (input[1] && input[2] && input[0] == 'N' && input[1] == 'O' && input[2] == 'T')
        return T_NOT;
    if (input[1] && input[2] && input[0] == 'A' && input[1] == 'N' && input[2] == 'D')
        return T_AND;
    //checks for single character tokens
    char c = *input;
    if (c == '|')
        return T_OR;
    if (c == '&')
        return T_AND;
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