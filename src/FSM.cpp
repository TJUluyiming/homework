#include "FSM.h"

#include <algorithm>
#include <cctype>
#include <stack>

using namespace std;

//==============================================================================
// 1. 词法 NFA 构造（C--：IDN / INT / FLOAT 合并单图）
//==============================================================================

namespace {

void addLetterEdges(NFA& nfa, int from, int to) {
    for (char c = 'a'; c <= 'z'; ++c) nfa.addEdge(from, to, c);
    for (char c = 'A'; c <= 'Z'; ++c) nfa.addEdge(from, to, c);
    nfa.addEdge(from, to, '_');
}

void addDigitEdges(NFA& nfa, int from, int to) {
    for (char c = '0'; c <= '9'; ++c) nfa.addEdge(from, to, c);
}

/** 与附录 Ident / IntConst / floatConst 语义等价的合并 NFA */
NFA createLexerNFA() {
    NFA nfa;
    const int s0 = nfa.newState();
    const int s1 = nfa.newState(true);  // IDN
    const int s2 = nfa.newState(true);  // INT
    const int s3 = nfa.newState();
    const int s4 = nfa.newState(true);  // FLOAT

    addLetterEdges(nfa, s0, s1);
    addLetterEdges(nfa, s1, s1);
    addDigitEdges(nfa, s1, s1);

    addDigitEdges(nfa, s0, s2);
    addDigitEdges(nfa, s2, s2);
    nfa.addEdge(s2, s3, '.');
    addDigitEdges(nfa, s3, s4);
    addDigitEdges(nfa, s4, s4);

    nfa.stateAcceptKind[s1] = "IDN";
    nfa.stateAcceptKind[s2] = "INT";
    nfa.stateAcceptKind[s4] = "FLOAT";
    nfa.states[s1].isEnd = true;
    nfa.states[s2].isEnd = true;
    nfa.states[s4].isEnd = true;
    return nfa;
}

/** DFA 状态中多种接受类型并存时的优先级（最长匹配后的归类） */
string pickAcceptKind(const set<string>& kinds) {
    if (kinds.count("FLOAT")) return "FLOAT";
    if (kinds.count("INT")) return "INT";
    if (kinds.count("IDN")) return "IDN";
    return "";
}

/** 扫描表字符类：0=字母, 1=数字, 2='_', 3='.' */
int getCharClass(char c) {
    if (isalpha(static_cast<unsigned char>(c))) return 0;
    if (isdigit(static_cast<unsigned char>(c))) return 1;
    if (c == '_') return 2;
    if (c == '.') return 3;
    return 4;
}

} // namespace

//==============================================================================
// 2. NFA 基础实现
//==============================================================================

NFA::NFA() : stateCounter(0) {}

int NFA::newState(bool isEnd) {
    states.push_back(State(stateCounter, isEnd));
    return stateCounter++;
}

void NFA::addEdge(int from, int to, char label) {
    edges.push_back(Edge(from, to, label));
}

set<int> NFA::epsilonClosure(set<int> states) const {
    set<int> closure = states;
    stack<int> st;
    for (int s : states) st.push(s);

    while (!st.empty()) {
        int state = st.top();
        st.pop();
        for (const Edge& e : edges) {
            if (e.from == state && e.label == 'E' && closure.find(e.to) == closure.end()) {
                closure.insert(e.to);
                st.push(e.to);
            }
        }
    }
    return closure;
}

set<int> NFA::move(const set<int>& states, char input) const {
    set<int> result;
    for (int state : states) {
        for (const Edge& e : edges) {
            if (e.from == state && e.label == input) {
                result.insert(e.to);
            }
        }
    }
    return result;
}

//==============================================================================
// 3. NFA → DFA（子集构造）
//==============================================================================

void DFA::fromNFA(const NFA& nfa) {
    states.clear();
    edges.clear();
    acceptStates.clear();
    transition.clear();
    stateAcceptKind.clear();

    if (nfa.states.empty()) return;

    set<set<int>> Dstates;
    map<set<int>, int> stateMap;
    queue<set<int>> workList;

    set<int> startSet = nfa.epsilonClosure({nfa.states[0].id});
    Dstates.insert(startSet);
    workList.push(startSet);
    stateMap[startSet] = 0;
    states.push_back(State(0, false));
    startState = 0;

    vector<char> alphabet;
    for (const Edge& e : nfa.edges) {
        if (e.label != 'E') {
            bool found = false;
            for (char c : alphabet) {
                if (c == e.label) {
                    found = true;
                    break;
                }
            }
            if (!found) alphabet.push_back(e.label);
        }
    }

    while (!workList.empty()) {
        set<int> T = workList.front();
        workList.pop();
        int T_id = stateMap[T];

        set<string> kindsInState;
        for (int s : T) {
            auto it = nfa.stateAcceptKind.find(s);
            if (it != nfa.stateAcceptKind.end()) kindsInState.insert(it->second);
        }
        string kind = pickAcceptKind(kindsInState);
        if (!kind.empty()) {
            states[T_id].isEnd = true;
            acceptStates.push_back(T_id);
            stateAcceptKind[T_id] = kind;
        }

        for (char a : alphabet) {
            set<int> U = nfa.epsilonClosure(nfa.move(T, a));
            if (U.empty()) continue;

            if (Dstates.find(U) == Dstates.end()) {
                Dstates.insert(U);
                int newId = static_cast<int>(states.size());
                stateMap[U] = newId;
                states.push_back(State(newId, false));
                workList.push(U);
            }

            int U_id = stateMap[U];
            transition[{T_id, a}] = U_id;
            edges.push_back(Edge(T_id, U_id, a));
        }
    }

    sort(acceptStates.begin(), acceptStates.end());
    acceptStates.erase(unique(acceptStates.begin(), acceptStates.end()), acceptStates.end());
}

//==============================================================================
// 4. DFA 最小化（按接受类型初划分）
//==============================================================================

void DFA::minimize() {
    if (states.empty()) return;

    map<string, set<int>> initialGroups;
    for (const State& s : states) {
        string key = "NON_ACCEPT";
        auto it = stateAcceptKind.find(s.id);
        if (it != stateAcceptKind.end()) {
            key = "ACCEPT_" + it->second;
        }
        initialGroups[key].insert(s.id);
    }

    vector<set<int>> partitions;
    for (const auto& g : initialGroups) {
        partitions.push_back(g.second);
    }

    set<char> alphabet;
    for (const Edge& e : edges) {
        alphabet.insert(e.label);
    }

    auto findPartitionIndex = [&](const vector<set<int>>& parts, int state) {
        for (size_t i = 0; i < parts.size(); ++i) {
            if (parts[i].count(state)) return static_cast<int>(i);
        }
        return -1;
    };

    bool changed = true;
    while (changed) {
        changed = false;
        vector<set<int>> newPartitions;

        for (const auto& part : partitions) {
            if (part.size() <= 1) {
                newPartitions.push_back(part);
                continue;
            }

            map<string, set<int>> signatureGroups;
            for (int state : part) {
                string signature;
                for (char c : alphabet) {
                    int target = -1;
                    auto it = transition.find({state, c});
                    if (it != transition.end()) target = it->second;
                    signature += to_string(findPartitionIndex(partitions, target)) + ",";
                }
                signatureGroups[signature].insert(state);
            }

            if (signatureGroups.size() > 1) changed = true;
            for (const auto& g : signatureGroups) {
                newPartitions.push_back(g.second);
            }
        }
        partitions = newPartitions;
    }

    map<int, int> oldToNew;
    for (size_t i = 0; i < partitions.size(); ++i) {
        for (int oldState : partitions[i]) {
            oldToNew[oldState] = static_cast<int>(i);
        }
    }

    vector<State> newStates;
    vector<Edge> newEdges;
    vector<int> newAcceptStates;
    map<pair<int, char>, int> newTransition;
    map<int, string> newAcceptKind;

    for (size_t i = 0; i < partitions.size(); ++i) {
        newStates.push_back(State(static_cast<int>(i), false));
    }

    set<pair<int, pair<char, int>>> seen;
    for (const Edge& e : edges) {
        if (!oldToNew.count(e.from) || !oldToNew.count(e.to)) continue;
        int nf = oldToNew[e.from];
        int nt = oldToNew[e.to];
        if (seen.count({nf, {e.label, nt}})) continue;
        seen.insert({nf, {e.label, nt}});
        newEdges.push_back(Edge(nf, nt, e.label));
        newTransition[{nf, e.label}] = nt;
    }

    for (int oldAccept : acceptStates) {
        if (!oldToNew.count(oldAccept)) continue;
        int na = oldToNew[oldAccept];
        if (find(newAcceptStates.begin(), newAcceptStates.end(), na) == newAcceptStates.end()) {
            newAcceptStates.push_back(na);
            newStates[na].isEnd = true;
            if (stateAcceptKind.count(oldAccept)) {
                newAcceptKind[na] = stateAcceptKind[oldAccept];
            }
        }
    }

    startState = oldToNew[startState];
    states = newStates;
    edges = newEdges;
    acceptStates = newAcceptStates;
    transition = newTransition;
    stateAcceptKind = newAcceptKind;
}

//==============================================================================
// 5. DFA → 扫描表 & 对外接口
//==============================================================================

namespace {

LexerScanTable exportToScanTable(const DFA& dfa) {
    LexerScanTable table;
    table.startState = dfa.startState;

    for (const auto& kv : dfa.stateAcceptKind) {
        table.acceptType[kv.first] = kv.second;
    }

    for (const Edge& e : dfa.edges) {
        int cls = getCharClass(e.label);
        if (cls > 3) continue;
        auto key = make_pair(e.from, cls);
        if (!table.transition.count(key)) {
            table.transition[key] = e.to;
        }
    }

    return table;
}

} // namespace

LexerScanTable buildLexerScanTable() {
    NFA nfa = createLexerNFA();
    DFA dfa;
    dfa.fromNFA(nfa);
    dfa.minimize();
    return exportToScanTable(dfa);
}
