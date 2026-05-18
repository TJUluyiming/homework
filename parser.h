// parser.h
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stack>
#include <map>
#include <vector>
#include <string>
#include <set>

struct ParseTree {
    std::string type;
    std::string text;
    std::vector<ParseTree*> children;
    ParseTree(std::string t, std::string txt = "") : type(t), text(txt) {}
    ~ParseTree() { for(auto* c : children) delete c; }
};

class SLRParser {
private:
    struct Production {
        int id;
        std::string lhs;
        std::vector<std::string> rhs;
    };
    
    struct LRItem {
        int prod_id;
        int dot;
        bool operator<(const LRItem& o) const {
            if (prod_id != o.prod_id) return prod_id < o.prod_id;
            return dot < o.dot;
        }
        bool operator==(const LRItem& o) const {
            return prod_id == o.prod_id && dot == o.dot;
        }
    };
    
    struct Action { char type; int val; }; 
    
    std::vector<Production> prods_list;
    std::map<std::string, std::vector<int>> prod_map;
    std::set<std::string> terminals;
    std::set<std::string> non_terminals;
    
    std::map<std::string, std::set<std::string>> first_sets;
    std::map<std::string, std::set<std::string>> follow_sets;
    std::vector<std::set<LRItem>> states;
    
    std::vector<std::map<std::string, Action>> action_table;
    std::vector<std::map<std::string, int>> goto_table;
    
    std::set<LRItem> getClosure(const std::set<LRItem>& inst);
    std::set<LRItem> getGotoState(const std::set<LRItem>& state, const std::string& sym);
    void buildEngine();
    std::string getSymbolName(const Token& t);

public:
    SLRParser();
    ParseTree* parse(const std::vector<Token>& tokens);
};

#endif
