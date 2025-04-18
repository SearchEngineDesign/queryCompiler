#include "compiler.h"
#include "../isr/isrHandler.h"
#include "../stemmer/stemmer.h"
#include <stdexcept>
//#include <iostream>

bool QueryParser::IsBaseTerm()
    {
    return tokenStream.ReadTokenType() == T_WORD || tokenStream.ReadTokenType() == T_OPEN_PAREN 
    || tokenStream.ReadTokenType() == T_QUOTE;
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
        //std::cout << "Gobbled up OR" << std::endl;
        ISR* andTerm = AndC();
        if (andTerm == nullptr)
            throw std::runtime_error("Error: Expected constraint after OR");
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
        if (baseTerm == nullptr)
            break;
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
        //std::cout << "Gobbled up word: " << tokenStream.CurrentTokenString() << std::endl;
        string w = stemWord( tokenStream.CurrentTokenString() );
        return handler.OpenISRWord( w.c_str() );
        }
    else if ( tokenStream.ReadTokenType() == T_OPEN_PAREN )
        {
        //std::cout << "see open parenthesis" << std::endl;
        return ParenC();
        }
    else if ( tokenStream.ReadTokenType() == T_QUOTE )
        {
        //std::cout << "see quote" << std::endl;
        return QuoteC();
        }
    else if ( tokenStream.ReadTokenType() == T_NOT )
        {
        //std::cout << "hit NOT" << std::endl;
        return nullptr;
        }
    else
        return nullptr;
    }

//compiles a constraint in parentheses
//<ParenC> ::= ( <OrC> )
ISR* QueryParser::ParenC()
    {
    if (tokenStream.match(T_OPEN_PAREN))
        {
        //std::cout << "Gobbled up open parenthesis" << std::endl;
        ISR* constraint = OrC();
        if (constraint == nullptr)
            throw std::runtime_error("Error: Expected constraint after open parenthesis");
        if (tokenStream.match(T_CLOSE_PAREN))
            return constraint;
        else
            throw std::runtime_error("Error: Expected close parenthesis after constraint");
        }
    else
        return nullptr;
    }

//compiles a constraint in quotes
//<QuoteC> ::= " <wordC> "
ISR* QueryParser::QuoteC()
    {
    if (tokenStream.match(T_QUOTE))
        {
        //std::cout << "Gobbled up quote" << std::endl;
        ISR* constraint = wordC();
        if (constraint == nullptr)
            throw std::runtime_error("Error: Expected constraint after quote");
        if (tokenStream.match(T_QUOTE))
            return constraint;
        else
            throw std::runtime_error("Error: Expected close quote after constraint");
        }
    else
        return nullptr;
    }

//compiles an AND constraint of one or more words. JUST WORDS
//<wordC> ::= <word> { <word> }
ISR* QueryParser::wordC()
    {
    if (tokenStream.match(T_WORD))
        {
        vector<ISR*> terms;
        //std::cout << "Gobbled up word: " << tokenStream.CurrentTokenString() << std::endl;
        string w = stemWord( tokenStream.CurrentTokenString() );
        terms.push_back(handler.OpenISRWord( w.c_str() ));
        while (tokenStream.match(T_WORD))
            {
            //std::cout << "Gobbled up word: " << tokenStream.CurrentTokenString() << std::endl;
            string w = stemWord( tokenStream.CurrentTokenString() );
            terms.push_back(handler.OpenISRWord( w.c_str() ));
            }
        if (terms.size() == 1)
            return terms[0];
        else
            return handler.OpenISRAnd(terms.data(), terms.size());
        }
    else
        return nullptr;
    }

ISRContainer* QueryParser::compile()
    {
    vector<ISR*> included;
    vector<ISR*> excluded;
    try 
        {
        while (tokenStream.ReadTokenType() != T_EOF)
            {
            if (tokenStream.match( T_NOT ) )
                excluded.push_back(BaseC());
            else
                included.push_back(OrC());
            }
        }   
    catch (std::runtime_error& e)
        {
        //std::cerr << e.what() << std::endl;
        return nullptr;
        }
    if (included.size() == 0)
        {
        //std::cerr << "Error: No included constraints" << std::endl;
        return nullptr;
        }
    return handler.OpenISRContainer(included.data(), excluded.data(), included.size(), excluded.size());
    }
