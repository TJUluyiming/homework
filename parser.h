// parser.h
#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include <stack>
#include <map>
#include <vector>

struct ParseTree {
    std::string type;
    std::string text;
    std::vector<ParseTree*> children;
    ParseTree(std::string t, std::string txt = "") : type(t), text(txt) {}
    ~ParseTree() { for(auto* c : children) delete c; }
};

class SLRParser {
private:
    struct Action { char type; int val; };
    std::stack<int> state_stack;
    std::stack<std::string> sym_stack;
    std::stack<ParseTree*> tree_stack;
    std::map<int, std::pair<std::string, int>> prods;
    Action getAction(int s, const std::string& a);
    int getGoto(int s, const std::string& nt);

public:
    SLRParser();
    ParseTree* parse(const std::vector<Token>& tokens);
};

#endif
