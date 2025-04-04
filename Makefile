CXX = g++
CXXFLAGS = -std=c++11 -g

# OPENSSL_DIR = /opt/homebrew/opt/openssl@3
# INCLUDES = -I$(OPENSSL_DIR)/include
# LDFLAGS = -L$(OPENSSL_DIR)/lib

OPENSSL_DIR = /usr/include/openssl
INCLUDES = -I$(OPENSSL_DIR)
LDFLAGS = -L/usr/lib

all: search

search: main.cpp parser/HtmlParser.cpp parser/HtmlTags.cpp Crawler/crawler.cpp utils/string.cpp index/index.cpp frontier/frontier.cpp utils/Utf8.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ $(LDFLAGS) -lssl -lcrypto -lz -o search -g


.PHONY: clean

clean:
	rm -f search 
