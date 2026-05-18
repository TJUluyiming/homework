// ir_visitor.h
#ifndef IR_VISITOR_H
#define IR_VISITOR_H

#include "parser.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>

struct Type {
    std::string name;
    Type(std::string n = "i32") {
        if (n == "int") name = "i32"; // 拦截 C-- 的 int 并转换为 LLVM 的 i32
        else name = n;
    }
    bool isInt() const { return name == "i32"; }
    bool isFloat() const { return name == "float"; }
    bool isVoid() const { return name == "void"; }
};

struct Value {
    virtual ~Value() = default;
    virtual std::string getName() const { return ""; }
    virtual bool isConstant() const { return false; }
    virtual int getIntValue() const { return 0; }
    virtual float getFloatValue() const { return 0.0f; }
    virtual Type getType() const { return Type("i32"); }
};

struct ConstantInt : public Value {
    int v;
    ConstantInt(int _v) : v(_v) {}
    static Value* get(int v) { return new ConstantInt(v); }
    std::string getName() const override { return std::to_string(v); }
    bool isConstant() const override { return true; }
    int getIntValue() const override { return v; }
    Type getType() const override { return Type("i32"); }
};

struct ConstantFloat : public Value {
    float v;
    ConstantFloat(float _v) : v(_v) {}
    static Value* get(float v) { return new ConstantFloat(v); }
    std::string getName() const override { return std::to_string(v); }
    bool isConstant() const override { return true; }
    float getFloatValue() const override { return v; }
    Type getType() const override { return Type("float"); }
};

struct GlobalVariable : public Value {
    std::string name; Type type; bool is_const; Value* init;
    GlobalVariable(std::string n, Type t, bool c, Value* i) : name(n), type(t), is_const(c), init(i) {
        std::cout << "@" << name << " = global " << type.name << " " << (init ? init->getName() : "0") << "\n";
    }
    static Value* create(std::string n, Type t, bool c, Value* i) { return new GlobalVariable(n, t, c, i); }
    std::string getName() const override { return "@" + name; }
    Type getType() const override { return type; }
};

struct AllocaInst : public Value {
    std::string name; Type type;
    AllocaInst(std::string n, Type t) : name(n), type(t) { std::cout << "  %" << name << " = alloca " << type.name << "\n"; }
    std::string getName() const override { return "%" + name; }
    Type getType() const override { return type; }
};

struct StoreInst : public Value {
    StoreInst(Value* val, Value* ptr) {
        std::cout << "  store " << val->getType().name << " " << val->getName() << ", " << ptr->getType().name << "* " << ptr->getName() << "\n";
    }
};

struct LoadInst : public Value {
    std::string name; Type type;
    LoadInst(std::string n, Value* ptr) : name(n), type(ptr->getType()) {
        std::cout << "  %" << name << " = load " << type.name << ", " << type.name << "* " << ptr->getName() << "\n";
    }
    std::string getName() const override { return "%" + name; }
    Type getType() const override { return type; }
};

struct BinaryInst : public Value {
    std::string name; Type type;
    BinaryInst(std::string n, std::string op, Value* lhs, Value* rhs) : name(n), type(lhs->getType()) {
        std::cout << "  %" << name << " = " << op << " " << type.name << " " << lhs->getName() << ", " << rhs->getName() << "\n";
    }
    std::string getName() const override { return "%" + name; }
    Type getType() const override { return type; }
};

struct BranchInst : public Value {
    BranchInst(std::string label) { std::cout << "  br label %" << label << "\n"; }
    BranchInst(Value* cond, std::string t_lbl, std::string f_lbl) {
        std::cout << "  br i1 " << cond->getName() << ", label %" << t_lbl << ", label %" << f_lbl << "\n";
    }
};

struct LabelInst : public Value { LabelInst(std::string lbl) { std::cout << lbl << ":\n"; } };

struct ReturnInst : public Value {
    ReturnInst(Value* v = nullptr) {
        if (v) std::cout << "  ret " << v->getType().name << " " << v->getName() << "\n";
        else std::cout << "  ret void\n";
    }
};

struct FunctionValue : public Value {
    std::string name; Type ret_type;
    FunctionValue(std::string n, Type t) : name(n), ret_type(t) {}
    std::string getName() const override { return "@" + name; }
    Type getType() const override { return ret_type; }
};

struct CallInst : public Value {
    std::string name; Type return_type;
    CallInst(std::string n, Value* func, const std::vector<Value*>& args = {}) : name(n), return_type(func->getType()) {
        std::cout << "  %" << name << " = call " << return_type.name << " " << func->getName() << "(";
        for (size_t i = 0; i < args.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << args[i]->getType().name << " " << args[i]->getName();
        }
        std::cout << ")\n";
    }
    std::string getName() const override { return "%" + name; }
    Type getType() const override { return return_type; }
};

class IRVisitor {
private:
    std::vector<std::map<std::string, Value*>> symbol_scopes;
    int temp_counter = 0;
    int label_counter = 0;
    std::string newTemp() { return "tmp" + std::to_string(temp_counter++); }
    std::string newLabel() { return "label" + std::to_string(label_counter++); }
    
    Value* visitExp(ParseTree* node);
    Value* visitCond(ParseTree* node);
    Value* visitLVal(ParseTree* node);
    Value* visitPrimaryExp(ParseTree* node);
    Value* visitUnaryExp(ParseTree* node);
    Value* visitMulExp(ParseTree* node);
    Value* visitAddExp(ParseTree* node);
    Value* visitRelExp(ParseTree* node);
    Value* visitEqExp(ParseTree* node);
    Value* visitLAndExp(ParseTree* node);
    Value* visitLOrExp(ParseTree* node);
    
    void extractVarDefs(ParseTree* node, std::vector<ParseTree*>& defs);
    void extractConstDefs(ParseTree* node, std::vector<ParseTree*>& defs);
    void extractFuncFParams(ParseTree* node, std::vector<ParseTree*>& params);
    void extractFuncRParams(ParseTree* node, std::vector<ParseTree*>& params); // 新增

public:
    IRVisitor(); // <--- 新增构造函数声明
    void visit(ParseTree* node);
};

#endif
