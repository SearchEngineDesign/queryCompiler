#pragma once

#include "tokenstream.h"
#include "../isr/isrHandler.h"

class TokenObject
{
public:
    TokenObject *Next;
    virtual ISR *Compile( );
    TokenObject( );
};

class TokenList : TokenObject
{
public:
    TokenObject *Top,
    *Bottom;
    void Empty( );
    void Append( TokenObject *t );
    TokenList( );
};

class QueryParser
{
public:
    Token *FindNextToken( );
    TokenObject *FindOrConstraint( );
    bool FindOrOp( );
    TokenObject *FindConstraint( );
    TokenObject *FindBase( );
    TokenObject *FindPhrase( );
    TokenObject *FindParentheses( );
    TokenObject *FindSearchWord( );
    //constructor
    //if string is true, s is a query and gets converted to istream
    //if string is false, s is a filename that istream reads from
    QueryParser( string& s, bool string ) {
        tokenStream = TokenStream(s, string);
    }
    //same thing as setting string to false. Initializes istream from filename
    QueryParser ( string& filename ) {
        tokenStream = TokenStream(filename);
    }
    //initialize istream from istream
    QueryParser ( std::ifstream& stream ) {
        tokenStream = TokenStream(stream);
    }
private:
    TokenStream tokenStream;
};

