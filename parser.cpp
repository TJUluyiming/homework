// parser.cpp
#include "parser.h"
#include <iostream>
#include <algorithm>

SLRParser::SLRParser() {
    prods_list.push_back({0, "Program'", {"Program"}});
    prods_list.push_back({1, "Program", {"compUnit"}});
    prods_list.push_back({2, "compUnit", {"compUnit", "decl"}});
    prods_list.push_back({3, "compUnit", {"compUnit", "funcDef"}});
    prods_list.push_back({4, "compUnit", {}}); 
    prods_list.push_back({5, "decl", {"constDecl"}});
    prods_list.push_back({6, "decl", {"varDecl"}});
    prods_list.push_back({7, "constDecl", {"const", "bType", "constDefs", ";"}});
    prods_list.push_back({8, "constDefs", {"constDef"}});
    prods_list.push_back({9, "constDefs", {"constDefs", ",", "constDef"}});
    prods_list.push_back({10, "bType", {"int"}});
    prods_list.push_back({11, "bType", {"float"}}); // 仅属于 bType
    prods_list.push_back({12, "constDef", {"Ident", "=", "constInitVal"}});
    prods_list.push_back({13, "constInitVal", {"constExp"}});
    prods_list.push_back({14, "varDecl", {"bType", "varDefs", ";"}});
    prods_list.push_back({15, "varDefs", {"varDef"}});
    prods_list.push_back({16, "varDefs", {"varDefs", ",", "varDef"}});
    prods_list.push_back({17, "varDef", {"Ident"}});
    prods_list.push_back({18, "varDef", {"Ident", "=", "initVal"}});
    prods_list.push_back({19, "initVal", {"exp"}});
    prods_list.push_back({20, "funcDef", {"funcType", "Ident", "(", ")", "block"}});
    prods_list.push_back({21, "funcDef", {"funcType", "Ident", "(", "funcFParams", ")", "block"}});
    prods_list.push_back({22, "funcType", {"void"}});
    prods_list.push_back({23, "funcType", {"int"}}); // int 依然属于 funcType
    prods_list.push_back({24, "funcFParams", {"funcFParams", ",", "funcFParam"}});
    prods_list.push_back({25, "funcFParams", {"funcFParam"}});
    prods_list.push_back({26, "funcFParam", {"bType", "Ident"}});
    prods_list.push_back({27, "block", {"{", "blockItems", "}"}});
    prods_list.push_back({28, "blockItems", {"blockItems", "blockItem"}});
    prods_list.push_back({29, "blockItems", {}}); 
    prods_list.push_back({30, "blockItem", {"decl"}});
    prods_list.push_back({31, "blockItem", {"stmt"}});
    prods_list.push_back({32, "stmt", {"lVal", "=", "exp", ";"}});
    prods_list.push_back({33, "stmt", {"exp", ";"}});
    prods_list.push_back({34, "stmt", {";"}});
    prods_list.push_back({35, "stmt", {"block"}});
    prods_list.push_back({36, "stmt", {"if", "(", "cond", ")", "stmt"}});
    prods_list.push_back({37, "stmt", {"if", "(", "cond", ")", "stmt", "else", "stmt"}});
    prods_list.push_back({38, "stmt", {"return", ";"}});
    prods_list.push_back({39, "stmt", {"return", "exp", ";"}});
    prods_list.push_back({40, "exp", {"addExp"}});
    prods_list.push_back({41, "cond", {"lOrExp"}});
    prods_list.push_back({42, "lVal", {"Ident"}});
    prods_list.push_back({43, "primaryExp", {"(", "exp", ")"}});
    prods_list.push_back({44, "primaryExp", {"lVal"}});
    prods_list.push_back({45, "primaryExp", {"number"}});
    prods_list.push_back({46, "number", {"IntConst"}});
    prods_list.push_back({47, "number", {"floatConst"}});
    prods_list.push_back({48, "unaryExp", {"primaryExp"}});
    prods_list.push_back({49, "unaryExp", {"Ident", "(", ")"}});
    prods_list.push_back({50, "unaryExp", {"Ident", "(", "funcRParams", ")"}});
    prods_list.push_back({51, "unaryExp", {"unaryOp", "unaryExp"}});
    prods_list.push_back({52, "unaryOp", {"+"}});
    prods_list.push_back({53, "unaryOp", {"-"}});
    prods_list.push_back({54, "unaryOp", {"!"}});
    prods_list.push_back({55, "funcRParams", {"funcRParams", ",", "funcRParam"}});
    prods_list.push_back({56, "funcRParams", {"funcRParam"}});
    prods_list.push_back({57, "funcRParam", {"exp"}});
    prods_list.push_back({58, "mulExp", {"unaryExp"}});
    prods_list.push_back({59, "mulExp", {"mulExp", "*", "unaryExp"}});
    prods_list.push_back({60, "mulExp", {"mulExp", "/", "unaryExp"}});
    prods_list.push_back({61, "mulExp", {"mulExp", "%", "unaryExp"}});
    prods_list.push_back({62, "addExp", {"mulExp"}});
    prods_list.push_back({63, "addExp", {"addExp", "+", "mulExp"}});
    prods_list.push_back({64, "addExp", {"addExp", "-", "mulExp"}});
    prods_list.push_back({65, "relExp", {"addExp"}});
    prods_list.push_back({66, "relExp", {"relExp", "<", "addExp"}});
    prods_list.push_back({67, "relExp", {"relExp", ">", "addExp"}});
    prods_list.push_back({68, "relExp", {"relExp", "<=", "addExp"}});
    prods_list.push_back({69, "relExp", {"relExp", ">=", "addExp"}});
    prods_list.push_back({70, "eqExp", {"relExp"}});
    prods_list.push_back({71, "eqExp", {"eqExp", "==", "relExp"}});
    prods_list.push_back({72, "eqExp", {"eqExp", "!=", "relExp"}});
    prods_list.push_back({73, "lAndExp", {"eqExp"}});
    prods_list.push_back({74, "lAndExp", {"lAndExp", "&&", "eqExp"}});
    prods_list.push_back({75, "lOrExp", {"lAndExp"}});
    prods_list.push_back({76, "lOrExp", {"lOrExp", "||", "lAndExp"}});
    prods_list.push_back({77, "constExp", {"addExp"}});

    for (const auto& p : prods_list) {
        non_terminals.insert(p.lhs);
        prod_map[p.lhs].push_back(p.id);
    }
    for (const auto& p : prods_list) {
        for (const auto& sym : p.rhs) {
            if (!non_terminals.count(sym)) terminals.insert(sym);
        }
    }
    terminals.insert("$");
    buildEngine();
}

std::set<SLRParser::LRItem> SLRParser::getClosure(const std::set<LRItem>& inst) {
    std::set<LRItem> closure = inst;
    std::vector<LRItem> queue(inst.begin(), inst.end());
    size_t head = 0;
    while (head < queue.size()) {
        LRItem item = queue[head++];
        const auto& prod = prods_list[item.prod_id];
        if (item.dot < (int)prod.rhs.size()) {
            std::string next_sym = prod.rhs[item.dot];
            if (non_terminals.count(next_sym)) {
                for (int pid : prod_map[next_sym]) {
                    LRItem newItem{pid, 0};
                    if (!closure.count(newItem)) {
                        closure.insert(newItem);
                        queue.push_back(newItem);
                    }
                }
            }
        }
    }
    return closure;
}

std::set<SLRParser::LRItem> SLRParser::getGotoState(const std::set<LRItem>& state, const std::string& sym) {
    std::set<LRItem> next_set;
    for (const auto& item : state) {
        const auto& prod = prods_list[item.prod_id];
        if (item.dot < (int)prod.rhs.size() && prod.rhs[item.dot] == sym) {
            next_set.insert({item.prod_id, item.dot + 1});
        }
    }
    return getClosure(next_set);
}

void SLRParser::buildEngine() {
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& p : prods_list) {
            if (p.id == 0) continue;
            std::set<std::string>& f_set = first_sets[p.lhs];
            size_t old_size = f_set.size();
            if (p.rhs.empty()) {
                f_set.insert("");
            } else {
                bool all_empty = true;
                for (const auto& sym : p.rhs) {
                    if (terminals.count(sym)) {
                        f_set.insert(sym);
                        all_empty = false;
                        break;
                    } else {
                        const auto& next_f = first_sets[sym];
                        for (const auto& s : next_f) {
                            if (!s.empty()) f_set.insert(s);
                        }
                        if (!next_f.count("")) {
                            all_empty = false;
                            break;
                        }
                    }
                }
                if (all_empty) f_set.insert("");
            }
            if (f_set.size() > old_size) changed = true;
        }
    }

    follow_sets["Program"].insert("$");
    changed = true;
    while (changed) {
        changed = false;
        for (const auto& p : prods_list) {
            for (size_t i = 0; i < p.rhs.size(); i++) {
                std::string B = p.rhs[i];
                if (non_terminals.count(B)) {
                    std::set<std::string>& fol_B = follow_sets[B];
                    size_t old_size = fol_B.size();
                    bool all_empty = true;
                    for (size_t j = i + 1; j < p.rhs.size(); j++) {
                        std::string next_sym = p.rhs[j];
                        if (terminals.count(next_sym)) {
                            fol_B.insert(next_sym);
                            all_empty = false;
                            break;
                        } else {
                            const auto& f_next = first_sets[next_sym];
                            for (const auto& s : f_next) {
                                if (!s.empty()) fol_B.insert(s);
                            }
                            if (!f_next.count("")) {
                                all_empty = false;
                                break;
                            }
                        }
                    }
                    if (all_empty) {
                        const auto& fol_A = follow_sets[p.lhs];
                        for (const auto& s : fol_A) fol_B.insert(s);
                    }
                    if (fol_B.size() > old_size) changed = true;
                }
            }
        }
    }

    std::set<LRItem> i0 = getClosure({{0, 0}});
    states.push_back(i0);
    std::vector<std::set<LRItem>> queue = {i0};
    size_t head = 0;
    std::map<std::pair<int, std::string>, int> transition_map;

    while (head < queue.size()) {
        std::set<LRItem> curr_state = queue[head];
        int curr_idx = head++;
        std::set<std::string> syms;
        for (const auto& item : curr_state) {
            const auto& prod = prods_list[item.prod_id];
            if (item.dot < (int)prod.rhs.size()) syms.insert(prod.rhs[item.dot]);
        }
        for (const auto& sym : syms) {
            std::set<LRItem> next_state = getGotoState(curr_state, sym);
            if (next_state.empty()) continue;
            auto it = std::find(states.begin(), states.end(), next_state);
            int next_idx;
            if (it == states.end()) {
                next_idx = states.size();
                states.push_back(next_state);
                queue.push_back(next_state);
            } else {
                next_idx = std::distance(states.begin(), it);
            }
            transition_map[{curr_idx, sym}] = next_idx;
        }
    }

    action_table.resize(states.size());
    goto_table.resize(states.size());
    for (size_t i = 0; i < states.size(); i++) {
        for (const auto& t : terminals) action_table[i][t] = {'e', 0};
        for (const auto& nt : non_terminals) goto_table[i][nt] = -1;
    }

    for (const auto& edge : transition_map) {
        if (terminals.count(edge.first.second)) action_table[edge.first.first][edge.first.second] = {'s', edge.second};
        else goto_table[edge.first.first][edge.first.second] = edge.second;
    }

    for (size_t i = 0; i < states.size(); i++) {
        for (const auto& item : states[i]) {
            const auto& prod = prods_list[item.prod_id];
            if (item.dot == (int)prod.rhs.size()) {
                if (item.prod_id == 0) {
                    action_table[i]["$"] = {'a', 0};
                } else {
                    for (const auto& a : follow_sets[prod.lhs]) {
                        if (action_table[i][a].type != 's') action_table[i][a] = {'r', item.prod_id};
                    }
                }
            }
        }
    }
}

std::string SLRParser::getSymbolName(const Token& t) {
    // Parser文法中用 Ident 表示函数和变量名，强制把 main 的种别映射回 Ident 供语法分析器识别
    if (t.type == IDN || t.value == "main") return "Ident";
    if (t.type == INT_CONST) return "IntConst";
    if (t.type == FLOAT_CONST) return "floatConst";
    return t.value;
}

ParseTree* SLRParser::parse(const std::vector<Token>& tokens) {
    std::stack<int> state_stack;
    std::stack<std::string> sym_stack;
    std::stack<ParseTree*> tree_stack;
    state_stack.push(0);
    size_t pos = 0;
    int step = 1;

    while (true) {
        int s = state_stack.top();
        std::string current_sym = (pos < tokens.size()) ? getSymbolName(tokens[pos]) : "$";
        std::string top_sym = sym_stack.empty() ? "" : sym_stack.top();

        Action act;

        // --- 核心修复：LL(2) 前看机制动态消解 SLR(1) 冲突盲区 ---
        // 彻底根治 bType -> int 和 funcType -> int 在遇到 Ident 时的冲突
        if (top_sym == "int" && current_sym == "Ident") {
            std::string next_sym_peek = (pos + 1 < tokens.size()) ? getSymbolName(tokens[pos+1]) : "";
            if (next_sym_peek == "(") {
                act = {'r', 23}; // 遇到括号，100% 确认是函数 (funcType -> int)
            } else {
                act = {'r', 10}; // 否则一定是变量声明 (bType -> int)
            }
        } else {
            if (!action_table[s].count(current_sym)) {
                std::cout << step << "\t" << top_sym << "#" << current_sym << "\terror\n";
                return nullptr;
            }
            act = action_table[s][current_sym];
        }

        if (act.type == 's') {
            std::cout << step++ << "\t" << top_sym << "#" << current_sym << "\tmove\n";
            state_stack.push(act.val);
            sym_stack.push(current_sym);
            tree_stack.push(new ParseTree(current_sym, (pos < tokens.size() ? tokens[pos].value : "")));
            pos++;
        } 
        else if (act.type == 'r') {
            std::cout << step++ << "\t" << top_sym << "#" << current_sym << "\treduction\n";
            Production p = prods_list[act.val];
            ParseTree* n = new ParseTree(p.lhs);
            int rhs_size = p.rhs.size();
            std::vector<ParseTree*> children_nodes(rhs_size);
            for (int j = rhs_size - 1; j >= 0; j--) {
                state_stack.pop(); sym_stack.pop();
                children_nodes[j] = tree_stack.top(); tree_stack.pop();
            }
            for (auto* child : children_nodes) n->children.push_back(child);
            tree_stack.push(n);
            sym_stack.push(p.lhs);

            int g = goto_table[state_stack.top()][p.lhs];
            if (g == -1) return nullptr;
            state_stack.push(g);
        } 
        else if (act.type == 'a') {
            std::cout << step++ << "\t" << top_sym << "#" << current_sym << "\taccept\n";
            ParseTree* root = new ParseTree("Program");
            if (!tree_stack.empty()) { root->children.push_back(tree_stack.top()); tree_stack.pop(); }
            return root;
        } 
        else {
            std::cout << step << "\t" << top_sym << "#" << current_sym << "\terror\n";
            return nullptr;
        }
    }
    return nullptr;
}
