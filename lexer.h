// lexer.h
#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

enum TokenType { KW, OP, SE, IDN, INT_CONST, FLOAT_CONST, END_OF_FILE };

struct Token {
    TokenType type;
    std::string type_name;
    std::string value;
    int type_code;
    int line;
};

class Lexer {
private:
    std::string source;
    size_t pos;
    int current_line;
    std::map<std::string, int> kw_map;
    std::map<std::string, int> op_map;
    std::map<std::string, int> se_map;
    std::string toLower(std::string s);

public:
    std::map<std::string, Token> symbol_table; // 词法分析符号表
    Lexer(const std::string& code);
    std::vector<Token> tokenize();
};

#endif
