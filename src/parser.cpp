#include "parser.h"
#include "config.h"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

//==============================================================================
// 工具：输出目录、tokens.out 解析、文法文件读取辅助
//==============================================================================

bool ensureParentDir(const std::string& filePath) {
    const size_t pos = filePath.find_last_of("/\\");
    if (pos == std::string::npos) return true;
    const std::string dir = filePath.substr(0, pos);
    if (dir.empty()) return true;
#ifdef _WIN32
    const int rc = _mkdir(dir.c_str());
#else
    const int rc = mkdir(dir.c_str(), 0755);
#endif
    return (rc == 0 || errno == EEXIST);
}

namespace {

/**
 * 将词法种别映射为 SLR 文法终结符名。
 * main 关键字(attr=5) 映射为 Ident，以便按函数名参与 funcDef 归约。
 */
std::string mapToGrammarSym(const std::string& kind, const std::string& attr,
                            const std::string& lexeme) {
    if (kind == "KW") {
        if (attr == "5") return "Ident";
        return lexeme;
    }
    if (kind == "IDN") return "Ident";
    if (kind == "INT") return "IntConst";
    if (kind == "FLOAT") return "floatConst";
    if (kind == "OP" || kind == "SE") return lexeme;
    return lexeme;
}

/** 解析 tokens.out 一行：词素<TAB><种别,属性> */
bool parseTokenLine(const std::string& line, Token& tok) {
    if (line.empty()) return false;
    const size_t tab = line.find('\t');
    if (tab == std::string::npos) return false;

    tok.lexeme = line.substr(0, tab);
    const size_t lt = line.find('<', tab);
    const size_t gt = line.find('>', tab);
    if (lt == std::string::npos || gt == std::string::npos || gt <= lt + 1) return false;

    const std::string inside = line.substr(lt + 1, gt - lt - 1);
    const size_t comma = inside.find(',');
    if (comma == std::string::npos) return false;

    tok.kind = inside.substr(0, comma);
    tok.attr = inside.substr(comma + 1);
    tok.grammarSym = mapToGrammarSym(tok.kind, tok.attr, tok.lexeme);
    return true;
}

std::string trim(const std::string& s) {
    size_t b = 0;
    while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    size_t e = s.size();
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

std::string stripQuotes(std::string sym) {
    if (sym.size() >= 2 && sym.front() == '\'' && sym.back() == '\'') {
        return sym.substr(1, sym.size() - 2);
    }
    return sym;
}

std::vector<std::string> splitRhs(const std::string& rhs) {
    std::vector<std::string> syms;
    std::istringstream iss(rhs);
    std::string tok;
    while (iss >> tok) {
        syms.push_back(stripQuotes(tok));
    }
    return syms;
}

} // namespace

//==============================================================================
// 对外接口：加载 token 流
//==============================================================================

bool loadTokensFromFile(const std::string& path, std::vector<Token>& out) {
    std::ifstream in(path);
    if (!in.is_open()) return false;

    out.clear();
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        Token tok;
        if (!parseTokenLine(line, tok)) continue;
        out.push_back(tok);
    }
    return !out.empty();
}

//==============================================================================
// 文法加载与初始化
//==============================================================================

SLRParser::SLRParser() {
    initGrammar();
}

/**
 * 从 grammar.txt 读取产生式。
 * 格式：序号. 左部 -> 右部符号…  或  # 注释；空右部表示 ε
 */
bool SLRParser::loadGrammarFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return false;

    prods_list.clear();
    std::string line;
    int fallback_id = 0;

    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        const size_t arrow = line.find("->");
        if (arrow == std::string::npos) continue;

        std::string leftPart = trim(line.substr(0, arrow));
        const std::string rightPart = trim(line.substr(arrow + 2));
        if (leftPart.empty()) continue;

        int prod_id = fallback_id;
        size_t i = 0;
        while (i < leftPart.size() && std::isspace(static_cast<unsigned char>(leftPart[i]))) {
            ++i;
        }
        const size_t numStart = i;
        while (i < leftPart.size() && std::isdigit(static_cast<unsigned char>(leftPart[i]))) {
            ++i;
        }
        if (i > numStart) {
            prod_id = std::stoi(leftPart.substr(numStart, i - numStart));
            if (i < leftPart.size() && leftPart[i] == '.') ++i;
            while (i < leftPart.size() &&
                   std::isspace(static_cast<unsigned char>(leftPart[i]))) {
                ++i;
            }
            leftPart = trim(leftPart.substr(i));
        } else {
            prod_id = fallback_id++;
        }

        Production p;
        p.id = prod_id;
        p.lhs = stripQuotes(leftPart);
        p.rhs = splitRhs(rightPart);
        prods_list.push_back(p);

        if (i > numStart) {
            fallback_id = prod_id + 1;
        }
    }

    return !prods_list.empty();
}

/** 装入文法、划分终结符/非终结符、构造 SLR 分析表 */
void SLRParser::initGrammar() {
    if (!loadGrammarFromFile(ParserConfig::GRAMMAR_FILE)) {
        std::cerr << "Error: Cannot load grammar file: " << ParserConfig::GRAMMAR_FILE
                  << std::endl;
        std::exit(1);
    }

    non_terminals.clear();
    prod_map.clear();
    terminals.clear();
    first_sets.clear();
    follow_sets.clear();

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

std::string SLRParser::formatLookahead(const std::string& grammarSym) {
    return (grammarSym == "$") ? "EOF" : grammarSym;
}

//==============================================================================
// LR(0) 闭包与 GOTO
//==============================================================================

/** 项目集闭包：圆点后为非终结符时，将该非终结符所有产生式的项目加入 */
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

/** GOTO(I, X)：将 I 中所有「期待 X」的项目点后移，再求闭包 */
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

//==============================================================================
// SLR 分析表构造：FIRST → FOLLOW → 项目集族 → ACTION/GOTO
//==============================================================================

void SLRParser::buildEngine() {
    // --- 1. 计算 FIRST 集（空串用 "" 表示 ε）---
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

    // --- 2. 计算 FOLLOW 集 ---
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

    // --- 3. 构造 LR(0) 项目集规范族（从增广文法项目 Program' -> ·Program 开始）---
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

    // --- 4. 填 ACTION / GOTO：移进、GOTO、规约、接受 ---
    action_table.resize(states.size());
    goto_table.resize(states.size());
    for (size_t i = 0; i < states.size(); i++) {
        for (const auto& t : terminals) action_table[i][t] = {'e', 0};
        for (const auto& nt : non_terminals) goto_table[i][nt] = -1;
    }

    for (const auto& edge : transition_map) {
        if (terminals.count(edge.first.second))
            action_table[edge.first.first][edge.first.second] = {'s', edge.second};
        else
            goto_table[edge.first.first][edge.first.second] = edge.second;
    }

    for (size_t i = 0; i < states.size(); i++) {
        for (const auto& item : states[i]) {
            const auto& prod = prods_list[item.prod_id];
            if (item.dot == (int)prod.rhs.size()) {
                if (item.prod_id == 0) {
                    action_table[i]["$"] = {'a', 0};
                } else {
                    for (const auto& a : follow_sets[prod.lhs]) {
                        if (action_table[i][a].type != 's')
                            action_table[i][a] = {'r', item.prod_id};
                    }
                }
            }
        }
    }
}

//==============================================================================
// 语法分析驱动：移进 / 规约 / 接受，同步构建 ParseTree
//==============================================================================

ParseTree* SLRParser::parse(const std::vector<Token>& tokens, std::ostream& traceOut) {
    std::stack<int> state_stack;
    std::stack<std::string> sym_stack;
    std::stack<ParseTree*> tree_stack;
    state_stack.push(0);
    size_t pos = 0;
    int step = 1;

    while (true) {
        int s = state_stack.top();
        const std::string current_sym =
            (pos < tokens.size()) ? tokens[pos].grammarSym : "$";
        const std::string display_lookahead = formatLookahead(current_sym);

        Action act;

        // int/Ident 冲突：向前多看一个符号，区分 funcType->int 与 bType->int
        const std::string top_sym = sym_stack.empty() ? "" : sym_stack.top();
        if (top_sym == "int" && current_sym == "Ident") {
            const std::string next_peek =
                (pos + 1 < tokens.size()) ? tokens[pos + 1].grammarSym : "";
            if (next_peek == "(") {
                act = {'r', 23};  // funcType -> int
            } else {
                act = {'r', 10};  // bType -> int
            }
        } else {
            if (!action_table[s].count(current_sym)) {
                traceOut << step << "\t" << top_sym << "#" << display_lookahead
                         << "\terror\n";
                return nullptr;
            }
            act = action_table[s][current_sym];
        }

        if (act.type == 's') {  // 移进
            traceOut << step++ << "\t" << current_sym << "#" << display_lookahead
                     << "\tmove\n";
            state_stack.push(act.val);
            sym_stack.push(current_sym);
            tree_stack.push(new ParseTree(
                current_sym, (pos < tokens.size() ? tokens[pos].lexeme : "")));
            pos++;
        } else if (act.type == 'r') {  // 规约：弹栈并建树
            Production p = prods_list[act.val];
            traceOut << step++ << "\t" << p.lhs << "#" << display_lookahead
                     << "\treduction\n";
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
        } else if (act.type == 'a') {  // 接受
            traceOut << step++ << "\tProgram#EOF\taccept\n";
            ParseTree* root = new ParseTree("Program");
            if (!tree_stack.empty()) {
                root->children.push_back(tree_stack.top());
                tree_stack.pop();
            }
            return root;
        } else {
            traceOut << step << "\t" << top_sym << "#" << display_lookahead
                     << "\terror\n";
            return nullptr;
        }
    }
    return nullptr;
}

//==============================================================================
// 可选独立入口：仅语法分析（-DBUILD_PARSER_EXE）
//==============================================================================

#ifdef BUILD_PARSER_EXE
int main() {
    std::vector<Token> tokens;
    if (!loadTokensFromFile(ParserConfig::TOKENS_IN, tokens)) {
        std::cerr << "Error: Cannot load tokens: " << ParserConfig::TOKENS_IN << std::endl;
        std::cerr << "Run lexer.exe first." << std::endl;
        return 1;
    }

    if (!ensureParentDir(ParserConfig::PARSE_OUT)) {
        std::cerr << "Error: Cannot create output directory." << std::endl;
        return 1;
    }

    std::ofstream traceOut(ParserConfig::PARSE_OUT);
    if (!traceOut.is_open()) {
        std::cerr << "Error: Cannot open " << ParserConfig::PARSE_OUT << std::endl;
        return 1;
    }

    SLRParser parser;
    ParseTree* root = parser.parse(tokens, traceOut);
    if (!root) {
        std::cerr << "Parse failed." << std::endl;
        return 1;
    }
    delete root;

    std::cout << "Parse trace written to: " << ParserConfig::PARSE_OUT << std::endl;
    return 0;
}
#endif