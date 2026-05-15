#ifndef IR_VISITOR_H
#define IR_VISITOR_H

#include "parser.h"
#include <map>

struct Type { std::string name = "i32"; };
struct Value {};
struct ConstantInt { static Value* get(int v) { return new Value(); } };

struct GlobalVariable : public Value {
    static GlobalVariable* create(std::string name, Type t, bool isC, Value* init) {
        std::cout << "生成中间代码: @" << name << " = global " << t.name << " " << "\n";
        return new GlobalVariable();
    }
};

struct SymbolTable {
    std::map<std::string, Value*> vars;
    void put(std::string n, Value* v) { vars[n] = v; }
    std::map<std::string, Value*> variable() { return vars; }
};

class IRVisitor {
private:
    SymbolTable s;
public:
    void visit(ParseTree* node);
};

#endif
