#include "compiler.h"
#include "../isr/isrHandler.h"
#include <iostream>
bool QueryParser::IsBaseTerm()
    {
    return tokenStream.ReadTokenType() == T_WORD || tokenStream.ReadTokenType() == T_OPEN_PAREN 
    || tokenStream.ReadTokenType() == T_QUOTE || tokenStream.ReadTokenType() == T_NOT;
    }

//compiles an OR constraint
//<OrC> ::= <AndC> { <OrOp> <AndC> }
ISR* QueryParser::OrC()
    {
    vector<ISR*> terms;
    ISR* andTerm = AndC();
    if (andTerm == nullptr)
        return nullptr;
    terms.push_back(andTerm);
    //recursively compiles further OR terms
    while ( tokenStream.match(T_OR) )
        {
        std::cout << "Gobbled up OR" << std::endl;
        ISR* andTerm = AndC();
        if (andTerm == nullptr)
            return nullptr;
        terms.push_back(andTerm);
        }
    //if there is only one term, return it
    if (terms.size() == 1)
        return terms[0];
    //otherwise, compile the OR constraint
    return handler.OpenISROr(terms.data(), terms.size());
    }

//compiles an AND constraint
//<AndC> ::= <BaseC> { <BaseC> }
ISR* QueryParser::AndC()
    {
    vector<ISR*> terms;
    ISR* baseTerm = BaseC();
    if (baseTerm == nullptr)
        return nullptr;
    terms.push_back(baseTerm);  
    //recursively compiles further AND terms
    while ( IsBaseTerm() )
        {
        baseTerm = BaseC();
        terms.push_back(baseTerm);
        }
    //if there is only one term, return it
    if (terms.size() == 1)
        return terms[0];
    //otherwise, compile the AND constraint
    return handler.OpenISRAnd(terms.data(), terms.size());
    }

//finds base constraint (parentheses, quotes, words, NOT)
ISR* QueryParser::BaseC()
    {
    if ( tokenStream.match( T_WORD ) )
        {
        std::cout << "Gobbled up word: " << tokenStream.CurrentTokenString() << std::endl;
        return handler.OpenISRWord( tokenStream.CurrentTokenString().c_str() );
        }
    else if ( tokenStream.match( T_OPEN_PAREN ) )
        {
        std::cout << "Gobbled up open parenthesis" << std::endl;
        ISR* paren = ParenC();
        if ( paren == nullptr )
            return nullptr;
        return paren;
        }
    else if ( tokenStream.match( T_QUOTE ) )
        {
        std::cout << "Gobbled up quote" << std::endl;
        ISR* quote = QuoteC();
        if ( quote == nullptr )
            return nullptr;
        return quote;
        }
    else if ( tokenStream.match( T_NOT ) )
        {
        std::cout << "Gobbled up NOT" << std::endl;
        return nullptr;
        }
    else
        {
        return nullptr;
        }
    }

ISR* QueryParser::ParenC()
    {
    return nullptr;
    }

ISR* QueryParser::QuoteC()
    {
    return nullptr;
    }
