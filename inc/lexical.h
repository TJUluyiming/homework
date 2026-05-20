#ifndef LEXER_LEXICAL_H
#define LEXER_LEXICAL_H

#include "FSM.h"
#include "SymbolTable.h"

#include <fstream>
#include <map>
#include <ostream>
#include <string>

class Lexer {
public:
    explicit Lexer(const std::string& source);

    void analyze(std::ostream& tokenOut, std::ostream& symbolOut);

private:
    std::string source;
    int pos;
    int line;
    int col;

    LexerScanTable scanTable;
    SymbolTable symbolTable;

    std::map<std::string, int> kw_map;
    std::map<std::string, int> op_map;
    std::map<std::string, int> se_map;

    void initTokenMaps();
    static std::string toLower(std::string s);
    static int getCharClass(char c);

    int dfaNextState(int state, char c) const;
    bool scanWithDFA(std::string& lexeme, std::string& tokenType);
};

void lexicalAnalysis(const std::string& source,
                     const std::string& tokenPath,
                     const std::string& symbolPath);

/** 读取 .sy 源文件全文 */
std::string readSourceFile(const std::string& path);

#endif
