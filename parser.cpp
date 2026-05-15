#include "parser.h"
#include <iostream>

SLRParser::SLRParser() {
    // 文法：varDecl -> int varDef ; (3个符号)
    productions[1] = {"varDef", 3}; // Ident = initVal
    productions[2] = {"varDecl", 3}; // int varDef ;
}

SLRParser::Action SLRParser::getAction(int state, const std::string& symbol) {
    if (state == 0 && symbol == "int") return {'s', 1};
    if (state == 1 && symbol == "IDN") return {'s', 2};
    if (state == 2 && symbol == "=")   return {'s', 3};
    if (state == 3 && symbol == "INT") return {'s', 4};
    if (state == 4 && symbol == ";")   return {'r', 1}; 
    if (state == 5 && symbol == ";")   return {'s', 6};
    if (state == 6 && symbol == "$")   return {'r', 2}; // 规约成根节点
    if (state == 7 && symbol == "$")   return {'a', 0};
    return {'e', 0};
}

int SLRParser::getGoto(int state, const std::string& nt) {
    if (nt == "varDef") return 5;
    if (nt == "varDecl") return 7;
    return 0;
}

ParseTree* SLRParser::parse(const std::vector<Token>& tokens) {
    while(!state_stack.empty()) state_stack.pop();
    state_stack.push(0);
    int step = 1;

    for (size_t i = 0; i < tokens.size(); ) {
        int s = state_stack.top();
        std::string a = (tokens[i].type == IDN) ? "IDN" : (tokens[i].type == INT ? "INT" : tokens[i].value);
        
        Action act = getAction(s, a);
        if (act.type == 's') {
            std::cout << step++ << "\t" << a << "#" << tokens[i].value << "\tmove" << std::endl;
            state_stack.push(act.val);
            symbol_stack.push(new ParseTree(a, tokens[i].value));
            i++;
        } else if (act.type == 'r') {
            auto prod = productions[act.val];
            std::cout << step++ << "\t" << prod.first << "#" << a << "\treduction" << std::endl;
            ParseTree* node = new ParseTree(prod.first);
            for (int j = 0; j < prod.second; j++) {
                state_stack.pop();
                node->children.insert(node->children.begin(), symbol_stack.top());
                symbol_stack.pop();
            }
            symbol_stack.push(node);
            state_stack.push(getGoto(state_stack.top(), prod.first));
        } else if (act.type == 'a') {
            std::cout << "Parsing Successful (Accept)" << std::endl;
            return symbol_stack.top();
        } else {
            std::cerr << "Syntax Error!" << std::endl;
            return nullptr;
        }
    }
    return nullptr;
}