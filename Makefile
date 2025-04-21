CXX = g++
CXXFLAGS = -std=c++17 -g

UTF8PROC_DIR = $(shell pwd)/../index/stemmer/utf8proc
LDFLAGS = -L$(UTF8PROC_DIR) -Wl,-rpath,$(UTF8PROC_DIR)

all: testQueryCompiler

testQueryCompiler: testQueryCompiler.cpp compiler.cpp tokenstream.cpp ../isr/isrHandler.cpp ../isr/isr.cpp ../index/index.cpp ../index/stemmer/stemmer.cpp ../utils/Utf8.cpp ../utils/searchstring.cpp ../utils/IndexBlob.cpp
	$(CXX) $(CXXFLAGS) -I. -I../isr -I../index -I../utils -I../index/stemmer -I../index/stemmer/utf8proc $^ $(LDFLAGS) $(UTF8PROC_DIR)/libutf8proc.a -o tqc

.PHONY: clean

clean:
	rm -f tqc *.o 