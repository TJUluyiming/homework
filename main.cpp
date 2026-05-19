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
    
    // 全功能测试用例（覆盖浮点数声明、全局变量、局部变量及主控逻辑）
    std::string code = "int a = 10 ; float b = 3.14 ; int main ( ) { return 0 ; }";
    
    // === 阶段1：词法分析 ===
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    std::cout << ">>> Phase 1: Lexical Analysis\n";
    for (const auto& t : tokens) {
        if(t.type != END_OF_FILE) {
            std::string content;
            if (t.type == IDN || t.type == INT_CONST || t.type == FLOAT_CONST) content = t.value; 
            else content = std::to_string(t.type_code); 
            std::cout << t.value << "\t<" << t.type_name << "," << content << ">\n";
        }
    }
    
    std::cout << "\n>>> Symbol Table\n";
    for (const auto& pair : lexer.symbol_table) {
        std::cout << pair.first << "\t<IDN," << pair.second.value << ">\n";
    }
    
    // === 阶段2：语法分析 ===
    std::cout << "\n>>> Phase 2: Syntax Analysis\n";
    SLRParser parser;
    ParseTree* ast = parser.parse(tokens);
    
    if (!ast) {
        std::cerr << "\nCompilation failed due to syntax errors.\n";
        return 1;
    }
    
    // === 阶段3：中间代码生成 ===
    std::cout << "\n>>> Phase 3: Intermediate Code Generation\n";
    IRVisitor visitor;
    visitor.visit(ast);
    std::cout << visitor.output() << std::endl;
    delete ast;
    return 0;
}
