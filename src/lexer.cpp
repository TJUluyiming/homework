#include "config.h"
#include "lexical.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

string readSourceFile(const string& path) {
    ifstream in(path);
    if (!in.is_open()) {
        cerr << "Error: Cannot open input file: " << path << endl;
        return "";
    }
    string source;
    string line;
    while (getline(in, line)) {
        source += line;
        source += '\n';
    }
    return source;
}

#ifdef BUILD_LEXER_EXE
int main() {
    const string source = readSourceFile(LexerConfig::INPUT_SY);
    if (source.empty()) {
        cerr << "Error: No input (check config: " << LexerConfig::INPUT_SY << ")" << endl;
        return 1;
    }

    lexicalAnalysis(source, LexerConfig::TOKENS_OUT, LexerConfig::SYMBOLS_OUT);
    return 0;
}
#endif
