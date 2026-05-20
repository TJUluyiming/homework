#include "ir_visitor.h"
#include <algorithm>

//==============================================================================
// ??????????????
//==============================================================================

namespace {
constexpr const char* kModuleId = "sysy2026_compiler";
}

IRVisitor::IRVisitor() {
    module_ = new Module(kModuleId);
    builder_ = new IRBuilder(nullptr, module_);
    cur_func_ = nullptr;
}

void IRVisitor::setSourceFile(const std::string& path) {
    source_file_ = path;
}

IRVisitor::~IRVisitor() {
    delete builder_;
}

//==============================================================================
// ???????????
//==============================================================================

/** ??????????????????????????????? declare??????????????? */
void IRVisitor::declareBuiltins() {
    auto *i32 = Type::get_int32_type(module_);
    auto *void_ty = Type::get_void_type(module_);

    Function::create(FunctionType::get(i32, {}), "getint", module_);
    Function::create(FunctionType::get(i32, {}), "getch", module_);
    Function::create(FunctionType::get(i32, {Type::get_int32_ptr_type(module_)}), "getarray", module_);
    Function::create(FunctionType::get(void_ty, {i32}), "putint", module_);
    Function::create(FunctionType::get(void_ty, {i32}), "putch", module_);
    Function::create(FunctionType::get(void_ty, {i32, Type::get_int32_ptr_type(module_)}), "putarray", module_);
    Function::create(FunctionType::get(void_ty, {}), "starttime", module_);
    Function::create(FunctionType::get(void_ty, {}), "stoptime", module_);
}

/** ????????????????????????????????????????? label_ ??????? */
BasicBlock* IRVisitor::createBB(const std::string &name) {
    std::string bb_name = name.empty() ? newLabel() : name;
    auto *bb = BasicBlock::create(module_, bb_name, cur_func_);
    return bb;
}

/** ??????????????????????????????????????????????????????? alloca?? */
Value* IRVisitor::lookupSymbol(const std::string &name) {
    for (int i = (int)symbol_scopes.size() - 1; i >= 0; i--) {
        if (symbol_scopes[i].count(name))
            return symbol_scopes[i][name];
    }
    return nullptr;
}

/** bType ?????????????? int ??? float ????? */
Type* IRVisitor::getTypeFromBType(ParseTree* btype) {
    if (!btype || btype->children.empty()) return Type::get_int32_type(module_);
    std::string t = btype->children[0]->type;
    if (t == "float") return Type::get_float_type(module_);
    return Type::get_int32_type(module_);
}

//==============================================================================
// ????????? IR ?????
//==============================================================================

/**
 * ???????? type ???????????? visit*???
 * ?????????????????????????????????????????????
 */
void IRVisitor::visit(ParseTree* node) {
    if (!node) return;
    if (symbol_scopes.empty()) {
        symbol_scopes.push_back({});
        declareBuiltins();
    }

    const std::string &t = node->type;
    if (t == "Program")                    visitProgram(node);
    else if (t == "compUnit")             visitCompUnit(node);
    else if (t == "decl")                 visitDecl(node);
    else if (t == "constDecl")            visitConstDecl(node);
    else if (t == "varDecl")              visitVarDecl(node);
    else if (t == "funcDef")              visitFuncDef(node);
    else if (t == "block")                visitBlock(node);
    else if (t == "blockItems")           visitBlockItems(node);
    else if (t == "blockItem")            visitBlockItem(node);
    else if (t == "stmt")                 visitStmt(node);
    else if (t == "funcFParams")          visitFuncFParams(node);
    else if (t == "funcFParam")           visitFuncFParam(node);
    else if (t == "funcRParams")          visitFuncRParams(node);
    else if (t == "funcRParam")           visitFuncRParam(node);
    else if (t == "exp" || t == "constExp") visitExp(node);
    else visitExp(node);  // ???????????????????
}

/** ?????/?????????????????????????????? LLVM IR ?????? */
std::string IRVisitor::output() {
    module_->set_print_name();
    std::string body = module_->print();

    std::string header = std::string("; ModuleID = '") + kModuleId + "'\n";
    if (!source_file_.empty()) {
        header += "source_filename = \"" + source_file_ + "\"\n";
    }
    header += "\n";
    return header + body;
}

//==============================================================================
// ??????????????????????? visit??
//==============================================================================

void IRVisitor::visitProgram(ParseTree* node) {
    for (auto *c : node->children) visit(c);
}

void IRVisitor::visitCompUnit(ParseTree* node) {
    for (auto *c : node->children) visit(c);
}

void IRVisitor::visitDecl(ParseTree* node) {
    for (auto *c : node->children) visit(c);
}

void IRVisitor::visitConstDecl(ParseTree* node) {
    // constDecl -> const bType constDefs ;
    Type *ty = getTypeFromBType(node->children[1]);
    visitConstDefs(node->children[2], ty);
}

void IRVisitor::visitConstDefs(ParseTree* node, Type* baseType) {
    // constDefs -> constDef | constDefs , constDef
    for (auto *c : node->children) {
        if (c->type == "constDef") visitConstDef(c, baseType);
        else if (c->type == "constDefs") visitConstDefs(c, baseType);
    }
}

void IRVisitor::visitConstDef(ParseTree* node, Type* baseType) {
    // constDef -> Ident = constInitVal
    std::string name = node->children[0]->text;
    Value *init = visitConstInitVal(node->children[2]);
    GlobalVariable::create(name, module_, baseType, true,
        dynamic_cast<Constant*>(init));
    symbol_scopes.back()[name] = module_->get_global_variable().back();
}

void IRVisitor::visitVarDecl(ParseTree* node) {
    // varDecl -> bType varDefs ;
    Type *ty = getTypeFromBType(node->children[0]);
    visitVarDefs(node->children[1], ty);
}

void IRVisitor::visitVarDefs(ParseTree* node, Type* baseType) {
    for (auto *c : node->children) {
        if (c->type == "varDef") visitVarDef(c, baseType);
        else if (c->type == "varDefs") visitVarDefs(c, baseType);
    }
}

void IRVisitor::visitVarDef(ParseTree* node, Type* baseType) {
    // varDef -> Ident | Ident = initVal
    std::string name = node->children[0]->text;
    Value *init = nullptr;
    if (node->children.size() > 1)
        init = visitInitVal(node->children[2]);

    if (symbol_scopes.size() == 1) {
        // ????????????GlobalVariable + ??????????
        auto *gv = GlobalVariable::create(name, module_, baseType, false,
            init ? dynamic_cast<Constant*>(init) : ConstantZero::get(baseType, module_));
        symbol_scopes.back()[name] = gv;
    } else {
        // ???????????????? alloca?????? store ??????
        auto *alloca = builder_->create_alloca(baseType);
        alloca->set_name(name);
        if (init)
            builder_->create_store(init, alloca);
        symbol_scopes.back()[name] = alloca;
    }
}

//==============================================================================
// ???????????????
//==============================================================================

void IRVisitor::visitFuncDef(ParseTree* node) {
    // funcDef -> funcType Ident ( ) block
    //         | funcType Ident ( funcFParams ) block
    std::string ret_str = node->children[0]->children[0]->type;
    Type *ret_type;
    if (ret_str == "void") ret_type = Type::get_void_type(module_);
    else if (ret_str == "float") ret_type = Type::get_float_type(module_);
    else ret_type = Type::get_int32_type(module_);

    std::string func_name = node->children[1]->text;

    func_param_types_.clear();
    func_param_names_.clear();
    ParseTree *block_node = nullptr;

    if (node->children.size() == 5) {
        block_node = node->children[4];
    } else {
        visit(node->children[3]);  // funcFParams
        block_node = node->children[5];
    }

    auto *func_ty = FunctionType::get(ret_type, func_param_types_);
    auto *func = Function::create(func_ty, func_name, module_);
    cur_func_ = func;

    auto *entry_bb = createBB("ENTRY");
    builder_->set_insert_point(entry_bb);
    builder_->set_curFunc(func);

    symbol_scopes.push_back({});

    // ????????????????? store ??? alloca??????? alloca ?????
    auto arg_it = func->get_args().begin();
    for (size_t i = 0; i < func_param_names_.size(); i++, ++arg_it) {
        (*arg_it)->set_name("param_" + func_param_names_[i]);
        auto *alloca = builder_->create_alloca(func_param_types_[i]);
        alloca->set_name(func_param_names_[i]);
        builder_->create_store(*arg_it, alloca);
        symbol_scopes.back()[func_param_names_[i]] = alloca;
    }

    visit(block_node);

    if (!builder_->get_insert_block()->get_terminator()) {
        if (ret_type->is_void_type())
            builder_->create_void_ret();
        else
            builder_->create_ret(ConstantZero::get(ret_type, module_));
    }

    symbol_scopes.pop_back();
    cur_func_ = nullptr;
}

void IRVisitor::visitBlock(ParseTree* node) {
    for (auto *c : node->children)
        if (c->type == "blockItems") visit(c);
}

void IRVisitor::visitBlockItems(ParseTree* node) {
    for (auto *c : node->children) visit(c);
}

void IRVisitor::visitBlockItem(ParseTree* node) {
    for (auto *c : node->children) visit(c);
}

//==============================================================================
// ??
//==============================================================================

void IRVisitor::visitStmt(ParseTree* node) {
    if (node->children.empty()) return;

    std::string first_type = node->children[0]->type;

    if (first_type == "lVal") {
        // stmt -> lVal = exp ;
        Value *ptr = visitLVal(node->children[0]);
        Value *val = visitExp(node->children[2]);
        if (ptr && val) builder_->create_store(val, ptr);
    }
    else if (first_type == "exp") {
        visitExp(node->children[0]);
    }
    else if (first_type == "block") {
        symbol_scopes.push_back({});
        visit(node->children[0]);
        symbol_scopes.pop_back();
    }
    else if (first_type == "return") {
        if (node->children.size() > 2 && node->children[1]->type == "exp") {
            builder_->create_ret(visitExp(node->children[1]));
        } else {
            builder_->create_void_ret();
        }
    }
    else if (first_type == "if") {
        Value *cond = visitCond(node->children[2]);
        auto *true_bb = createBB();
        if (node->children.size() == 5) {
            // if ( cond ) stmt
            auto *end_bb = createBB();
            builder_->create_cond_br(cond, true_bb, end_bb);

            builder_->set_insert_point(true_bb);
            visit(node->children[4]);
            builder_->create_br(end_bb);

            builder_->set_insert_point(end_bb);
        } else {
            // if ( cond ) stmt else stmt
            auto *false_bb = createBB();
            auto *end_bb = createBB();
            builder_->create_cond_br(cond, true_bb, false_bb);

            builder_->set_insert_point(true_bb);
            visit(node->children[4]);
            builder_->create_br(end_bb);

            builder_->set_insert_point(false_bb);
            visit(node->children[6]);
            builder_->create_br(end_bb);

            builder_->set_insert_point(end_bb);
        }
    }
    else if (first_type == ";") {
        // ???
    }
}

//==============================================================================
// ???????????????????????????????????????? visitFuncDef / ???????????
//==============================================================================

void IRVisitor::visitFuncFParams(ParseTree* node) {
    for (auto *c : node->children) {
        if (c->type == "funcFParam") visitFuncFParam(c);
        else if (c->type == "funcFParams") visitFuncFParams(c);
    }
}

void IRVisitor::visitFuncFParam(ParseTree* node) {
    // funcFParam -> bType Ident
    Type *pt = getTypeFromBType(node->children[0]);
    std::string name = node->children[1]->text;
    func_param_types_.push_back(pt);
    func_param_names_.push_back(name);
}

void IRVisitor::visitFuncRParams(ParseTree* node) {
    for (auto *c : node->children) {
        if (c->type == "funcRParam") visitFuncRParam(c);
        else if (c->type == "funcRParams") visitFuncRParams(c);
    }
}

void IRVisitor::visitFuncRParam(ParseTree* node) {
    // funcRParam -> exp
    if (!node->children.empty())
        func_call_args_.push_back(visitExp(node->children[0]));
}

//==============================================================================
// ?????????? Value*?????????????????? exp/cond/addExp ????
//==============================================================================

Value* IRVisitor::visitInitVal(ParseTree* node) {
    if (!node) return nullptr;
    return visitExp(node->children[0]);
}

Value* IRVisitor::visitConstInitVal(ParseTree* node) {
    if (!node) return nullptr;
    return visitExp(node->children[0]);
}

Value* IRVisitor::visitExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "initVal" || node->type == "constInitVal")
        return visitExp(node->children[0]);
    if (node->type == "exp" || node->type == "constExp")
        return visitAddExp(node->children[0]);
    return visitAddExp(node);
}

Value* IRVisitor::visitCond(ParseTree* node) {
    if (!node) return nullptr;
    if (node->type == "cond") return visitLOrExp(node->children[0]);
    return nullptr;
}

/** lVal -> Ident????????????????????????????? visitStmt ?????? store */
Value* IRVisitor::visitLVal(ParseTree* node) {
    if (!node) return nullptr;
    std::string name = node->children[0]->text;
    return lookupSymbol(name);
}

Value* IRVisitor::visitPrimaryExp(ParseTree* node) {
    if (!node) return nullptr;
    // primaryExp -> ( exp )
    if (node->children.size() == 3 && node->children[0]->type == "(")
        return visitExp(node->children[1]);
    // primaryExp -> lVal????????? load??
    if (node->children[0]->type == "lVal") {
        Value *ptr = visitLVal(node->children[0]);
        if (ptr) {
            auto *load = builder_->create_load(ptr);
            load->set_name(newTemp());
            return load;
        }
    }
    // primaryExp -> number
    if (node->children[0]->type == "number") {
        ParseTree *num_node = node->children[0]->children[0];
        if (num_node->type == "IntConst")
            return ConstantInt::get(std::stoi(num_node->text), module_);
        if (num_node->type == "floatConst")
            return ConstantFloat::get(std::stof(num_node->text), module_);
    }
    return nullptr;
}

Value* IRVisitor::visitUnaryExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children[0]->type == "primaryExp")
        return visitPrimaryExp(node->children[0]);

    // unaryExp -> Ident ( ) | Ident ( funcRParams )
    if (node->children[0]->type == "Ident") {
        std::string func_name = node->children[0]->text;
        std::vector<Value*> args;
        if (node->children.size() > 3 && node->children[2]->type == "funcRParams") {
            func_call_args_.clear();
            visit(node->children[2]);
            args = func_call_args_;
        }
        for (auto &f : module_->get_functions()) {
            if (f->get_name() == func_name) {
                auto *call = builder_->create_call(f, args);
                if (!call->get_type()->is_void_type())
                    call->set_name(newTemp());
                return call;
            }
        }
        return nullptr;
    }

    // unaryExp -> unaryOp unaryExp
    if (node->children[0]->type == "unaryOp") {
        std::string op = node->children[0]->children[0]->type;
        Value *operand = visitUnaryExp(node->children[1]);
        if (!operand) return nullptr;

        if (op == "+") return operand;
        if (op == "-") {
            if (operand->get_type()->is_float_type())
                return builder_->create_fsub(ConstantFloat::get(0.0f, module_), operand);
            else
                return builder_->create_isub(ConstantInt::get(0, module_), operand);
        }
        if (op == "!") {
            if (operand->get_type()->is_float_type())
                return builder_->create_fcmp_oeq(operand, ConstantFloat::get(0.0f, module_));
            else
                return builder_->create_icmp_eq(operand, ConstantInt::get(0, module_));
        }
    }
    return nullptr;
}

Value* IRVisitor::visitMulExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitUnaryExp(node->children[0]);

    Value *lhs = visitMulExp(node->children[0]);
    Value *rhs = visitUnaryExp(node->children[2]);
    if (!lhs || !rhs) return nullptr;

    std::string op = node->children[1]->type;
    bool is_f = lhs->get_type()->is_float_type() || rhs->get_type()->is_float_type();

    if (op == "*")
        return is_f ? builder_->create_fmul(lhs, rhs) : builder_->create_imul(lhs, rhs);
    if (op == "/")
        return is_f ? builder_->create_fdiv(lhs, rhs) : builder_->create_isdiv(lhs, rhs);
    if (op == "%")
        return builder_->create_irem(lhs, rhs);
    return nullptr;
}

Value* IRVisitor::visitAddExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitMulExp(node->children[0]);

    Value *lhs = visitAddExp(node->children[0]);
    Value *rhs = visitMulExp(node->children[2]);
    if (!lhs || !rhs) return nullptr;

    std::string op = node->children[1]->type;
    bool is_f = lhs->get_type()->is_float_type() || rhs->get_type()->is_float_type();

    if (op == "+")
        return is_f ? builder_->create_fadd(lhs, rhs) : builder_->create_iadd(lhs, rhs);
    if (op == "-")
        return is_f ? builder_->create_fsub(lhs, rhs) : builder_->create_isub(lhs, rhs);
    return nullptr;
}

Value* IRVisitor::visitRelExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitAddExp(node->children[0]);

    Value *lhs = visitRelExp(node->children[0]);
    Value *rhs = visitAddExp(node->children[2]);
    if (!lhs || !rhs) return nullptr;

    std::string op = node->children[1]->type;
    bool is_f = lhs->get_type()->is_float_type() || rhs->get_type()->is_float_type();

    if (op == "<")  return is_f ? builder_->create_fcmp_olt(lhs, rhs) : builder_->create_icmp_lt(lhs, rhs);
    if (op == ">")  return is_f ? builder_->create_fcmp_ogt(lhs, rhs) : builder_->create_icmp_gt(lhs, rhs);
    if (op == "<=") return is_f ? builder_->create_fcmp_ole(lhs, rhs) : builder_->create_icmp_le(lhs, rhs);
    if (op == ">=") return is_f ? builder_->create_fcmp_oge(lhs, rhs) : builder_->create_icmp_ge(lhs, rhs);
    return nullptr;
}

Value* IRVisitor::visitEqExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitRelExp(node->children[0]);

    Value *lhs = visitEqExp(node->children[0]);
    Value *rhs = visitRelExp(node->children[2]);
    if (!lhs || !rhs) return nullptr;

    std::string op = node->children[1]->type;
    bool is_f = lhs->get_type()->is_float_type() || rhs->get_type()->is_float_type();

    if (op == "==")
        return is_f ? builder_->create_fcmp_oeq(lhs, rhs) : builder_->create_icmp_eq(lhs, rhs);
    if (op == "!=")
        return is_f ? builder_->create_fcmp_one(lhs, rhs) : builder_->create_icmp_ne(lhs, rhs);
    return nullptr;
}

/** ?????????????????? */
Value* IRVisitor::visitLAndExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitEqExp(node->children[0]);

    auto *result = builder_->create_alloca(Type::get_int1_type(module_));
    result->set_name(newTemp());

    auto *next_bb = createBB();
    auto *false_bb = createBB();
    auto *end_bb = createBB();

    Value *lhs = visitLAndExp(node->children[0]);
    builder_->create_cond_br(lhs, next_bb, false_bb);

    builder_->set_insert_point(next_bb);
    Value *rhs = visitEqExp(node->children[2]);
    builder_->create_store(rhs, result);
    builder_->create_br(end_bb);

    builder_->set_insert_point(false_bb);
    builder_->create_store(ConstantInt::get(false, module_), result);
    builder_->create_br(end_bb);

    builder_->set_insert_point(end_bb);
    auto *load = builder_->create_load(result);
    load->set_name(newTemp());
    return load;
}

/** ??????????????????? */
Value* IRVisitor::visitLOrExp(ParseTree* node) {
    if (!node) return nullptr;
    if (node->children.size() == 1) return visitLAndExp(node->children[0]);

    auto *result = builder_->create_alloca(Type::get_int1_type(module_));
    result->set_name(newTemp());

    auto *true_bb = createBB();
    auto *next_bb = createBB();
    auto *end_bb = createBB();

    Value *lhs = visitLOrExp(node->children[0]);
    builder_->create_cond_br(lhs, true_bb, next_bb);

    builder_->set_insert_point(next_bb);
    Value *rhs = visitLAndExp(node->children[2]);
    builder_->create_store(rhs, result);
    builder_->create_br(end_bb);

    builder_->set_insert_point(true_bb);
    builder_->create_store(ConstantInt::get(true, module_), result);
    builder_->create_br(end_bb);

    builder_->set_insert_point(end_bb);
    auto *load = builder_->create_load(result);
    load->set_name(newTemp());
    return load;
}
