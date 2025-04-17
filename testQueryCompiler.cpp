#include "compiler.h"
#include <iostream>
#include "../utils/searchstring.h"

int main() {
    // Set the query string directly for testing
    string query = "apple OR banana dog";

    // Construct QueryParser with the custom string class
    QueryParser parser(query);

    // Parse the query into an ISR object (using OrC as the top-level entry)
    ISR* isr = parser.OrC();

    if (!isr) {
        std::cout << "Failed to parse query into ISR." << std::endl;
        return 1;
    }

    // Print basic info about the resulting ISR object
    std::cout << "Successfully parsed query into ISR object." << std::endl;
    std::cout << "ISR object address: " << isr << std::endl;
    // Optionally, print the type using RTTI (if available)
    std::cout << "ISR dynamic type: " << typeid(*isr).name() << std::endl;

    // Clean up
    delete isr;
    return 0;
} 