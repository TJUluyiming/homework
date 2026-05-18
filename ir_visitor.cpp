// ir_visitor.cpp
#include "ir_visitor.h"
#include <algorithm>

IRVisitor::IRVisitor() {
    std::cout << "declare i32 @getint()\n";
    std::cout << "declare i32 @getch()\n";
    std::cout << "declare i32 @getarray(i32*)\n";
    std::cout << "declare void @putint(i32)\n";
    std::cout << "declare void @putch(i32)\n";
    std::cout << "declare void @putarray(i32, i32*)\n";
    std::cout << "declare void @starttime()\n";
    std::cout << "declare void @stoptime()\n\n";
}
void IRVisitor::extractVarDefs(ParseTree* node, std::vector<ParseTree*>& defs) {
    if (!node) return;
    if (node->type == "varDef") defs.push_back(node);
    else for (auto* c : node->children) extractVarDefs(c, defs);
}

void IRVisitor::extractConstDefs(ParseTree* node, std::vector<ParseTree*>& defs) {
    if (!node) return;
    if (node->type == "constDef") defs.push_back(node);
    else for (auto* c : node->children) extractConstDefs(c, defs);
}

void IRVisitor::extractFuncFParams(ParseTree* node, std::vector<ParseTree*>& params) {
    if (!node) return;
    if (node->type == "funcFParam") params.push_back(node);
    else for (auto* c : node->children) extractFuncFParams(c, params);
}

// 提取实际参数列表
void IRVisitor::extractFuncRParams(ParseTree* node, std::vector<ParseTree*>& params) {
    if (!node) return;
    if (node->type == "funcRParam") params.push_back(node);
    else for (auto* c : node->children) extractFuncRParams(c, params);
}

void IRVisitor::visit(ParseTree* node) {
    if (!node) return;
    if (symbol_scopes.empty()) symbol_scopes.push_back({});
    
    if (node->type == "Program" || node->type == "compUnit" || node->type == "decl" || node->type == "blockItems") {
        for (auto* c : node->children) visit(c);
    }
    else if (node->type == "constDecl") {
        Type type("i32");
        if (node->children[1] && !node->children[1]->children.empty()) {
            type = Type(node->children[1]->children[0]->type); 
        }
        std::vector<ParseTree*> const_defs;
        extractConstDefs(node->children[2], const_defs);
        for (auto* def : const_defs) {
            std::string var_name = def->children[0]->text;
            Value* init_val = visitExp(def->children[2]->children[0]);
            if (!var_name.empty() && init_val) {
                symbol_scopes.back()[var_name] = GlobalVariable::create(var_name, type, true, init_val);
            }
        }
    }
    else if (node->type == "varDecl") {
        Type type("i32");
        if (node->children[0] && !node->children[0]->children.empty()) {
            type = Type(node->children[0]->children[0]->type);
        }
        std::vector<ParseTree*> var_defs;
        extractVarDefs(node->children[1], var_defs);
        for (auto* def : var_defs) {
            std::string var_name = def->children[0]->text;
            Value* init_val = (def->children.size() > 1) ? visitExp(def->children[2]) : nullptr;
            if (!var_name.empty()) {
                Value* var;
                if (symbol_scopes.size() == 1) {
                    var = GlobalVariable::create(var_name, type, false, init_val);
                } else {
                    var = new AllocaInst(var_name, type);
                    if (init_val) new StoreInst(init_val, var);
                }
                symbol_scopes.back()[var_name] = var;
            }
        }
    }
    else if (node->type == "funcDef") {
        std::string ret_str = node->children[0]->children[0]->type;
        Type ret_type(ret_str); std::string func_name = node->children[1]->text;
        
        std::cout << "define " << ret_type.name << " @" << func_name << "(";
        std::vector<ParseTree*> params; ParseTree* block_node = nullptr;
        if (node->children.size() == 5) block_node = node->children[4];
        else { extractFuncFParams(node->children[3], params); block_node = node->children[6]; }
        
        for (size_t i = 0; i < params.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << params[i]->children[0]->children[0]->type << " %param_" << params[i]->children[1]->text;
        }
        std::cout << ") {\n" << func_name << "_ENTRY:\n";
        symbol_scopes.push_back({});
        
        for (auto* p : params) {
            std::string p_type = p->children[0]->children[0]->type; std::string p_name = p->children[1]->text;
            Value* alloca_space = new AllocaInst(p_name, Type(p_type));
            struct ParamValue : public Value {
                std::string name; Type t; ParamValue(std::string n, Type _t) : name(n), t(_t) {}
                std::string getName() const override { return "%param_" + name; }
                Type getType() const override { return t; }
            } p_val(p_name, Type(p_type));
            new StoreInst(&p_val, alloca_space);
            symbol_scopes.back()[p_name] = alloca_space;
        }
        visit(block_node);
        symbol_scopes.pop_back(); std::cout << "}\n";
    }
    else if (node->type == "block") {
        for (auto* c : node->children) if (c->type == "blockItems") visit(c);
    }
    else if (node->type == "blockItem") {
        for (auto* c : node->children) visit(c);
    }
    else if (node->type == "stmt") {
        if (node->children[0]->type == "lVal") {
            Value* ptr = visitLVal(node->children[0]); Value* val = visitExp(node->children[2]);
            if (ptr && val) new StoreInst(val, ptr);
        }
        else if (node->children[0]->type == "exp") visitExp(node->children[0]);
        else if (node->children[0]->type == "block") {
            symbol_scopes.push_back({}); visit(node->children[0]); symbol_scopes.pop_back();
        }
        else if (node->children[0]->type == "return") {
            if (node->children.size() > 2 && node->children[1]->type == "exp") {
                new ReturnInst(visitExp(node->children[1]));
            } else new ReturnInst(nullptr);
        }
        else if (node->children[0]->type == "if") {
            Value* cond = visitCond(node->children[2]);
            std::string true_lbl = newLabel(); std::string end_lbl = newLabel();
            if (node->children.size() == 5) {
                new BranchInst(cond, true_lbl, end_lbl);
                new LabelInst(true_lbl); visit(node->children[4]); new BranchInst(end_lbl);
                new LabelInst(end_lbl);
            } else {
                std::string false_lbl = newLabel();
                new BranchInst(cond, true_lbl, false_lbl);
                new LabelInst(true_lbl); visit(node->children[4]); new BranchInst(end_lbl);
                new LabelInst(false_lbl); visit(node->children[6]); new BranchInst(end_lbl);
                new LabelInst(end_lbl);
            }
        }
    }
}

Value* IRVisitor::visitExp(ParseTree* node) {
    if (!node) return nullptr;
    
    // 新增：如果是 initVal 或 constInitVal 节点，剥开一层往下传
    if (node->type == "initVal" || node->type == "constInitVal") {
        return visitExp(node->children[0]);
    }
    
    if (node->type == "exp" || node->type == "constExp") {
        return visitAddExp(node->children[0]);
    }
    
    return visitAddExp(node);
}

Value* IRVisitor::visitCond(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "cond") return visitLOrExp(node->children[0]);
    return nullptr;
}

Value* IRVisitor::visitLVal(ParseTree* node) {
    if (!node) return nullptr;
    std::string name = node->children[0]->text;
    for (int i = (int)symbol_scopes.size() - 1; i >= 0; i--) {
        if (symbol_scopes[i].count(name)) return symbol_scopes[i][name];
    }
    return nullptr;
}

Value* IRVisitor::visitPrimaryExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 3) return visitExp(node->children[1]);
    if (node->children[0]->type == "lVal") {
        Value* ptr = visitLVal(node->children[0]);
        if (ptr) return new LoadInst(newTemp(), ptr);
    }
    if (node->children[0]->type == "number") {
        ParseTree* num_node = node->children[0]->children[0];
        if (num_node->type == "IntConst") return ConstantInt::get(std::stoi(num_node->text));
        if (num_node->type == "floatConst") return ConstantFloat::get(std::stof(num_node->text));
    }
    return nullptr;
}

Value* IRVisitor::visitUnaryExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children[0]->type == "primaryExp") return visitPrimaryExp(node->children[0]);
    
    // 核心修复：添加函数调用支持 (CallInst)
    if (node->children[0]->type == "Ident") {
        std::string func_name = node->children[0]->text;
        std::vector<Value*> args;
        
        // 存在传入参数时
        if (node->children.size() > 3 && node->children[2]->type == "funcRParams") {
            std::vector<ParseTree*> rparams;
            extractFuncRParams(node->children[2], rparams);
            for (auto* p : rparams) {
                if (!p->children.empty()) args.push_back(visitExp(p->children[0]));
            }
        }
        
        // C-- 的库调用统一回退认定为 i32 返回类型
        Value* func_val = new FunctionValue(func_name, Type("i32"));
        return new CallInst(newTemp(), func_val, args);
    }
    
    if (node->children[0]->type == "unaryOp") {
        std::string op = node->children[0]->children[0]->type; Value* operand = visitUnaryExp(node->children[1]);
        if (op == "+") return operand;
        if (op == "-") {
            if (operand->getType().isFloat()) return new BinaryInst(newTemp(), "fsub", ConstantFloat::get(0.0f), operand);
            return new BinaryInst(newTemp(), "sub", ConstantInt::get(0), operand);
        }
        if (op == "!") return new BinaryInst(newTemp(), "xor", operand, ConstantInt::get(1));
    }
    return nullptr;
}

Value* IRVisitor::visitMulExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitUnaryExp(node->children[0]);
    Value* lhs = visitMulExp(node->children[0]); Value* rhs = visitUnaryExp(node->children[2]);
    std::string op = node->children[1]->type;
    bool is_f = lhs->getType().isFloat() || rhs->getType().isFloat();
    if (op == "*") return new BinaryInst(newTemp(), is_f ? "fmul" : "mul", lhs, rhs);
    if (op == "/") return new BinaryInst(newTemp(), is_f ? "fdiv" : "sdiv", lhs, rhs);
    if (op == "%") return new BinaryInst(newTemp(), "srem", lhs, rhs);
    return nullptr;
}

Value* IRVisitor::visitAddExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitMulExp(node->children[0]);
    Value* lhs = visitAddExp(node->children[0]); Value* rhs = visitMulExp(node->children[2]);
    std::string op = node->children[1]->type;
    bool is_f = lhs->getType().isFloat() || rhs->getType().isFloat();
    if (op == "+") return new BinaryInst(newTemp(), is_f ? "fadd" : "add", lhs, rhs);
    if (op == "-") return new BinaryInst(newTemp(), is_f ? "fsub" : "sub", lhs, rhs);
    return nullptr;
}

Value* IRVisitor::visitRelExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitAddExp(node->children[0]);
    Value* lhs = visitRelExp(node->children[0]); Value* rhs = visitAddExp(node->children[2]);
    std::string op = node->children[1]->type;
    std::string instr = lhs->getType().isFloat() ? "fcmp " : "icmp ";
    if (op == "<") instr += lhs->getType().isFloat() ? "olt" : "slt";
    else if (op == ">") instr += lhs->getType().isFloat() ? "ogt" : "sgt";
    else if (op == "<=") instr += lhs->getType().isFloat() ? "ole" : "sle";
    else if (op == ">=") instr += lhs->getType().isFloat() ? "oge" : "sge";
    return new BinaryInst(newTemp(), instr, lhs, rhs);
}

Value* IRVisitor::visitEqExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitRelExp(node->children[0]);
    Value* lhs = visitEqExp(node->children[0]); Value* rhs = visitRelExp(node->children[2]);
    std::string op = node->children[1]->type;
    std::string instr = lhs->getType().isFloat() ? "fcmp " : "icmp ";
    if (op == "==") instr += lhs->getType().isFloat() ? "oeq" : "eq";
    else if (op == "!=") instr += lhs->getType().isFloat() ? "one" : "ne";
    return new BinaryInst(newTemp(), instr, lhs, rhs);
}

Value* IRVisitor::visitLAndExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitEqExp(node->children[0]);
    
    std::string next_lbl = newLabel(); std::string false_lbl = newLabel(); std::string end_lbl = newLabel();
    Value* result = new AllocaInst(newTemp(), Type("i1"));
    Value* lhs = visitLAndExp(node->children[0]);
    new BranchInst(lhs, next_lbl, false_lbl);
    
    new LabelInst(next_lbl);
    Value* rhs = visitEqExp(node->children[2]); new StoreInst(rhs, result); new BranchInst(end_lbl);
    new LabelInst(false_lbl); new StoreInst(ConstantInt::get(0), result); new BranchInst(end_lbl);
    new LabelInst(end_lbl);
    return new LoadInst(newTemp(), result);
}

Value* IRVisitor::visitLOrExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitLAndExp(node->children[0]);
    
    std::string next_lbl = newLabel(); std::string true_lbl = newLabel(); std::string end_lbl = newLabel();
    Value* result = new AllocaInst(newTemp(), Type("i1"));
    Value* lhs = visitLOrExp(node->children[0]);
    new BranchInst(lhs, true_lbl, next_lbl);
    
    new LabelInst(next_lbl);
    Value* rhs = visitLAndExp(node->children[2]); new StoreInst(rhs, result); new BranchInst(end_lbl);
    new LabelInst(true_lbl); new StoreInst(ConstantInt::get(1), result); new BranchInst(end_lbl);
    new LabelInst(end_lbl);
    return new LoadInst(newTemp(), result);
}
