#include "lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& code) : source(code), pos(0) {
    kw_map = {{"int", 1}, {"void", 2}, {"return", 3}, {"const", 4}, {"main", 5}, {"float", 6}, {"if", 7}, {"else", 8}};
    op_map = {{"+", 6}, {"-", 7}, {"*", 8}, {"/", 9}, {"%", 10}, {"=", 11}, {">", 12}, {"<", 13}, {"==", 14}};
    se_map = {{"(", 20}, {")", 21}, {"{", 22}, {"}", 23}, {";", 24}, {",", 25}};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < source.length()) {
        while (pos < source.length() && isspace(source[pos])) pos++;
        if (pos >= source.length()) break;

        char curr = source[pos];
        if (isalpha(curr) || curr == '_') {
            std::string s;
            while (pos < source.length() && (isalnum(source[pos]) || source[pos] == '_')) s += source[pos++];
            if (kw_map.count(s)) tokens.push_back({KW, "KW", s, kw_map[s]});
            else tokens.push_back({IDN, "IDN", s, 0});
        } 
        else if (isdigit(curr)) {
            std::string s;
            bool is_f = false;
            while (pos < source.length() && (isdigit(source[pos]) || source[pos] == '.')) {
                if (source[pos] == '.') is_f = true;
                s += source[pos++];
            }
            if (is_f) tokens.push_back({FLOAT, "FLOAT", s, 2});
            else tokens.push_back({INT, "INT", s, 1});
        } 
        else {
            std::string one(1, curr);
            if (op_map.count(one)) { tokens.push_back({OP, "OP", one, op_map[one]}); pos++; }
            else if (se_map.count(one)) { tokens.push_back({SE, "SE", one, se_map[one]}); pos++; }
            else pos++;
        }
    }
    tokens.push_back({END_OF_FILE, "EOF", "$", -1});
    return tokens;
}