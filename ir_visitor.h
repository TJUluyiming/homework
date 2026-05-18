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
    Type(std::string n = "i32") : name(n) {}
};

struct Value {
    virtual ~Value() = default;
    virtual std::string getName() const { return ""; }
    virtual bool isConstant() const { return false; }
    virtual int getConstantValue() const { return 0; }
};

struct ConstantInt : public Value {
    int v;
    ConstantInt(int _v) : v(_v) {}
    static Value* get(int v) { return new ConstantInt(v); }
    std::string getName() const override { return std::to_string(v); }
    bool isConstant() const override { return true; }
    int getConstantValue() const override { return v; }
};

struct GlobalVariable : public Value {
    std::string name;
    GlobalVariable(std::string n, Type t, bool c, Value* i) : name(n) {
        int val = i ? static_cast<ConstantInt*>(i)->v : 0;
        std::cout << "@" << n << " = dso_local global " << t.name << " " << val << ", align 4\n";
    }
    static Value* create(std::string n, Type t, bool c, Value* i) {
        return new GlobalVariable(n, t, c, i);
    }
    std::string getName() const override { return "@" + name; }
};

struct AllocaInst : public Value {
    std::string name;
    AllocaInst(std::string n, Type t) : name(n) {
        std::cout << "  %" << name << " = alloca " << t.name << ", align 4\n";
    }
    std::string getName() const override { return "%" + name; }
};

struct LoadInst : public Value {
    std::string name;
    LoadInst(std::string n, Value* ptr) : name(n) {
        std::cout << "  %" << name << " = load i32, i32* " << ptr->getName() << ", align 4\n";
    }
    std::string getName() const override { return "%" + name; }
};

struct StoreInst : public Value {
    StoreInst(Value* val, Value* ptr) {
        std::cout << "  store i32 " << val->getName() << ", i32* " << ptr->getName() << ", align 4\n";
    }
};

struct BinaryOpInst : public Value {
    std::string name;
    BinaryOpInst(std::string n, std::string op, Value* lhs, Value* rhs) : name(n) {
        std::cout << "  %" << name << " = " << op << " i32 " << lhs->getName() << ", " << rhs->getName() << "\n";
    }
    std::string getName() const override { return "%" + name; }
};

struct ICmpInst : public Value {
    std::string name;
    ICmpInst(std::string n, std::string pred, Value* lhs, Value* rhs) : name(n) {
        std::cout << "  %" << name << " = icmp " << pred << " i32 " << lhs->getName() << ", " << rhs->getName() << "\n";
    }
    std::string getName() const override { return "%" + name; }
};

struct BranchInst : public Value {
    BranchInst(std::string dest) {
        std::cout << "  br label %" << dest << "\n";
    }
    BranchInst(Value* cond, std::string true_dest, std::string false_dest) {
        std::cout << "  br i1 " << cond->getName() << ", label %" << true_dest << ", label %" << false_dest << "\n";
    }
};

struct LabelInst : public Value {
    LabelInst(std::string name) {
        std::cout << "\n" << name << ":\n";
    }
};

struct ReturnInst : public Value {
    ReturnInst(Value* val) {
        std::cout << "  ret i32 " << val->getName() << "\n";
    }
    ReturnInst() {
        std::cout << "  ret void\n";
    }
};

struct Function : public Value {
    std::string name;
    Function(std::string n, Type t, const std::vector<std::string>& params = {}) : name(n) {
        std::cout << "\ndefine " << t.name << " @" << n << "(";
        for (size_t i = 0; i < params.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << "i32 %" << params[i];
        }
        std::cout << ") {\n";
    }
    static Value* create(std::string n, Type t, const std::vector<std::string>& params = {}) {
        return new Function(n, t, params);
    }
    std::string getName() const override { return "@" + name; }
};

class IRVisitor {
private:
    std::vector<std::map<std::string, Value*>> symbol_scopes; // 支持作用域的符号表
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

public:
    void visit(ParseTree* node);
};

#endif
