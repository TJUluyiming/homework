// lexer.cpp
#include "lexer.h"
#include <algorithm>
#include <cctype>

Lexer::Lexer(const std::string& code) : source(code), pos(0), current_line(1) {
    // 关键字：1-8 
    kw_map = {{"int", 1}, {"void", 2}, {"return", 3}, {"const", 4}, {"main", 5}, {"float", 6}, {"if", 7}, {"else", 8}};
    // 运算符：6-19 + 26 (修复 ! 与界符 ( 的冲突)
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
    while (pos < source.length()) {
        char curr = source[pos];
        
        if (curr == '\n') {
            current_line++;
            pos++;
            continue;
        }
        if (isspace(curr)) {
            pos++;
            continue;
        }
        
        // 基于有限自动机理论处理行注释与块注释
        if (curr == '/' && pos + 1 < source.length() && source[pos + 1] == '/') {
            pos += 2;
            while (pos < source.length() && source[pos] != '\n') pos++;
            continue;
        }
        if (curr == '/' && pos + 1 < source.length() && source[pos + 1] == '*') {
            pos += 2;
            while (pos + 1 < source.length() && !(source[pos] == '*' && source[pos + 1] == '/')) {
                if (source[pos] == '\n') current_line++;
                pos++;
            }
            if (pos + 1 < source.length()) pos += 2;
            continue;
        }
        
        // 标识符与关键字
        if (isalpha(curr) || curr == '_') {
            std::string s;
            while (pos < source.length() && (isalnum(source[pos]) || source[pos] == '_')) {
                s += source[pos++];
            }
            std::string lower_s = toLower(s);
            if (kw_map.count(lower_s)) {
                tokens.push_back({KW, "KW", s, kw_map[lower_s], current_line});
            } else {
                Token t = {IDN, "IDN", s, 0, current_line};
                tokens.push_back(t);
                if (symbol_table.find(s) == symbol_table.end()) {
                    symbol_table[s] = t;
                }
            }
            continue;
        }
        
        // 常数（整型与浮点型自动化扫描）
        if (isdigit(curr)) {
            std::string s;
            bool is_f = false;
            while (pos < source.length() && (isdigit(source[pos]) || source[pos] == '.')) {
                if (source[pos] == '.') is_f = true;
                s += source[pos++];
            }
            tokens.push_back({is_f ? FLOAT_CONST : INT_CONST, 
                             is_f ? "FLOAT" : "INT", 
                             s, is_f ? 2 : 1, current_line});
            continue;
        }
        
        // 多目及单目符号识别
        if (pos + 1 < source.length()) {
            std::string op2 = source.substr(pos, 2);
            if (op_map.count(op2)) {
                tokens.push_back({OP, "OP", op2, op_map[op2], current_line});
                pos += 2;
                continue;
            }
        }
        
        std::string op1(1, curr);
        if (op_map.count(op1)) {
            tokens.push_back({OP, "OP", op1, op_map[op1], current_line});
            pos++;
        } else if (se_map.count(op1)) {
            tokens.push_back({SE, "SE", op1, se_map[op1], current_line});
            pos++;
        } else {
            pos++; // 容错滤过未知字符
        }
    }
    tokens.push_back({END_OF_FILE, "EOF", "$", 0, current_line});
    return tokens;
}
