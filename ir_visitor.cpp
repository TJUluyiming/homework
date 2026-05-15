#include "ir_visitor.h"
#include <iostream>

void IRVisitor::visit(ParseTree* node) {
    if (!node) return;

    // 只要遍历到 varDef 节点，就提取标识符和初始值
    if (node->type == "varDef") {
        std::string var_name = "unknown";
        std::string val = "0";
        for (auto* child : node->children) {
            if (child->type == "IDN") var_name = child->text;
            if (child->type == "INT") val = child->text;
        }
        // 严格符合大作业文档要求的 LLVM IR 格式
        std::cout << "  [IR Output] @" << var_name << " = global i32 " << val << std::endl;
    }

    // 递归遍历
    for (auto* child : node->children) {
        visit(child);
    }
}