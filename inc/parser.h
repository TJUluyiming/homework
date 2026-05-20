#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <ostream>
#include <set>
#include <stack>
#include <string>
#include <vector>

/**
 * 词法分析输出的一个单词（或从 tokens.out 读入）。
 * grammarSym：映射后的 SLR 文法终结符名（如 Ident、IntConst、+、int）。
 */
struct Token {
    std::string lexeme;     // 词素原文
    std::string kind;       // 种别：KW / OP / SE / IDN / INT / FLOAT
    std::string attr;       // 属性（关键字编号或常量值等）
    std::string grammarSym; // 文法终结符名，供分析表查表
};

/** SLR 规约过程中构建的语法树结点 */
struct ParseTree {
    std::string type;                  // 非终结符：产生式左部；终结符：文法符号名
    std::string text;                  // 终结符词素（非终结符一般为空）
    std::vector<ParseTree*> children;
    ParseTree(std::string t, std::string txt = "") : type(t), text(txt) {}
    ~ParseTree() { for (auto* c : children) delete c; }
};

/**
 * SLR(1) 语法分析器：读 test/grammar.txt 建表，对 token 序列移进/规约并建树。
 */
class SLRParser {
public:
    SLRParser();

    /**
     * 执行语法分析。
     * @return 成功返回 Program 根结点；失败返回 nullptr，轨迹写入 traceOut
     */
    ParseTree* parse(const std::vector<Token>& tokens, std::ostream& traceOut);

private:
    /** 一条产生式：序号、左部、右部符号序列（空右部表示 ε） */
    struct Production {
        int id;
        std::string lhs;
        std::vector<std::string> rhs;
    };

    /** LR(0) 项目：产生式编号 + 圆点位置 */
    struct LRItem {
        int prod_id;
        int dot;
        bool operator<(const LRItem& o) const {
            if (prod_id != o.prod_id) return prod_id < o.prod_id;
            return dot < o.dot;
        }
        bool operator==(const LRItem& o) const {
            return prod_id == o.prod_id && dot == o.dot;
        }
    };

    /** 分析表动作：s=移进(状态号) r=规约(产生式号) a=接受 e=出错 */
    struct Action {
        char type;
        int val;
    };

    std::vector<Production> prods_list;
    std::map<std::string, std::vector<int>> prod_map; // 左部 -> 产生式 id 列表
    std::set<std::string> terminals;
    std::set<std::string> non_terminals;

    std::map<std::string, std::set<std::string>> first_sets;
    std::map<std::string, std::set<std::string>> follow_sets;
    std::vector<std::set<LRItem>> states; // LR(0) 项目集规范族

    std::vector<std::map<std::string, Action>> action_table;
    std::vector<std::map<std::string, int>> goto_table;

    std::set<LRItem> getClosure(const std::set<LRItem>& inst);
    std::set<LRItem> getGotoState(const std::set<LRItem>& state, const std::string& sym);
    void buildEngine();  // FIRST/FOLLOW + 项目集 + SLR 分析表
    void initGrammar();
    bool loadGrammarFromFile(const std::string& path);

    /** 轨迹输出时把 $ 显示为 EOF */
    static std::string formatLookahead(const std::string& grammarSym);
};

/** 确保输出路径的父目录存在（如 output/） */
bool ensureParentDir(const std::string& filePath);

/** 从 tokens.out 加载单词流（每行：词素<TAB><种别,属性>） */
bool loadTokensFromFile(const std::string& path, std::vector<Token>& out);

#endif
