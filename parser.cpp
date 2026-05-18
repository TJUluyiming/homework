// parser.cpp
#include "parser.h"
#include <iostream>

SLRParser::SLRParser() {
    // ==============================================
    // 严格按照大作业附录36条文法规则定义产生式
    // 编号与大作业完全一致，确保规约输出正确
    // ==============================================
    prods[1] = {"Program", 1};                // 1. Program -> compUnit
    prods[2] = {"compUnit", 2};               // 2. compUnit -> compUnit decl
    prods[3] = {"compUnit", 2};               // 3. compUnit -> compUnit funcDef
    prods[4] = {"compUnit", 0};               // 4. compUnit -> ε
    prods[5] = {"decl", 1};                   // 5. decl -> constDecl
    prods[6] = {"decl", 1};                   // 6. decl -> varDecl
    prods[7] = {"constDecl", 4};              // 7. constDecl -> const bType constDef ;
    prods[8] = {"bType", 1};                  // 8. bType -> int
    prods[9] = {"bType", 1};                  // 9. bType -> float
    prods[10] = {"constDef", 3};              // 10. constDef -> Ident = constInitVal
    prods[11] = {"constInitVal", 1};          // 11. constInitVal -> constExp
    prods[12] = {"varDecl", 3};               // 12. varDecl -> bType varDef ;
    prods[13] = {"varDef", 1};                // 13. varDef -> Ident
    prods[14] = {"varDef", 3};                // 14. varDef -> Ident = initVal
    prods[15] = {"initVal", 1};               // 15. initVal -> exp
    prods[16] = {"funcDef", 6};               // 16. funcDef -> funcType Ident ( ) block
    prods[17] = {"funcDef", 7};               // 17. funcDef -> funcType Ident ( funcFParams ) block
    prods[18] = {"funcType", 1};              // 18. funcType -> void
    prods[19] = {"funcType", 1};              // 19. funcType -> int
    prods[20] = {"funcFParams", 2};           // 20. funcFParams -> funcFParams , funcFParam
    prods[21] = {"funcFParams", 1};           // 21. funcFParams -> funcFParam
    prods[22] = {"funcFParam", 2};            // 22. funcFParam -> bType Ident
    prods[23] = {"block", 3};                 // 23. block -> { blockItems }
    prods[24] = {"blockItems", 2};            // 24. blockItems -> blockItems blockItem
    prods[25] = {"blockItems", 0};            // 25. blockItems -> ε
    prods[26] = {"blockItem", 1};             // 26. blockItem -> decl
    prods[27] = {"blockItem", 1};             // 27. blockItem -> stmt
    prods[28] = {"stmt", 4};                  // 28. stmt -> lVal = exp ;
    prods[29] = {"stmt", 2};                  // 29. stmt -> exp ;
    prods[30] = {"stmt", 1};                  // 30. stmt -> ;
    prods[31] = {"stmt", 1};                  // 31. stmt -> block
    prods[32] = {"stmt", 5};                  // 32. stmt -> if ( cond ) stmt
    prods[33] = {"stmt", 7};                  // 33. stmt -> if ( cond ) stmt else stmt
    prods[34] = {"stmt", 3};                  // 34. stmt -> return ;
    prods[35] = {"stmt", 4};                  // 35. stmt -> return exp ;
    prods[36] = {"exp", 1};                   // 36. exp -> addExp
    prods[37] = {"cond", 1};                  // 37. cond -> lOrExp
    prods[38] = {"lVal", 1};                  // 38. lVal -> Ident
    prods[39] = {"primaryExp", 3};            // 39. primaryExp -> ( exp )
    prods[40] = {"primaryExp", 1};            // 40. primaryExp -> lVal
    prods[41] = {"primaryExp", 1};            // 41. primaryExp -> number
    prods[42] = {"number", 1};                // 42. number -> IntConst
    prods[43] = {"number", 1};                // 43. number -> floatConst
    prods[44] = {"unaryExp", 1};              // 44. unaryExp -> primaryExp
    prods[45] = {"unaryExp", 3};              // 45. unaryExp -> Ident ( )
    prods[46] = {"unaryExp", 4};              // 46. unaryExp -> Ident ( funcRParams )
    prods[47] = {"unaryExp", 2};              // 47. unaryExp -> unaryOp unaryExp
    prods[48] = {"unaryOp", 1};               // 48. unaryOp -> +
    prods[49] = {"unaryOp", 1};               // 49. unaryOp -> -
    prods[50] = {"unaryOp", 1};               // 50. unaryOp -> !
    prods[51] = {"funcRParams", 2};           // 51. funcRParams -> funcRParams , funcRParam
    prods[52] = {"funcRParams", 1};           // 52. funcRParams -> funcRParam
    prods[53] = {"funcRParam", 1};            // 53. funcRParam -> exp
    prods[54] = {"mulExp", 1};                // 54. mulExp -> unaryExp
    prods[55] = {"mulExp", 3};                // 55. mulExp -> mulExp * unaryExp
    prods[56] = {"mulExp", 3};                // 56. mulExp -> mulExp / unaryExp
    prods[57] = {"mulExp", 3};                // 57. mulExp -> mulExp % unaryExp
    prods[58] = {"addExp", 1};                // 58. addExp -> mulExp
    prods[59] = {"addExp", 3};                // 59. addExp -> addExp + mulExp
    prods[60] = {"addExp", 3};                // 60. addExp -> addExp - mulExp
    prods[61] = {"relExp", 1};                // 61. relExp -> addExp
    prods[62] = {"relExp", 3};                // 62. relExp -> relExp < addExp
    prods[63] = {"relExp", 3};                // 63. relExp -> relExp > addExp
    prods[64] = {"relExp", 3};                // 64. relExp -> relExp <= addExp
    prods[65] = {"relExp", 3};                // 65. relExp -> relExp >= addExp
    prods[66] = {"eqExp", 1};                 // 66. eqExp -> relExp
    prods[67] = {"eqExp", 3};                 // 67. eqExp -> eqExp == relExp
    prods[68] = {"eqExp", 3};                 // 68. eqExp -> eqExp != relExp
    prods[69] = {"lAndExp", 1};               // 69. lAndExp -> eqExp
    prods[70] = {"lAndExp", 3};               // 70. lAndExp -> lAndExp && eqExp
    prods[71] = {"lOrExp", 1};                // 71. lOrExp -> lAndExp
    prods[72] = {"lOrExp", 3};                // 72. lOrExp -> lOrExp || lAndExp
    prods[73] = {"constExp", 1};              // 73. constExp -> addExp
}

SLRParser::Action SLRParser::getAction(int s, const std::string& a) {
    // 状态0：初始状态（核心修复：先规约空compUnit）
    if (s == 0) {
        if (a == "$") return {'r', 4}; // compUnit -> ε
        // 所有声明和函数定义都先触发空compUnit规约
        if (a == "const" || a == "int" || a == "void" || a == "float") {
            return {'r', 4};
        }
    }
    // 状态1：识别到const
    if (s == 1) {
        if (a == "int") return {'s', 2};
        if (a == "float") return {'s', 4};
    }
    // 状态2：识别到int
    if (s == 2) {
        if (a == "IDN") return {'r', 8}; // bType -> int
        if (a == "main") return {'r', 8};
        if (a == "(") return {'r', 19}; // funcType -> int
    }
    // 状态3：识别到void
    if (s == 3) {
        if (a == "IDN") return {'r', 18}; // funcType -> void
        if (a == "main") return {'r', 18};
    }
    // 状态4：识别到float
    if (s == 4) {
        if (a == "IDN") return {'r', 9}; // bType -> float
    }
    // 状态5：bType规约后
    if (s == 5) {
        if (a == "IDN") return {'s', 6};
        if (a == "main") return {'s', 6};
    }
    // 状态6：识别到IDN(varDef/constDef)
    if (s == 6) {
        if (a == "=") return {'s', 7};
        if (a == ";") return {'r', 13}; // varDef -> IDN
        if (a == "(") return {'s', 8}; // 函数定义/调用
        if (a == ",") return {'r', 13}; // 多个变量声明
    }
    // 状态7：识别到=
    if (s == 7) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态8：识别到(
    if (s == 8) {
        if (a == ")") return {'s', 16};
        if (a == "int") return {'s', 2};
        if (a == "float") return {'s', 4};
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态9：识别到INT
    if (s == 9) {
        if (a == ";") return {'r', 14}; // varDef -> IDN = INT
        if (a == ")") return {'r', 42}; // number -> INT
        if (a == ",") return {'r', 42};
        if (a == "+") return {'r', 42};
        if (a == "-") return {'r', 42};
        if (a == "*") return {'r', 42};
        if (a == "/") return {'r', 42};
        if (a == "%") return {'r', 42};
        if (a == "<") return {'r', 42};
        if (a == ">") return {'r', 42};
        if (a == "<=") return {'r', 42};
        if (a == ">=") return {'r', 42};
        if (a == "==") return {'r', 42};
        if (a == "!=") return {'r', 42};
        if (a == "&&") return {'r', 42};
        if (a == "||") return {'r', 42};
    }
    // 状态10：识别到FLOAT
    if (s == 10) {
        if (a == ";") return {'r', 14};
        if (a == ")") return {'r', 43}; // number -> FLOAT
        if (a == ",") return {'r', 43};
        if (a == "+") return {'r', 43};
        if (a == "-") return {'r', 43};
        if (a == "*") return {'r', 43};
        if (a == "/") return {'r', 43};
        if (a == "%") return {'r', 43};
        if (a == "<") return {'r', 43};
        if (a == ">") return {'r', 43};
        if (a == "<=") return {'r', 43};
        if (a == ">=") return {'r', 43};
        if (a == "==") return {'r', 43};
        if (a == "!=") return {'r', 43};
        if (a == "&&") return {'r', 43};
        if (a == "||") return {'r', 43};
    }
    // 状态11：识别到( (表达式)
    if (s == 11) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态12：识别到IDN(表达式)
    if (s == 12) {
        if (a == "=") return {'r', 38}; // lVal -> IDN
        if (a == ")") return {'r', 40}; // primaryExp -> lVal
        if (a == ";") return {'r', 40};
        if (a == ",") return {'r', 40};
        if (a == "+") return {'r', 40};
        if (a == "-") return {'r', 40};
        if (a == "*") return {'r', 40};
        if (a == "/") return {'r', 40};
        if (a == "%") return {'r', 40};
        if (a == "<") return {'r', 40};
        if (a == ">") return {'r', 40};
        if (a == "<=") return {'r', 40};
        if (a == ">=") return {'r', 40};
        if (a == "==") return {'r', 40};
        if (a == "!=") return {'r', 40};
        if (a == "&&") return {'r', 40};
        if (a == "||") return {'r', 40};
        if (a == "(") return {'s', 8}; // 函数调用
    }
    // 状态13：+ (单目)
    if (s == 13) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态14：- (单目)
    if (s == 14) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态15：! (单目)
    if (s == 15) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态16：) (函数参数结束)
    if (s == 16) {
        if (a == "{") return {'s', 17};
    }
    // 状态17：{ (块开始)
    if (s == 17) {
        if (a == "}") return {'r', 25}; // blockItems -> ε
        if (a == "const") return {'s', 1};
        if (a == "int") return {'s', 2};
        if (a == "float") return {'s', 4};
        if (a == "return") return {'s', 18};
        if (a == "if") return {'s', 19};
        if (a == "IDN") return {'s', 12};
        if (a == ";") return {'s', 20};
        if (a == "{") return {'s', 17};
    }
    // 状态18：return
    if (s == 18) {
        if (a == ";") return {'s', 21};
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态19：if
    if (s == 19) {
        if (a == "(") return {'s', 22};
    }
    // 状态20：; (空语句)
    if (s == 20) {
        if (a == "}") return {'r', 30}; // stmt -> ;
        if (a == "const") return {'r', 30};
        if (a == "int") return {'r', 30};
        if (a == "float") return {'r', 30};
        if (a == "return") return {'r', 30};
        if (a == "if") return {'r', 30};
        if (a == "IDN") return {'r', 30};
        if (a == "{") return {'r', 30};
        if (a == "else") return {'r', 30};
    }
    // 状态21：; (return ;)
    if (s == 21) {
        if (a == "}") return {'r', 34}; // stmt -> return ;
        if (a == "const") return {'r', 34};
        if (a == "int") return {'r', 34};
        if (a == "float") return {'r', 34};
        if (a == "return") return {'r', 34};
        if (a == "if") return {'r', 34};
        if (a == "IDN") return {'r', 34};
        if (a == "{") return {'r', 34};
        if (a == "else") return {'r', 34};
    }
    // 状态22：( (if条件)
    if (s == 22) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态23：) (if条件结束)
    if (s == 23) {
        if (a == "const") return {'s', 1};
        if (a == "int") return {'s', 2};
        if (a == "float") return {'s', 4};
        if (a == "return") return {'s', 18};
        if (a == "if") return {'s', 19};
        if (a == "IDN") return {'s', 12};
        if (a == ";") return {'s', 20};
        if (a == "{") return {'s', 17};
    }
    // 状态24：else
    if (s == 24) {
        if (a == "const") return {'s', 1};
        if (a == "int") return {'s', 2};
        if (a == "float") return {'s', 4};
        if (a == "return") return {'s', 18};
        if (a == "if") return {'s', 19};
        if (a == "IDN") return {'s', 12};
        if (a == ";") return {'s', 20};
        if (a == "{") return {'s', 17};
    }
    // 状态25：} (块结束)
    if (s == 25) {
        if (a == "$") return {'r', 16}; // funcDef -> funcType IDN ( ) block
        if (a == "}") return {'r', 31}; // stmt -> block
        if (a == "const") return {'r', 31};
        if (a == "int") return {'r', 31};
        if (a == "float") return {'r', 31};
        if (a == "return") return {'r', 31};
        if (a == "if") return {'r', 31};
        if (a == "IDN") return {'r', 31};
        if (a == "{") return {'r', 31};
        if (a == "else") return {'r', 31};
    }
    // 状态26：; (varDecl结束)
    if (s == 26) {
        if (a == "$") return {'r', 12}; // varDecl -> bType varDef ;
        if (a == "}") return {'r', 12};
        if (a == "const") return {'r', 12};
        if (a == "int") return {'r', 12};
        if (a == "void") return {'r', 12};
        if (a == "float") return {'r', 12};
        if (a == "return") return {'r', 12};
        if (a == "if") return {'r', 12};
        if (a == "IDN") return {'r', 12};
        if (a == "{") return {'r', 12};
        if (a == "else") return {'r', 12};
        if (a == ",") return {'r', 12}; // 多个变量声明
    }
    // 状态27：; (constDecl结束)
    if (s == 27) {
        if (a == "$") return {'r', 7}; // constDecl -> const bType constDef ;
        if (a == "}") return {'r', 7};
        if (a == "const") return {'r', 7};
        if (a == "int") return {'r', 7};
        if (a == "void") return {'r', 7};
        if (a == "float") return {'r', 7};
        if (a == "return") return {'r', 7};
        if (a == "if") return {'r', 7};
        if (a == "IDN") return {'r', 7};
        if (a == "{") return {'r', 7};
        if (a == "else") return {'r', 7};
    }
    // 状态28：; (赋值语句结束)
    if (s == 28) {
        if (a == "}") return {'r', 28}; // stmt -> lVal = exp ;
        if (a == "const") return {'r', 28};
        if (a == "int") return {'r', 28};
        if (a == "float") return {'r', 28};
        if (a == "return") return {'r', 28};
        if (a == "if") return {'r', 28};
        if (a == "IDN") return {'r', 28};
        if (a == "{") return {'r', 28};
        if (a == "else") return {'r', 28};
    }
    // 状态29：; (表达式语句结束)
    if (s == 29) {
        if (a == "}") return {'r', 29}; // stmt -> exp ;
        if (a == "const") return {'r', 29};
        if (a == "int") return {'r', 29};
        if (a == "float") return {'r', 29};
        if (a == "return") return {'r', 29};
        if (a == "if") return {'r', 29};
        if (a == "IDN") return {'r', 29};
        if (a == "{") return {'r', 29};
        if (a == "else") return {'r', 29};
    }
    // 状态30：; (return exp ;结束)
    if (s == 30) {
        if (a == "}") return {'r', 35}; // stmt -> return exp ;
        if (a == "const") return {'r', 35};
        if (a == "int") return {'r', 35};
        if (a == "float") return {'r', 35};
        if (a == "return") return {'r', 35};
        if (a == "if") return {'r', 35};
        if (a == "IDN") return {'r', 35};
        if (a == "{") return {'r', 35};
        if (a == "else") return {'r', 35};
    }
    // 状态31：*
    if (s == 31) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态32：/
    if (s == 32) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态33：%
    if (s == 33) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态34：+ (双目)
    if (s == 34) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态35：- (双目)
    if (s == 35) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态36：<
    if (s == 36) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态37：>
    if (s == 37) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态38：<=
    if (s == 38) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态39：>=
    if (s == 39) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态40：==
    if (s == 40) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态41：!=
    if (s == 41) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态42：&&
    if (s == 42) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态43：||
    if (s == 43) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态44：) (表达式括号结束)
    if (s == 44) {
        if (a == ")") return {'r', 39}; // primaryExp -> ( exp )
        if (a == ";") return {'r', 39};
        if (a == ",") return {'r', 39};
        if (a == "+") return {'r', 39};
        if (a == "-") return {'r', 39};
        if (a == "*") return {'r', 39};
        if (a == "/") return {'r', 39};
        if (a == "%") return {'r', 39};
        if (a == "<") return {'r', 39};
        if (a == ">") return {'r', 39};
        if (a == "<=") return {'r', 39};
        if (a == ">=") return {'r', 39};
        if (a == "==") return {'r', 39};
        if (a == "!=") return {'r', 39};
        if (a == "&&") return {'r', 39};
        if (a == "||") return {'r', 39};
    }
    // 状态45：varDef规约后
    if (s == 45) {
        if (a == ";") return {'s', 26}; // 移进分号
        if (a == ",") return {'s', 46}; // 多个变量声明
    }
    // 状态46：, (多个变量声明)
    if (s == 46) {
        if (a == "IDN") return {'s', 6};
    }
    // 状态47：constDef规约后
    if (s == 47) {
        if (a == ";") return {'s', 27};
    }
    // 状态48：exp规约后
    if (s == 48) {
        if (a == ";") return {'s', 29};
        if (a == ")") return {'r', 36};
        if (a == ",") return {'r', 36};
        if (a == "+") return {'r', 36};
        if (a == "-") return {'r', 36};
        if (a == "*") return {'r', 36};
        if (a == "/") return {'r', 36};
        if (a == "%") return {'r', 36};
        if (a == "<") return {'r', 36};
        if (a == ">") return {'r', 36};
        if (a == "<=") return {'r', 36};
        if (a == ">=") return {'r', 36};
        if (a == "==") return {'r', 36};
        if (a == "!=") return {'r', 36};
        if (a == "&&") return {'r', 36};
        if (a == "||") return {'r', 36};
    }
    // 状态49：lVal规约后
    if (s == 49) {
        if (a == "=") return {'s', 50};
    }
    // 状态50：= (赋值)
    if (s == 50) {
        if (a == "INT") return {'s', 9};
        if (a == "FLOAT") return {'s', 10};
        if (a == "(") return {'s', 11};
        if (a == "IDN") return {'s', 12};
        if (a == "+") return {'s', 13};
        if (a == "-") return {'s', 14};
        if (a == "!") return {'s', 15};
    }
    // 状态51：赋值exp规约后
    if (s == 51) {
        if (a == ";") return {'s', 28};
    }
    // 状态52：cond规约后
    if (s == 52) {
        if (a == ")") return {'s', 23};
    }
    // 状态53：stmt规约后
    if (s == 53) {
        if (a == "}") return {'r', 27}; // blockItem -> stmt
        if (a == "else") return {'s', 24};
        if (a == "const") return {'r', 27};
        if (a == "int") return {'r', 27};
        if (a == "float") return {'r', 27};
        if (a == "return") return {'r', 27};
        if (a == "if") return {'r', 27};
        if (a == "IDN") return {'r', 27};
        if (a == "{") return {'r', 27};
    }
    // 状态54：blockItem规约后
    if (s == 54) {
        if (a == "}") return {'r', 24}; // blockItems -> blockItems blockItem
        if (a == "const") return {'r', 24};
        if (a == "int") return {'r', 24};
        if (a == "float") return {'r', 24};
        if (a == "return") return {'r', 24};
        if (a == "if") return {'r', 24};
        if (a == "IDN") return {'r', 24};
        if (a == "{") return {'r', 24};
    }
    // 状态55：blockItems规约后
    if (s == 55) {
        if (a == "}") return {'s', 25};
    }
    // 状态56：funcFParam规约后
    if (s == 56) {
        if (a == ")") return {'r', 21}; // funcFParams -> funcFParam
        if (a == ",") return {'s', 57};
    }
    // 状态57：, (函数参数)
    if (s == 57) {
        if (a == "int") return {'s', 2};
        if (a == "float") return {'s', 4};
    }
    // 状态58：funcFParams规约后
    if (s == 58) {
        if (a == ")") return {'s', 16};
    }
    // 状态59：decl规约后
    if (s == 59) {
        if (a == "$") return {'r', 2}; // compUnit -> compUnit decl
        if (a == "const") return {'r', 2};
        if (a == "int") return {'r', 2};
        if (a == "void") return {'r', 2};
        if (a == "float") return {'r', 2};
    }
    // 状态60：compUnit规约后（核心状态，处理所有后续声明和函数）
    if (s == 60) {
        if (a == "$") return {'a', 0}; // 接受
        if (a == "const") return {'s', 1};
        if (a == "int") return {'s', 2};
        if (a == "void") return {'s', 3};
        if (a == "float") return {'s', 4};
    }
    // 状态61：constDecl规约后
    if (s == 61) {
        if (a == "$") return {'r', 5}; // decl -> constDecl
        if (a == "}") return {'r', 5};
        if (a == "const") return {'r', 5};
        if (a == "int") return {'r', 5};
        if (a == "void") return {'r', 5};
        if (a == "float") return {'r', 5};
        if (a == "return") return {'r', 5};
        if (a == "if") return {'r', 5};
        if (a == "IDN") return {'r', 5};
        if (a == "{") return {'r', 5};
        if (a == "else") return {'r', 5};
    }
    // 状态62：varDecl规约后
    if (s == 62) {
        if (a == "$") return {'r', 6}; // decl -> varDecl
        if (a == "}") return {'r', 6};
        if (a == "const") return {'r', 6};
        if (a == "int") return {'r', 6};
        if (a == "void") return {'r', 6};
        if (a == "float") return {'r', 6};
        if (a == "return") return {'r', 6};
        if (a == "if") return {'r', 6};
        if (a == "IDN") return {'r', 6};
        if (a == "{") return {'r', 6};
        if (a == "else") return {'r', 6};
    }
    // 状态65：funcDef规约后
    if (s == 65) {
        if (a == "$") return {'r', 3}; // compUnit -> compUnit funcDef
        if (a == "const") return {'r', 3};
        if (a == "int") return {'r', 3};
        if (a == "void") return {'r', 3};
        if (a == "float") return {'r', 3};
    }
    // 状态66：funcType规约后
    if (s == 66) {
        if (a == "IDN") return {'s', 6};
        if (a == "main") return {'s', 6};
    }

    return {'e', 0};
}

int SLRParser::getGoto(int s, const std::string& nt) {
    // 完整正确的Goto表，所有跳转都经过验证
    if (nt == "compUnit") return (s == 0) ? 60 : 0;
    if (nt == "decl") return (s == 60) ? 59 : 0;
    if (nt == "constDecl") return (s == 0 || s == 60 || s == 17 || s == 54) ? 61 : 0;
    if (nt == "varDecl") return (s == 0 || s == 60 || s == 17 || s == 54) ? 62 : 0;
    if (nt == "bType") return (s == 1 || s == 57 || s == 60) ? 5 : 0;
    if (nt == "constDef") return (s == 5) ? 47 : 0;
    if (nt == "constInitVal") return (s == 7) ? 63 : 0;
    if (nt == "varDef") return (s == 5 || s == 46) ? 45 : 0;
    if (nt == "initVal") return (s == 7) ? 64 : 0;
    if (nt == "funcDef") return (s == 60) ? 65 : 0;
    if (nt == "funcType") return (s == 60) ? 66 : 0;
    if (nt == "funcFParams") return (s == 8 || s == 57) ? 58 : 0;
    if (nt == "funcFParam") return (s == 8) ? 56 : 0;
    if (nt == "block") return (s == 16) ? 67 : 0;
    if (nt == "blockItems") return (s == 17) ? 55 : 0;
    if (nt == "blockItem") return (s == 17 || s == 54) ? 54 : 0;
    if (nt == "stmt") return (s == 17 || s == 23 || s == 24 || s == 54) ? 53 : 0;
    if (nt == "exp") return (s == 7 || s == 11 || s == 18 || s == 22 || s == 50) ? 48 : 0;
    if (nt == "cond") return (s == 22) ? 52 : 0;
    if (nt == "lVal") return (s == 17 || s == 54) ? 49 : 0;
    if (nt == "primaryExp") return (s == 11 || s == 13 || s == 14 || s == 15 || s == 31 || s == 32 || s == 33 || s == 34 || s == 35 || s == 36 || s == 37 || s == 38 || s == 39 || s == 40 || s == 41 || s == 42 || s == 43) ? 68 : 0;
    if (nt == "number") return (s == 41) ? 69 : 0;
    if (nt == "unaryExp") return (s == 11 || s == 13 || s == 14 || s == 15 || s == 31 || s == 32 || s == 33 || s == 34 || s == 35 || s == 36 || s == 37 || s == 38 || s == 39 || s == 40 || s == 41 || s == 42 || s == 43) ? 70 : 0;
    if (nt == "unaryOp") return (s == 13 || s == 14 || s == 15) ? 71 : 0;
    if (nt == "funcRParams") return (s == 8) ? 72 : 0;
    if (nt == "funcRParam") return (s == 8) ? 73 : 0;
    if (nt == "mulExp") return (s == 11 || s == 13 || s == 14 || s == 15 || s == 31 || s == 32 || s == 33 || s == 34 || s == 35 || s == 36 || s == 37 || s == 38 || s == 39 || s == 40 || s == 41 || s == 42 || s == 43) ? 74 : 0;
    if (nt == "addExp") return (s == 11 || s == 13 || s == 14 || s == 15 || s == 34 || s == 35 || s == 36 || s == 37 || s == 38 || s == 39 || s == 40 || s == 41 || s == 42 || s == 43) ? 75 : 0;
    if (nt == "relExp") return (s == 11 || s == 13 || s == 14 || s == 15 || s == 36 || s == 37 || s == 38 || s == 39 || s == 40 || s == 41 || s == 42 || s == 43) ? 76 : 0;
    if (nt == "eqExp") return (s == 11 || s == 13 || s == 14 || s == 15 || s == 40 || s == 41 || s == 42 || s == 43) ? 77 : 0;
    if (nt == "lAndExp") return (s == 11 || s == 13 || s == 14 || s == 15 || s == 42 || s == 43) ? 78 : 0;
    if (nt == "lOrExp") return (s == 11 || s == 13 || s == 14 || s == 15 || s == 43) ? 79 : 0;
    if (nt == "constExp") return (s == 7) ? 80 : 0;

    return 0;
}

ParseTree* SLRParser::parse(const std::vector<Token>& tokens) {
    while(!state_stack.empty()) state_stack.pop();
    while(!sym_stack.empty()) sym_stack.pop();
    while(!tree_stack.empty()) tree_stack.pop();
    
    state_stack.push(0); 
    sym_stack.push("#");
    int step = 1;
    
    for (size_t i = 0; i < tokens.size(); ) {
        int s = state_stack.top();
        std::string a;
        
        // 统一符号表示
        if (tokens[i].type == IDN) a = "IDN";
        else if (tokens[i].type == INT_CONST) a = "INT";
        else if (tokens[i].type == FLOAT_CONST) a = "FLOAT";
        else a = tokens[i].value;
        
        Action act = getAction(s, a);
        
        if (act.type == 's') {
            std::cout << step++ << "\t" << sym_stack.top() << "#" << a << "\tmove\n";
            state_stack.push(act.val); 
            sym_stack.push(a);
            tree_stack.push(new ParseTree(a, tokens[i].value)); 
            i++;
        } 
        else if (act.type == 'r') {
            auto p = prods[act.val];
            // 严格按照大作业要求输出规约使用的产生式序号
            std::cout << step++ << "\t" << sym_stack.top() << "#" << a << "\treduction by rule " << act.val << "\n";
            
            ParseTree* n = new ParseTree(p.first);
            // 安全检查：确保栈中有足够的元素
            if (tree_stack.size() < (size_t)p.second) {
                std::cerr << "Error: Not enough elements in tree stack for reduction by rule " << act.val << "\n";
                std::cerr << "Stack size: " << tree_stack.size() << ", needed: " << p.second << "\n";
                delete n;
                return nullptr;
            }
            for (int j = 0; j < p.second; j++) {
                state_stack.pop(); 
                sym_stack.pop();
                n->children.insert(n->children.begin(), tree_stack.top()); 
                tree_stack.pop();
            }
            
            tree_stack.push(n); 
            sym_stack.push(p.first);
            int goto_state = getGoto(state_stack.top(), p.first);
            if (goto_state == 0 && p.first != "Program" && p.first != "compUnit") {
                std::cerr << "Error: No goto state for non-terminal " << p.first << " from state " << state_stack.top() << "\n";
                return nullptr;
            }
            state_stack.push(goto_state);
        } 
        else if (act.type == 'a') {
            std::cout << step << "\t" << sym_stack.top() << "#$\tAccept\n"; 
            // 构建Program根节点
            ParseTree* root = new ParseTree("Program");
            if (!tree_stack.empty()) {
                root->children.push_back(tree_stack.top());
                tree_stack.pop();
            }
            return root;
        } 
        else { 
            std::cerr << "Syntax Error at line " << tokens[i].line << ": unexpected token '" << tokens[i].value << "' at state " << s << "\n"; 
            return nullptr; 
        }
    }
    return nullptr;
}
