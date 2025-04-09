#include "tokenstream.h"

class TokenStream {
    public:
    //initialize from string
    TokenStream(const string& content, bool string) {
        if (string)
            {
            std::ofstream f("temp.txt");
            f << content;
            f.close();
            input.open("temp.txt");
            }
        else
            input.open(content.c_str());
    }
    //initialize from file
    TokenStream(const string& filename) {
        input.open(filename.c_str());
    }
    TokenStream ( std::ifstream &stream ) {
        input = std::move(stream);
    }
    Token currentToken( ) {
        return currentToken_;
    }
    Token TakeToken( ) {
        if ( !( input >> currentTokenString_ ) )
            return Token( "/0", TokenEOF );
        currentToken_ = Token( table.FindToken( currentTokenString_ ) );
        return currentToken_;
    }
    private:
    std::ifstream input;
    string currentTokenString_;
    Token currentToken_;
    TokenTable table;
};