// ir_visitor.cpp
#include "ir_visitor.h"

void IRVisitor::visit(ParseTree* node) {
    if (!node) return;
    
    // 初始化全局作用域
    if (symbol_scopes.empty()) symbol_scopes.push_back({});
    
    if (node->type == "Program") {
        for (auto* c : node->children) visit(c);
    }
    else if (node->type == "compUnit") {
        for (auto* c : node->children) visit(c);
    }
    else if (node->type == "decl") {
        for (auto* c : node->children) visit(c);
    }
    else if (node->type == "constDecl") {
        Type type("i32");
        std::string var_name;
        Value* init_val = nullptr;
        bool is_const = true;
        
        for (auto* c : node->children) {
            if (c->type == "bType") {
                for (auto* cc : c->children) {
                    if (cc->type == "int") type = Type("i32");
                    if (cc->type == "float") type = Type("float");
                }
            }
            if (c->type == "constDef") {
                for (auto* cc : c->children) {
                    if (cc->type == "IDN") var_name = cc->text;
                    if (cc->type == "constInitVal") {
                        init_val = visitExp(cc->children[0]);
                    }
                }
            }
        }
        
        if (!var_name.empty() && init_val && init_val->isConstant()) {
            Value* var = GlobalVariable::create(var_name, type, is_const, init_val);
            symbol_scopes.back()[var_name] = var;
        }
    }
    else if (node->type == "varDecl") {
        Type type("i32");
        std::string var_name;
        Value* init_val = nullptr;
        
        for (auto* c : node->children) {
            if (c->type == "bType") {
                for (auto* cc : c->children) {
                    if (cc->type == "int") type = Type("i32");
                    if (cc->type == "float") type = Type("float");
                }
            }
            if (c->type == "varDef") {
                for (auto* cc : c->children) {
                    if (cc->type == "IDN") var_name = cc->text;
                    if (cc->type == "initVal") {
                        init_val = visitExp(cc->children[0]);
                    }
                }
            }
        }
        
        if (!var_name.empty()) {
            Value* var;
            if (symbol_scopes.size() == 1) { // 全局变量
                var = GlobalVariable::create(var_name, type, false, init_val);
            } else { // 局部变量
                var = new AllocaInst(var_name, type);
                if (init_val) {
                    new StoreInst(init_val, var);
                }
            }
            symbol_scopes.back()[var_name] = var;
        }
    }
    else if (node->type == "funcDef") {
        std::string func_name = "main";
        Type return_type("i32");
        std::vector<std::string> params;
        
        // 获取函数名和返回类型
        for (auto* c : node->children) {
            if (c->type == "funcType") {
                for (auto* cc : c->children) {
                    if (cc->type == "int") return_type = Type("i32");
                    if (cc->type == "void") return_type = Type("void");
                }
            }
            if (c->type == "IDN" || c->text == "main") func_name = c->text;
            if (c->type == "funcFParams") {
                for (auto* cc : c->children) {
                    if (cc->type == "funcFParam") {
                        for (auto* ccc : cc->children) {
                            if (ccc->type == "IDN") {
                                params.push_back(ccc->text);
                            }
                        }
                    }
                }
            }
        }
        
        // 创建函数
        Function::create(func_name, return_type, params);
        std::cout << "entry:\n";
        
        // 进入函数局部作用域
        symbol_scopes.push_back({});
        
        // 分配函数参数空间
        for (const auto& param : params) {
            Value* alloca = new AllocaInst(param, Type("i32"));
            symbol_scopes.back()[param] = alloca;
        }
        
        // 遍历函数体
        for (auto* c : node->children) {
            if (c->type == "block") {
                for (auto* cc : c->children) {
                    if (cc->type == "blockItems") {
                        for (auto* ccc : cc->children) {
                            visit(ccc);
                        }
                    }
                }
            }
        }
        
        // 退出作用域
        symbol_scopes.pop_back();
        std::cout << "}\n";
    }
    else if (node->type == "stmt") {
        // 赋值语句
        if (node->children.size() >= 4 && node->children[1]->type == "=") {
            Value* lval = visitLVal(node->children[0]);
            Value* exp = visitExp(node->children[2]);
            if (lval && exp) {
                new StoreInst(exp, lval);
            }
        }
        // return语句
        else if (!node->children.empty() && node->children[0]->type == "return") {
            if (node->children.size() == 4) { // return exp ;
                Value* ret_val = visitExp(node->children[1]);
                if (ret_val) {
                    new ReturnInst(ret_val);
                }
            } else { // return ;
                new ReturnInst();
            }
        }
        // if语句
        else if (!node->children.empty() && node->children[0]->type == "if") {
            Value* cond = visitCond(node->children[2]);
            std::string true_label = newLabel();
            std::string false_label = newLabel();
            std::string end_label = newLabel();
            
            if (node->children.size() == 7) { // if-else
                new BranchInst(cond, true_label, false_label);
                
                new LabelInst(true_label);
                visit(node->children[4]);
                new BranchInst(end_label);
                
                new LabelInst(false_label);
                visit(node->children[6]);
                new BranchInst(end_label);
                
                new LabelInst(end_label);
            } else { // if
                new BranchInst(cond, true_label, end_label);
                
                new LabelInst(true_label);
                visit(node->children[4]);
                new BranchInst(end_label);
                
                new LabelInst(end_label);
            }
        }
        // 块语句
        else if (!node->children.empty() && node->children[0]->type == "block") {
            symbol_scopes.push_back({});
            for (auto* c : node->children[0]->children) {
                if (c->type == "blockItems") {
                    for (auto* cc : c->children) {
                        visit(cc);
                    }
                }
            }
            symbol_scopes.pop_back();
        }
        // 表达式语句
        else if (node->children.size() == 2 && node->children[1]->type == ";") {
            visitExp(node->children[0]);
        }
    }
    else {
        // 递归访问其他节点
        for (auto* c : node->children) {
            visit(c);
        }
    }
}

Value* IRVisitor::visitExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "exp") {
        return visitAddExp(node->children[0]);
    }
    return visitAddExp(node);
}

Value* IRVisitor::visitCond(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "cond") {
        return visitLOrExp(node->children[0]);
    }
    return visitLOrExp(node);
}

Value* IRVisitor::visitLVal(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "lVal") {
        std::string name = node->children[0]->text;
        // 从符号表查找变量（从内到外）
        for (auto it = symbol_scopes.rbegin(); it != symbol_scopes.rend(); ++it) {
            if (it->count(name)) {
                return (*it)[name];
            }
        }
        std::cerr << "Error: Undefined variable '" << name << "'\n";
    }
    return nullptr;
}

Value* IRVisitor::visitPrimaryExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "primaryExp") {
        if (node->children.size() == 3 && node->children[0]->type == "(") {
            return visitExp(node->children[1]);
        } else if (node->children.size() == 1 && node->children[0]->type == "lVal") {
            Value* lval = visitLVal(node->children[0]);
            if (lval) {
                return new LoadInst(newTemp(), lval);
            }
        } else if (node->children.size() == 1 && node->children[0]->type == "number") {
            if (node->children[0]->children[0]->type == "INT") {
                return ConstantInt::get(std::stoi(node->children[0]->children[0]->text));
            }
        }
    }
    return nullptr;
}

Value* IRVisitor::visitUnaryExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "unaryExp") {
        if (node->children.size() == 1) {
            return visitPrimaryExp(node->children[0]);
        } else if (node->children.size() == 2) {
            std::string op = node->children[0]->children[0]->type;
            Value* operand = visitUnaryExp(node->children[1]);
            if (operand) {
                if (op == "-") {
                    return new BinaryOpInst(newTemp(), "sub", ConstantInt::get(0), operand);
                } else if (op == "!") {
                    return new ICmpInst(newTemp(), "eq", operand, ConstantInt::get(0));
                }
                // + 单目运算符不做处理
                return operand;
            }
        }
    }
    return nullptr;
}

Value* IRVisitor::visitMulExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "mulExp") {
        if (node->children.size() == 1) {
            return visitUnaryExp(node->children[0]);
        } else {
            Value* lhs = visitMulExp(node->children[0]);
            std::string op = node->children[1]->type;
            Value* rhs = visitUnaryExp(node->children[2]);
            if (lhs && rhs) {
                if (op == "*") {
                    return new BinaryOpInst(newTemp(), "mul", lhs, rhs);
                } else if (op == "/") {
                    return new BinaryOpInst(newTemp(), "sdiv", lhs, rhs);
                } else if (op == "%") {
                    return new BinaryOpInst(newTemp(), "srem", lhs, rhs);
                }
            }
        }
    }
    return nullptr;
}

Value* IRVisitor::visitAddExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "addExp") {
        if (node->children.size() == 1) {
            return visitMulExp(node->children[0]);
        } else {
            Value* lhs = visitAddExp(node->children[0]);
            std::string op = node->children[1]->type;
            Value* rhs = visitMulExp(node->children[2]);
            if (lhs && rhs) {
                if (op == "+") {
                    return new BinaryOpInst(newTemp(), "add", lhs, rhs);
                } else if (op == "-") {
                    return new BinaryOpInst(newTemp(), "sub", lhs, rhs);
                }
            }
        }
    }
    return nullptr;
}

Value* IRVisitor::visitRelExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "relExp") {
        if (node->children.size() == 1) {
            return visitAddExp(node->children[0]);
        } else {
            Value* lhs = visitRelExp(node->children[0]);
            std::string op = node->children[1]->type;
            Value* rhs = visitAddExp(node->children[2]);
            if (lhs && rhs) {
                if (op == "<") {
                    return new ICmpInst(newTemp(), "slt", lhs, rhs);
                } else if (op == ">") {
                    return new ICmpInst(newTemp(), "sgt", lhs, rhs);
                } else if (op == "<=") {
                    return new ICmpInst(newTemp(), "sle", lhs, rhs);
                } else if (op == ">=") {
                    return new ICmpInst(newTemp(), "sge", lhs, rhs);
                }
            }
        }
    }
    return nullptr;
}

Value* IRVisitor::visitEqExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "eqExp") {
        if (node->children.size() == 1) {
            return visitRelExp(node->children[0]);
        } else {
            Value* lhs = visitEqExp(node->children[0]);
            std::string op = node->children[1]->type;
            Value* rhs = visitRelExp(node->children[2]);
            if (lhs && rhs) {
                if (op == "==") {
                    return new ICmpInst(newTemp(), "eq", lhs, rhs);
                } else if (op == "!=") {
                    return new ICmpInst(newTemp(), "ne", lhs, rhs);
                }
            }
        }
    }
    return nullptr;
}

Value* IRVisitor::visitLAndExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "lAndExp") {
        if (node->children.size() == 1) {
            return visitEqExp(node->children[0]);
        } else {
            Value* lhs = visitLAndExp(node->children[0]);
            Value* rhs = visitEqExp(node->children[2]);
            if (lhs && rhs) {
                return new BinaryOpInst(newTemp(), "and", lhs, rhs);
            }
        }
    }
    return nullptr;
}

Value* IRVisitor::visitLOrExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "lOrExp") {
        if (node->children.size() == 1) {
            return visitLAndExp(node->children[0]);
        } else {
            Value* lhs = visitLOrExp(node->children[0]);
            Value* rhs = visitLAndExp(node->children[2]);
            if (lhs && rhs) {
                return new BinaryOpInst(newTemp(), "or", lhs, rhs);
            }
        }
    }
    return nullptr;
}
