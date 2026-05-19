// ir_visitor.h
#ifndef IR_VISITOR_H
#define IR_VISITOR_H

#include "parser.h"
#include "Module.h"
#include "IRbuilder.h"
#include "Constant.h"
#include "Function.h"
#include "BasicBlock.h"
#include "GlobalVariable.h"
#include "Instruction.h"
#include <vector>
#include <map>
#include <string>

class IRVisitor {
private:
    Module *module_;
    IRBuilder *builder_;
    Function *cur_func_;
    std::vector<std::map<std::string, Value*>> symbol_scopes;
    int temp_counter = 0;
    int label_counter = 0;

    std::string newTemp() { return "tmp" + std::to_string(temp_counter++); }
    std::string newLabel() { return "L" + std::to_string(label_counter++); }

    // --- helpers ---
    BasicBlock* createBB(const std::string &name = "");
    Value* lookupSymbol(const std::string &name);
    Type* getTypeFromBType(ParseTree* btype);
    void declareBuiltins();

    // --- accumulation members (used by visitFuncFParam / visitFuncRParam) ---
    std::vector<Type*> func_param_types_;
    std::vector<std::string> func_param_names_;
    std::vector<Value*> func_call_args_;

    // === structural visitors (void return 鈥?process side effects) ===
    void visitProgram(ParseTree* node);
    void visitCompUnit(ParseTree* node);
    void visitDecl(ParseTree* node);
    void visitConstDecl(ParseTree* node);
    void visitConstDefs(ParseTree* node, Type* baseType);
    void visitConstDef(ParseTree* node, Type* baseType);
    void visitVarDecl(ParseTree* node);
    void visitVarDefs(ParseTree* node, Type* baseType);
    void visitVarDef(ParseTree* node, Type* baseType);
    void visitFuncDef(ParseTree* node);
    void visitFuncFParams(ParseTree* node);
    void visitFuncFParam(ParseTree* node);
    void visitFuncRParams(ParseTree* node);
    void visitFuncRParam(ParseTree* node);
    void visitBlock(ParseTree* node);
    void visitBlockItems(ParseTree* node);
    void visitBlockItem(ParseTree* node);
    void visitStmt(ParseTree* node);

    // === value-producing visitors (return Value*) ===
    Value* visitExp(ParseTree* node);
    Value* visitCond(ParseTree* node);
    Value* visitLVal(ParseTree* node);
    Value* visitInitVal(ParseTree* node);
    Value* visitConstInitVal(ParseTree* node);
    Value* visitPrimaryExp(ParseTree* node);
    Value* visitUnaryExp(ParseTree* node);
    Value* visitMulExp(ParseTree* node);
    Value* visitAddExp(ParseTree* node);
    Value* visitRelExp(ParseTree* node);
    Value* visitEqExp(ParseTree* node);
    Value* visitLAndExp(ParseTree* node);
    Value* visitLOrExp(ParseTree* node);

public:
    IRVisitor();
    ~IRVisitor();
    void visit(ParseTree* node);
    std::string output();
};

#endif
