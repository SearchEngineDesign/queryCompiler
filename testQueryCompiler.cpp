#include "compiler.h"
#include <iostream>
#include <cf/searchstring.h>
#include "../isr/isrHandler.h"
int main() {
    // Set the query string directly for testing

    string query = "i me myself";

    // string test = "wiki";
    std::cout << "string test" << std::endl;
    string w;
    w.push_back('a');
    w.push_back('b');
    w.push_back('c');
    w.push_back('d');
    w.push_back('e');
    w.push_back('f');
    w.push_back('g');
    std::cout << w.c_str() << std::endl;
    // Construct QueryParser with the custom string class
    QueryParser parser(query, 'b');
    string path = "../log/chunks/31";

    // Parse the query into an ISR object (using OrC as the top-level entry)
    parser.SetIndexReadHandler(path);
    ISR* isr = parser.compile();
    const vector<ISRWord*>& flattenedWords = parser.getFlattenedWords();
    const vector<ISRWord*>& flattenedTitles = parser.getFlattenedTitles();

    // for (const auto& word : flattenedWords) {
    //     std::cout << "Flattened word: " << word << std::endl;
    // }
   if (isr == nullptr)
      return 0;
   
    // Seek through results and output matching documents
    int i = 0;
    size_t target = 0;
    while (isr->Seek(target) != nullptr) {
        std::cout << "matching doc: " << isr->GetMatchingDoc() << std::endl;

        if (i == 10) // Limit output to 10 results
            break;
        i++;
        target = isr->EndDoc->GetStartLocation() + 1;
        std::cout << "target: " << target << "\n";
    }

    ISRHandler isrHandler;
    isrHandler.CloseISR(isr);
    return 0;
} 