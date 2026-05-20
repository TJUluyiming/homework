#include "lexical.h"

#include <cctype>
#include <cerrno>
#include <iostream>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

using namespace std;

namespace {

const LexerScanTable& globalScanTable() {
    static LexerScanTable table = buildLexerScanTable();
    return table;
}

bool ensureParentDir(const string& filePath) {
    size_t pos = filePath.find_last_of("/\\");
    if (pos == string::npos) return true;
    string dir = filePath.substr(0, pos);
    if (dir.empty()) return true;
#ifdef _WIN32
    int rc = _mkdir(dir.c_str());
#else
    int rc = mkdir(dir.c_str(), 0755);
#endif
    return (rc == 0 || errno == EEXIST);
}

} // namespace

Lexer::Lexer(const string& src)
    : source(src), pos(0), line(1), col(1), scanTable(globalScanTable()) {
    initTokenMaps();
    for (const auto& kw : kw_map) {
        symbolTable.addKeyword(kw.first);
    }
}

void Lexer::initTokenMaps() {
    kw_map = {{"int", 1}, {"void", 2}, {"return", 3}, {"const", 4},
              {"main", 5}, {"float", 6}, {"if", 7}, {"else", 8}};
    op_map = {{"+", 6}, {"-", 7}, {"*", 8}, {"/", 9}, {"%", 10},
              {"=", 11}, {">", 12}, {"<", 13}, {"==", 14}, {"<=", 15},
              {">=", 16}, {"!=", 17}, {"&&", 18}, {"||", 19}};
    se_map = {{"(", 20}, {")", 21}, {"{", 22}, {"}", 23}, {";", 24}, {",", 25}};
}

string Lexer::toLower(string s) {
    for (char& c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}

int Lexer::getCharClass(char c) {
    if (isalpha(static_cast<unsigned char>(c))) return 0;
    if (isdigit(static_cast<unsigned char>(c))) return 1;
    if (c == '_') return 2;
    if (c == '.') return 3;
    return 4;
}

int Lexer::dfaNextState(int state, char c) const {
    int charClass = getCharClass(c);
    auto it = scanTable.transition.find({state, charClass});
    if (it == scanTable.transition.end()) return -1;
    return it->second;
}

bool Lexer::scanWithDFA(string& lexeme, string& tokenType) {
    int i = pos;
    int currentState = scanTable.startState;
    int lastAcceptPos = -1;
    int lastAcceptState = -1;

    while (i < static_cast<int>(source.length())) {
        int nextState = dfaNextState(currentState, source[i]);
        if (nextState == -1) break;
        currentState = nextState;
        i++;

        if (scanTable.acceptType.count(currentState)) {
            lastAcceptPos = i;
            lastAcceptState = currentState;
        }
    }

    if (lastAcceptPos == -1) return false;

    lexeme = source.substr(pos, static_cast<size_t>(lastAcceptPos - pos));
    tokenType = scanTable.acceptType.at(lastAcceptState);
    return true;
}

void Lexer::analyze(ostream& tokenOut, ostream& symbolOut) {
    while (pos < static_cast<int>(source.length())) {
        char c = source[pos];

        if (isspace(static_cast<unsigned char>(c))) {
            if (c == '\n') {
                line++;
                col = 1;
            } else {
                col++;
            }
            pos++;
            continue;
        }

        int start_col = col;
        string token;

        string dfaLexeme, dfaTokenType;
        if (scanWithDFA(dfaLexeme, dfaTokenType)) {
            token = dfaLexeme;
            pos += static_cast<int>(token.size());
            col += static_cast<int>(token.size());

            if (dfaTokenType == "IDN") {
                string lower_token = toLower(token);
                if (kw_map.count(lower_token)) {
                    tokenOut << token << "\t<KW," << kw_map[lower_token] << ">\n";
                } else {
                    symbolTable.addIdentifier(token, line, start_col);
                    tokenOut << token << "\t<IDN," << token << ">\n";
                }
            } else if (dfaTokenType == "INT") {
                symbolTable.addInteger(token, line, start_col);
                tokenOut << token << "\t<INT," << token << ">\n";
            } else if (dfaTokenType == "FLOAT") {
                symbolTable.addFloat(token, line, start_col);
                tokenOut << token << "\t<FLOAT," << token << ">\n";
            }
            continue;
        }

        token += c;
        char next_c = (pos + 1 < static_cast<int>(source.length())) ? source[pos + 1] : '\0';
        string two_char_op = token + next_c;

        if (op_map.count(two_char_op)) {
            tokenOut << two_char_op << "\t<OP," << op_map[two_char_op] << ">\n";
            pos += 2;
            col += 2;
            continue;
        }

        if (op_map.count(token)) {
            tokenOut << token << "\t<OP," << op_map[token] << ">\n";
            pos++;
            col++;
            continue;
        }

        if (se_map.count(token)) {
            tokenOut << token << "\t<SE," << se_map[token] << ">\n";
            pos++;
            col++;
            continue;
        }

        tokenOut << token << "\t<ERROR," << line << "," << start_col << ">\n";
        pos++;
        col++;
    }

    symbolTable.writeTo(symbolOut);
}

void lexicalAnalysis(const string& source, const string& tokenPath, const string& symbolPath) {
    if (!ensureParentDir(tokenPath) || !ensureParentDir(symbolPath)) {
        cerr << "Error: Cannot create output directory." << endl;
        return;
    }

    ofstream tokenOut(tokenPath);
    ofstream symbolOut(symbolPath);
    if (!tokenOut.is_open() || !symbolOut.is_open()) {
        cerr << "Error: Cannot open output files." << endl;
        return;
    }

    Lexer lexer(source);
    lexer.analyze(tokenOut, symbolOut);

    cout << "Token output written to: " << tokenPath << endl;
    cout << "Symbol table written to: " << symbolPath << endl;
}
