CXX = g++
CXXFLAGS = -std=c++11 -g

OPENSSL_DIR = /opt/homebrew/opt/openssl@3
INCLUDES = -I$(OPENSSL_DIR)/include
LDFLAGS = -L$(OPENSSL_DIR)/lib -Lindex/stemmer/utf8proc

all: search

search: main.cpp parser/HtmlParser.cpp parser/HtmlTags.cpp crawler/crawler.cpp utils/string.cpp index/index.cpp frontier/frontier.cpp utils/Utf8.cpp index/stemmer/stemmer.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ $(LDFLAGS) -lssl -lcrypto -lz -lutf8proc -o search -g

.PHONY: clean

clean:
	rm -f search 
