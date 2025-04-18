#pragma once

#include "tokenstream.h"
#include "../isr/isrHandler.h"
class QueryParser
{
public:
    //constructors
    QueryParser(char* s) : tokenStream(s), handler() {
        handler.SetIndexReadHandler(&readHandler);
    };
    QueryParser(string& s): tokenStream(s), handler() {
        handler.SetIndexReadHandler(&readHandler);
    };
    void SetIndexReadHandler(string& pathname) { 
        readHandler.ReadIndex(pathname.c_str());
        handler.SetIndexReadHandler(&readHandler);
    }
    void SetIndexReadHandler(char* pathname) { 
        readHandler.ReadIndex(pathname);
        handler.SetIndexReadHandler(&readHandler);
    }
    ISRContainer* compile( ) ;
    

private:
    TokenStream tokenStream;
    ISRHandler handler;
    IndexReadHandler readHandler;
    //compiles the query into an ISRContainer
    //returns nullptr if there is an error, writes error to cerr
    //please call the close method on the returned pointer when done
    //constraints start at OrC and prase recursively down
    //will parse until NOT or EOF is hit
    ISR* OrC( );
    ISR* AndC( );
    ISR* BaseC( );
    ISR* ParenC( );
    ISR* QuoteC( );
    ISR* wordC( );
    bool IsBaseTerm( );
};

