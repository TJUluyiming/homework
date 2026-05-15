#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stack>
#include <memory>

struct ParseTree {
    std::string type;
    std::string text;
    std::vector<ParseTree*> children;
    bool isGlobal = true; // 默认为全局变量
    ParseTree(std::string t, std::string txt = "") : type(t), text(txt) {}
    ~ParseTree() { for(auto c : children) delete c; }
};

class SLRParser {
private:
    struct Action { char type; int val; }; 
    std::stack<int> state_stack;
    std::stack<ParseTree*> symbol_stack;
    std::map<int, std::pair<std::string, int>> productions;

    Action getAction(int state, const std::string& symbol);
    int getGoto(int state, const std::string& nt);

public:
    SLRParser();
    ParseTree* parse(const std::vector<Token>& tokens);
};

#endif