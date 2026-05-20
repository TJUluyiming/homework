#ifndef LEXER_CONFIG_H
#define LEXER_CONFIG_H

/** 词法分析器路径配置（修改此处即可切换输入/输出） */
namespace LexerConfig {

inline const char* INPUT_SY = "test/test_wrs.sy";
inline const char* TOKENS_OUT = "output/tokens.out";
inline const char* SYMBOLS_OUT = "output/symbols.out";

} // namespace LexerConfig

/** 语法分析器路径（输入为词法输出 tokens.out） */
namespace ParserConfig {

inline const char* GRAMMAR_FILE = "test/grammar.txt";
inline const char* TOKENS_IN = "output/tokens.out";
inline const char* PARSE_OUT = "output/parse.out";

} // namespace ParserConfig

/** 中间代码生成输出路径 */
namespace CodegenConfig {

inline const char* IR_OUT = "output/program.ll";

} // namespace CodegenConfig

#endif
