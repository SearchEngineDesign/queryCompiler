#include "compiler.h"
#include "../isr/isrHandler.h"
#include "../index/stemmer/stemmer.h"
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
    if (andTerm == nullptr) {
        if (!tokenStream.match(T_OR))
            {
            // std::cout << "OrC returned nullptr" << std::endl;
            return nullptr;
            }
        andTerm = AndC();
        if (andTerm == nullptr)
            return nullptr;
    }
    terms.push_back(andTerm);
    
    while (tokenStream.match(T_OR)) {
        ISR* andTerm = AndC();
        if (andTerm == nullptr)
            throw std::runtime_error("Error: Expected term after OR");
        terms.push_back(andTerm);
    }
    
    if (terms.size() == 1)
        return terms[0];
    //allocate a dynamic array for terms
    ISR** termsArray = new ISR*[terms.size()];
    for (size_t i = 0; i < terms.size(); i++)
        termsArray[i] = terms[i];
    return handler.OpenISROr(termsArray, terms.size());
}

//compiles an AND constraint
//<AndC> ::= <BaseC> { <BaseC> }
ISR* QueryParser::AndC()
    {
    vector<ISR*> terms;
    ISR* baseTerm = BaseC();
    terms.push_back(baseTerm);
    //clears AND token
    tokenStream.match(T_AND);
    
    //recursively compiles further AND terms
    while ( IsBaseTerm() )
        {
        baseTerm = BaseC();
        //clears AND token
        tokenStream.match(T_AND);
        terms.push_back(baseTerm);
        }
    //if there is only one term, return it
    if (terms.size() == 1)
        return terms[0];
    //allocate a dynamic array for terms
    ISR** termsArray = new ISR*[terms.size()];
    for (size_t i = 0; i < terms.size(); i++) {
      termsArray[i] = terms[i];
      if (terms[i] == nullptr)
         {
         delete[] termsArray;
         return nullptr;
         }
    }
    //otherwise, compile the AND constraint
    return handler.OpenISRAnd(termsArray, terms.size());
    }

//finds base constraint (parentheses, quotes, words, NOT)
ISR* QueryParser::BaseC()
    {
    if (tokenStream.match(T_WORD))
        {
        string w = stemWord(tokenStream.CurrentTokenString());
        ISRWord* word = handler.OpenISRWord(w.c_str());
        string titleStr(string("@") + w);
        ISRWord* title = handler.OpenISRWord(titleStr.c_str());

        if (word != nullptr) {
            // Only store basic ISRWord, not composite structures
            flattenedWords.push_back(word);
        }
        if (title != nullptr) {
            flattenedTitles.push_back(title);
        }
        if (type == 'b') {
            return word;
        }
        else if (type == 't') {
            return title;
        }
        return nullptr;
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
// TODO: shoude directly call phrase, or how do you recognize a phrase or a normal word?
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

//compiles a phrase constraint of one or more words.
//<wordC> ::= <word> { <word> }
ISR* QueryParser::wordC()
    {
    if (tokenStream.match(T_WORD))
        {
        vector<ISR*> terms;
        //std::cout << "Gobbled up word in quote: " << tokenStream.CurrentTokenString() << std::endl;
        string w = stemWord( tokenStream.CurrentTokenString() );
        ISRWord* word = handler.OpenISRWord(w.c_str());
        string titleStr(string("@") + w);
        ISRWord* title = handler.OpenISRWord(titleStr.c_str());
        if (word != nullptr)
            {
            flattenedWords.push_back(word);
            if (type == 'b')
                terms.push_back(word);
            }
        if (title != nullptr)
            {
            flattenedTitles.push_back(title);
            if (type == 't')
                terms.push_back(title);
            }
        while (tokenStream.match(T_WORD))
            {
            string w = stemWord(tokenStream.CurrentTokenString());
            ISRWord* word = handler.OpenISRWord(w.c_str());
            string titleStr(string("@") + w);
            ISRWord* title = handler.OpenISRWord(titleStr.c_str());
            if (word != nullptr)
                {
                flattenedWords.push_back(word);
                if (type == 'b')
                    terms.push_back(word);
                }
            if (title != nullptr)
                {
                flattenedTitles.push_back(title);
                if (type == 't')
                    terms.push_back(title);
                }
            }
        if (terms.size() == 1)
            return terms[0];
        else
            {
            //allocate a dynamic array for terms
            ISR** termsArray = new ISR*[terms.size()];
            for (size_t i = 0; i < terms.size(); i++)
                termsArray[i] = terms[i];
            return handler.OpenISRPhrase(termsArray, terms.size());
            }
        }
    else
        return nullptr;
    }

ISR* QueryParser::compile()
    {
    // vector<ISR*> included;
    // vector<ISR*> excluded;
    included.clear();
    excluded.clear();
    flattenedWords.clear();  // Clear any previous words
    flattenedTitles.clear();

    try 
        {
        while (tokenStream.ReadTokenType() != T_EOF)
            {
            if (tokenStream.match(T_NOT))
               {
               ISR* baseTerm = BaseC();
               if (baseTerm != nullptr)
                  excluded.push_back(baseTerm);
               }
            else
               {
               ISR* orTerm = OrC();
               if (orTerm != nullptr)
                  included.push_back(orTerm);
               }
            }
        }   
    catch (std::runtime_error& e)
        {
        std::cerr << e.what() << std::endl;
        return nullptr;
        }
    if (included.size() == 0)
        {
        std::cerr << "Error: No included constraints" << std::endl;
        return nullptr;
        }
    else
        {
        std::cout << "num of included: " << included.size() << ", num of excluded: " << excluded.size() << std::endl;
        std::cout << "num of flattened words: " << flattenedWords.size() << std::endl;
        std::cout << "num of flattened titles: " << flattenedTitles.size() << std::endl;
        std::cout << "--------------------------------" << std::endl;
        }

    if (excluded.size() == 0)
        {
            if (included.size() == 1)
                return included[0];
            else
                return handler.OpenISRAnd(included.data(), included.size());
        }

    return handler.OpenISRContainer(included.data(), excluded.data(), included.size(), excluded.size());
    }
