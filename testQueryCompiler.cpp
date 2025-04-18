#include "compiler.h"
#include <iostream>
#include "../utils/searchstring.h"
#include "../isr/isrHandler.h"
int main() {
    // Set the query string directly for testing
    string query = "lust gluttony !wrath (greed || sloth) !(envy | jealousy)";

    // Construct QueryParser with the custom string class
    QueryParser parser(query);

    // Parse the query into an ISR object (using OrC as the top-level entry)
    ISRContainer* isr = parser.compile();
   
    ISRHandler isrHandler;
    isrHandler.CloseISR(isr);
    return 0;
} 