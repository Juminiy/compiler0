#include <cassert>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <ios>
#include <utility>

#include "../include/_alib.hpp"
#include "../include/_ast.hpp"
#include "../include/_ir.hpp"
#include "../include/_ptrlib.hpp"
#include "../include/_rv64i.hpp"

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern int yyparse(std::unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {

    // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
    // compiler 模式 输入文件 -o 输出文件
    assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    // omit argv[3] = -o
    auto output = argv[4];
    auto output_v2 = parseGetOutputByInput(input);
    // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
    yyin = fopen(input, "r");
    assert(yyin);
    int _mode_num = parseMode(mode);
    std::ofstream ast_ofs, ir_ofs, asm_ofs;
    auto _plain_ofs = std::ofstream(output);

    // gen AST
    std::unique_ptr<BaseAST> _ast;
    auto ret = yyparse(_ast);
    assert(!ret);
    if (_mode_num >= MODE_ALL)
        ast_ofs = std::ofstream(__STR_CAT__(output_v2, ".ast"));
    else 
        ast_ofs = std::move(_plain_ofs);
    if (_mode_num == MODE_AST ||
        _mode_num == MODE_ALL)
        _ast->log(ast_ofs);

    // gen IR 
    auto _ir = 
        std::make_unique<Alan::ProgramIR>
        (*Alan::static_uptr_cast<CompUnitAST, BaseAST>(_ast));
    if (_mode_num >= MODE_ALL)
        ir_ofs = std::ofstream(__STR_CAT__(output_v2, ".koopa"));
    else 
        ir_ofs = std::move(_plain_ofs);
    if (_mode_num == MODE_IR ||
        _mode_num == MODE_ALL)
        _ir->log(ir_ofs);

    // gen ASM(RV64I)
    auto _asm = 
        std::make_unique<Alan::RV64::I::ProgramAsmGenerator>(*_ir);
    if (_mode_num >= MODE_ALL)
        asm_ofs = std::ofstream(__STR_CAT__(output_v2, ".s"));
    else 
        asm_ofs = std::move(_plain_ofs);
    if (_mode_num == MODE_ASM ||
        _mode_num == MODE_ALL)
        _asm->log(asm_ofs);

    return 0;
}
