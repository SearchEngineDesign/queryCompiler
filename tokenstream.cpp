#include "tokenstream.h"
#include "tokentype.h"

bool reserved(char c)
    {
    return c == '|' || c == '!' || c == '(' || c == ')' || c == '"' || isspace(c);
    }

Token* TokenStream::TakeToken( ) 
    {
    //get rid of leading whitespace
    while ( isspace( *input ) )
        input++;
    if ( *input == '\0' )
        return new TokenEOF();
    string t(input[0]);
    input++;
    if (t == "(")
        return new TokenOpenParen();
    if (t == ")")
        return new TokenCloseParen();
    if (t == "\"")
        return new TokenQuote();
    if (t == "|") 
        {
        //checks both || and |
        if (input[1] == '|')
            input++;
        return new TokenOr();
        }
    if (t == "!")
        return new TokenNot();
    //reads word
    while ( !reserved(*input) )
        {
        t += *input;
        input++;
        if (t == "OR")
            return new TokenOr();
        if (t == "NOT")
            return new TokenNot();
        }
    Token* token = new TokenWord(t);
    currentToken = token;
    currentTokenString = t;
    return token;
    }
//read what the token type is at input
TokenType TokenStream::ReadTokenType()
    {
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