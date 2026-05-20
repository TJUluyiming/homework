#ifndef LEXER_FSM_H
#define LEXER_FSM_H

#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

struct State {
    int id;
    bool isEnd;
    State(int i, bool e = false) : id(i), isEnd(e) {}
};

struct Edge {
    int from;
    int to;
    char label;
    Edge(int f, int t, char l) : from(f), to(t), label(l) {}
};

/** NFA：词法合并图 + 子集构造所需操作 */
class NFA {
public:
    std::vector<State> states;
    std::vector<Edge> edges;
    int stateCounter;
    std::map<int, std::string> stateAcceptKind;

    NFA();

    int newState(bool isEnd = false);
    void addEdge(int from, int to, char label);

    std::set<int> epsilonClosure(std::set<int> states) const;
    std::set<int> move(const std::set<int>& states, char input) const;
};

/** DFA：子集构造 + 最小化 */
class DFA {
public:
    std::vector<State> states;
    std::vector<Edge> edges;
    int startState;
    std::vector<int> acceptStates;
    std::map<std::pair<int, char>, int> transition;
    std::map<int, std::string> stateAcceptKind;

    void fromNFA(const NFA& nfa);
    void minimize();
};

/** 最小化 DFA 的字符类扫描表（供 lexical 最长匹配） */
struct LexerScanTable {
    int startState;
    std::map<std::pair<int, int>, int> transition;
    std::map<int, std::string> acceptType;
};

/** 词法自动机管线：合并 NFA → DFA → 最小化 → 扫描表 */
LexerScanTable buildLexerScanTable();

#endif
