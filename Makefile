CXX = g++
IR_INC = -Iir/include
CXXFLAGS = -std=c++17 -Wall -O2 -Iinc $(IR_INC)

LEXER_SRC = src/lexer.cpp src/FSM.cpp src/lexical.cpp src/SymbolTable.cpp
IR_SRC = $(wildcard ir/src/*.cpp)
COMPILER_SRC = src/compiler.cpp $(LEXER_SRC) src/parser.cpp src/ir_visitor.cpp $(IR_SRC)

.PHONY: all clean test lexer parser

all: compiler.exe

compiler.exe: $(COMPILER_SRC) inc/config.h inc/parser.h inc/ir_visitor.h test/grammar.txt
	$(CXX) $(CXXFLAGS) -o $@ $(COMPILER_SRC)

test: compiler.exe
	compiler.exe

# 可选：单独词法/语法 exe（需 -DBUILD_LEXER_EXE / -DBUILD_PARSER_EXE）
lexer.exe: $(LEXER_SRC) inc/config.h
	$(CXX) $(CXXFLAGS) -DBUILD_LEXER_EXE -o $@ $(LEXER_SRC)

parser.exe: src/parser.cpp inc/config.h inc/parser.h test/grammar.txt
	$(CXX) $(CXXFLAGS) -DBUILD_PARSER_EXE -o $@ src/parser.cpp

clean:
	del /Q compiler.exe lexer.exe parser.exe 2>nul || rm -f compiler.exe lexer.exe parser.exe
