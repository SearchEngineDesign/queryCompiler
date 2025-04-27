CXX = g++
CXXFLAGS = -std=c++17 -g

UTF8PROC_DIR = $(shell pwd | sed 's/ /\\ /g')/../index/stemmer/utf8proc
LDFLAGS = -L$(UTF8PROC_DIR) -Wl,-rpath,$(UTF8PROC_DIR)

all: testQueryCompiler

testQueryCompiler: testQueryCompiler.cpp compiler.cpp tokenstream.cpp ../isr/isrHandler.cpp ../isr/isr.cpp ../index/index.cpp ../index/stemmer/stemmer.cpp ../include/cf/Utf8.cpp ../include/cf/searchstring.cpp ../include/cf/IndexBlob.cpp stopwords.cpp
	$(CXX) $(CXXFLAGS) -I. -I../isr -I../index -I../utils -I../include -I../index/stemmer -I$(UTF8PROC_DIR) $^ $(LDFLAGS) $(UTF8PROC_DIR)/libutf8proc.a -o tqc

.PHONY: clean

clean:
	rm -f tqc *.o 