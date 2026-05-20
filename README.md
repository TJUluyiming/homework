# C-- 编译器前端

## 目录结构

```text
code/
├── inc/              头文件（FSM、lexical、parser、ir_visitor、config）
├── src/              源文件（含 compiler.cpp 主编译入口）
├── ir/               课程中端（include/、src/）
├── test/             测试用例与 grammar.txt
├── output/           各阶段输出
├── Makefile
└── compiler.exe      主编译器（词法 → 语法 → IR）
```

## 编译与运行

在 `code` 目录下（需已将 MinGW 的 `bin` 加入 PATH）：

```bat
make
make test
```

`compiler.exe` 一次完成三阶段，并写出下列文件。

路径在 `inc/config.h` 中配置：

| 配置项 | 默认 | 说明 |
|--------|------|------|
| `LexerConfig::INPUT_SY` | `test/test_wrs.sy` | 源程序 |
| `LexerConfig::TOKENS_OUT` | `output/tokens.out` | 词法 token |
| `LexerConfig::SYMBOLS_OUT` | `output/symbols.out` | 符号表 |
| `ParserConfig::GRAMMAR_FILE` | `test/grammar.txt` | SLR 文法（parser 内读） |
| `ParserConfig::TOKENS_IN` | `output/tokens.out` | 语法分析输入 |
| `ParserConfig::PARSE_OUT` | `output/parse.out` | 规约序列 |
| `CodegenConfig::IR_OUT` | `output/program.ll` | LLVM IR |

修改 `config.h` 后重新 `make` 再运行。

## 模块说明

| 可执行文件 | 说明 |
|------------|------|
| `compiler.exe` | `src/compiler.cpp`：词法 → `tokens.out` / `symbols.out` → SLR + 建树 → `parse.out` → visitor → `program.ll` |

`lexer.cpp`、`parser.cpp` 中的 `main` 默认不编译；若需单独调试：

```bat
make lexer.exe    # g++ … -DBUILD_LEXER_EXE
make parser.exe   # g++ … -DBUILD_PARSER_EXE
```

文法见 `test/grammar.txt`。词法说明见 `lexer_改造说明.md`。
