// lexer.cpp
#include "lexer.h"
#include <algorithm>
#include <cctype>

Lexer::Lexer(const std::string& code) : source(code), pos(0), current_line(1) {
    // 关键字：1-8 
    kw_map = {{"int", 1}, {"void", 2}, {"return", 3}, {"const", 4}, {"main", 5}, {"float", 6}, {"if", 7}, {"else", 8}};
    // 运算符：6-19+26 
    op_map = {{"+", 6}, {"-", 7}, {"*", 8}, {"/", 9}, {"%", 10}, {"=", 11}, {">", 12}, {"<", 13}, 
              {"==", 14}, {"<=", 15}, {">=", 16}, {"!=", 17}, {"&&", 18}, {"||", 19}, {"!", 26}};
    // 界符：20-25 
    se_map = {{"(", 20}, {")", 21}, {"{", 22}, {"}", 23}, {";", 24}, {",", 25}};
}

std::string Lexer::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    symbol_table.clear(); // 清空符号表
    
    while (pos < source.length()) {
        char curr = source[pos];
        if (isspace(curr)) { 
            if (curr == '\n') current_line++; 
            pos++; 
            continue; 
        }
        
        // 标识符和关键字
        if (isalpha(curr) || curr == '_') {
            std::string s;
            while (pos < source.length() && (isalnum(source[pos]) || source[pos] == '_')) 
                s += source[pos++];
            std::string ls = toLower(s);
            
            if (kw_map.count(ls)) {
                tokens.push_back({KW, "KW", ls, kw_map[ls], current_line});
            } else {
                Token t = {IDN, "IDN", s, 0, current_line};
                tokens.push_back(t);
                // 将标识符加入符号表（去重）
                if (symbol_table.find(s) == symbol_table.end()) {
                    symbol_table[s] = t;
                }
            }
        }
        // 数字常量
        else if (isdigit(curr)) {
            std::string s; 
            bool is_f = false;
            while (pos < source.length() && (isdigit(source[pos]) || source[pos] == '.')) {
                if (source[pos] == '.') is_f = true;
                s += source[pos++];
            }
            tokens.push_back({is_f ? FLOAT_CONST : INT_CONST, 
                             is_f ? "FLOAT" : "INT", 
                             s, is_f ? 2 : 1, current_line});
        }
        // 运算符和界符
        else {
            std::string op2 = source.substr(pos, 2);
            if (op_map.count(op2)) { 
                tokens.push_back({OP, "OP", op2, op_map[op2], current_line}); 
                pos += 2; 
            }
            else {
                std::string op1(1, curr);
                if (op_map.count(op1)) {
                    tokens.push_back({OP, "OP", op1, op_map[op1], current_line});
                }
                else if (se_map.count(op1)) {
                    tokens.push_back({SE, "SE", op1, se_map[op1], current_line});
                }
                pos++;
            }
        }
    }
    tokens.push_back({END_OF_FILE, "EOF", "$", -1, current_line});
    return tokens;
}
