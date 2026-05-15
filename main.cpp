#include "lexer.h"
#include "parser.h"
#include "ir_visitor.h"
#include <iostream>
#include <cstdio>
#include <vector>

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    std::cout << "[SYSTEM] Compiler Starter..." << std::endl;
    std::string code = "int a = 10 ;";
    
    std::cout << "\n>>> Phase 1: Lexical Analysis" << std::endl;
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();
    for (const auto& t : tokens) {
        if (t.value == "$") continue;
        std::cout << "Token: " << t.value << "\t<" << t.type_name << "," << (t.type_code == 0 ? t.value : std::to_string(t.type_code)) << ">" << std::endl;
    }

    std::cout << "\n>>> Phase 2: Syntax Analysis (SLR)" << std::endl;
    SLRParser parser;
    ParseTree* ast = parser.parse(tokens);
    
    if (ast) {
        std::cout << "[SUCCESS] AST Constructed." << std::endl;
        std::cout << "\n>>> Phase 3: Semantic Analysis & IR Generation" << std::endl;
        IRVisitor visitor;
        visitor.visit(ast);
        delete ast;
    } else {
        std::cerr << "[ERROR] Parsing failed!" << std::endl;
    }

    std::cout << "\n[FINISHED] All tasks completed successfully." << std::endl;
    return 0;
}