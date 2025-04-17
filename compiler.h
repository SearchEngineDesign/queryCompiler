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
    ISR* OrC( );
    ISR* AndC( );
    ISR* BaseC( );
    ISR* ParenC( );
    ISR* QuoteC( );
    bool IsBaseTerm( );
    

private:
    TokenStream tokenStream;
    ISRHandler handler;
    IndexReadHandler readHandler;
};

