CXX = g++
CXXFLAGS = -std=c++17 -g

LDFLAGS = -L../index/stemmer/utf8proc -Wl,-rpath,../index/stemmer/utf8proc

all: testQueryCompiler

testQueryCompiler: testQueryCompiler.cpp compiler.cpp tokenstream.cpp ../isr/isrHandler.cpp ../isr/isr.cpp ../index/index.cpp ../index/stemmer/stemmer.cpp ../utils/Utf8.cpp ../utils/searchstring.cpp
	$(CXX) $(CXXFLAGS) -I. -I../isr -I../index -I../utils -I../index/stemmer -I../index/stemmer/utf8proc $^ $(LDFLAGS) -lutf8proc -o tqc

.PHONY: clean

clean:
	rm -f tqc *.o 