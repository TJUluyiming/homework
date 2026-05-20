#include "config.h"
#include "ir_visitor.h"
#include "lexical.h"
#include "parser.h"

#include <fstream>
#include <iostream>
#include <string>

int main() {
    const std::string source = readSourceFile(LexerConfig::INPUT_SY);
    if (source.empty()) {
        std::cerr << "Error: Cannot read " << LexerConfig::INPUT_SY << std::endl;
        return 1;
    }

    if (!ensureParentDir(LexerConfig::TOKENS_OUT) ||
        !ensureParentDir(LexerConfig::SYMBOLS_OUT) ||
        !ensureParentDir(ParserConfig::PARSE_OUT) ||
        !ensureParentDir(CodegenConfig::IR_OUT)) {
        std::cerr << "Error: Cannot create output directory." << std::endl;
        return 1;
    }

    lexicalAnalysis(source, LexerConfig::TOKENS_OUT, LexerConfig::SYMBOLS_OUT);
    std::cout << "Lexical: " << LexerConfig::TOKENS_OUT << ", "
              << LexerConfig::SYMBOLS_OUT << std::endl;

    std::vector<Token> tokens;
    if (!loadTokensFromFile(ParserConfig::TOKENS_IN, tokens)) {
        std::cerr << "Error: Cannot load " << ParserConfig::TOKENS_IN << std::endl;
        return 1;
    }

    std::ofstream traceOut(ParserConfig::PARSE_OUT);
    if (!traceOut.is_open()) {
        std::cerr << "Error: Cannot open " << ParserConfig::PARSE_OUT << std::endl;
        return 1;
    }

    SLRParser parser;
    ParseTree* root = parser.parse(tokens, traceOut);
    traceOut.close();
    if (!root) {
        std::cerr << "Parse failed. See " << ParserConfig::PARSE_OUT << std::endl;
        return 1;
    }
    std::cout << "Parse trace: " << ParserConfig::PARSE_OUT << std::endl;

    IRVisitor visitor;
    visitor.setSourceFile(LexerConfig::INPUT_SY);
    visitor.visit(root);

    std::ofstream irOut(CodegenConfig::IR_OUT);
    if (!irOut.is_open()) {
        std::cerr << "Error: Cannot open " << CodegenConfig::IR_OUT << std::endl;
        delete root;
        return 1;
    }
    irOut << visitor.output();
    irOut.close();

    delete root;
    std::cout << "IR: " << CodegenConfig::IR_OUT << std::endl;
    return 0;
}
