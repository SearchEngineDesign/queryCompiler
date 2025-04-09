#include "tokenstream.h"
  
    Token TokenStream::currentToken( ) {
        return currentToken_;
    }
    Token TokenStream::TakeToken( ) {
        if ( !( input >> currentTokenString_ ) )
            return Token( "/0", TokenEOF );
        currentToken_ = Token( table.FindToken( currentTokenString_ ) );
        return currentToken_;
    }