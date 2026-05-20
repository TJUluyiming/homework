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

/**
 * 语法树访问者：遍历 ParseTree，调用课程中端库生成 LLVM IR。
 * 符号表用作用域栈 symbol_scopes；表达式 visit* 返回 Value*。
 */
class IRVisitor {
private:
    Module *module_;       // IR 模块（全局变量、函数声明/定义）
    IRBuilder *builder_;   // 在当前基本块插入指令
    Function *cur_func_;   // 正在生成的函数（visitFuncDef 内非空）

    /** 作用域栈：栈底为全局，每进入函数/块 push 一层 */
    std::vector<std::map<std::string, Value*>> symbol_scopes;

    int temp_counter = 0;
    int label_counter = 0;
    std::string newTemp() { return "tmp" + std::to_string(temp_counter++); }
    std::string newLabel() { return "L" + std::to_string(label_counter++); }

    BasicBlock* createBB(const std::string &name = "");
    Value* lookupSymbol(const std::string &name);
    Type* getTypeFromBType(ParseTree* btype);
    void declareBuiltins();  // getint / putint 等运行时声明

    /** 收集形参/实参时使用的临时向量（visitFuncFParam / visitFuncRParam） */
    std::vector<Type*> func_param_types_;
    std::vector<std::string> func_param_names_;
    std::vector<Value*> func_call_args_;

    std::string source_file_;  // 写入 .ll 头部 source_filename

    // --- 结构类 visit：无返回值，向 module 写入声明/指令 ---
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

    // --- 表达式类 visit：返回 Value* ---
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
    void setSourceFile(const std::string& path);
    void visit(ParseTree* node);
    std::string output();  // 模块头 + set_print_name + Module::print
};

#endif
