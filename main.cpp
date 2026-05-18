// main.cpp
#include "lexer.h"
#include "parser.h"
#include "ir_visitor.h"
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    // 测试用例
    std::string code = "int a = 10 ; int main() { return 0 ; }";
    
    // === 阶段1：词法分析 ===
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    std::cout << ">>> Phase 1: Lexical Analysis\n";
    for (const auto& t : tokens) {
        if(t.type != END_OF_FILE) {
            std::cout << t.value << "\t<" << t.type_name << "," << t.type_code << ">\n";
        }
    }
    
    // 输出符号表（大作业要求）
    std::cout << "\n>>> Symbol Table\n";
    for (const auto& pair : lexer.symbol_table) {
        std::cout << pair.first << "\t<IDN," << pair.second.value << ">\n";
    }
    
    // === 阶段2：语法分析 ===
    std::cout << "\n>>> Phase 2: Syntax Analysis\n";
    SLRParser parser;
    ParseTree* ast = parser.parse(tokens);
    
    // === 阶段3：中间代码生成 ===
    if (ast) {
        std::cout << "\n>>> Phase 3: LLVM IR Generation\n";
        std::cout << "; ModuleID = 'cminus_module'\n\n";
        
        IRVisitor visitor;
        visitor.visit(ast);
        
        delete ast;
        std::cout << "\nCompilation completed successfully!\n";
    } else {
        std::cerr << "\nCompilation failed due to syntax errors.\n";
        return 1;
    }
    
    return 0;
}
