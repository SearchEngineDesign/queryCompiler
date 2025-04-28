#pragma once

#include "tokenstream.h"
#include "../isr/isrHandler.h"
#include "../isr/isr.h"
#include "stopwords.h"
class QueryParser
{
public:
    //constructors
    QueryParser(char* s, char type) : tokenStream(s), handler(), type(type) {
        handler.SetIndexReadHandlerPtr(&readHandler);
    };
    QueryParser(string& s, char type): tokenStream(s), handler(), type(type) {
        handler.SetIndexReadHandlerPtr(&readHandler);
    };
    int SetIndexReadHandler(string& pathname) { 
        int ret = readHandler.ReadIndex(pathname.c_str());
        handler.SetIndexReadHandlerPtr(&readHandler);
        return ret;
    }
    int SetIndexReadHandler(const char* pathname) { 
        int ret = readHandler.ReadIndex(pathname);
        handler.SetIndexReadHandlerPtr(&readHandler);
        return ret;
    }

    IndexReadHandler& getIndexReadHandler() { return readHandler; }
    ISRHandler& getISRHandler() { return handler; }
    
    ISR* compile( ) ;
    
    // Getter for flattenedWords
    vector<ISRWord*> & getFlattenedWords() { return flattenedWords; }
    vector<ISRWord*>& getFlattenedTitles() { return flattenedTitles; }

private:
    TokenStream tokenStream;
    ISRHandler handler;
    IndexReadHandler readHandler;
    vector<ISRWord*> flattenedWords;
    vector<ISRWord*> flattenedTitles;
    vector<ISR*> included;
    vector<ISR*> excluded;
    char type;
    
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