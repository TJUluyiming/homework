#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

// 定义种别码
enum TokenType { KW, OP, SE, IDN, INT, FLOAT, END_OF_FILE };

struct Token {
    TokenType type;
    std::string type_name;
    std::string value;
    int type_code; 
};

class Lexer {
private:
    std::string source;
    size_t pos;
    std::map<std::string, int> kw_map;
    std::map<std::string, int> op_map;
    std::map<std::string, int> se_map;

public:
    Lexer(const std::string& code);
    std::vector<Token> tokenize();
};

#endif