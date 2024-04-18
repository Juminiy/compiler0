#pragma once
#ifndef _IR_HPP
#define _IR_HPP

#include <memory>
#include <string>
#include <ostream>
#include <vector> 

#include "_alib.hpp"
#include "_ast.hpp"
#include "_ptrlib.hpp"

__DEF_NS__(Alan)

class BaseIR 
{
public:
    virtual ~BaseIR() = default;
    virtual void log(std::ostream &) const = 0;
};  

class ValueIR : public BaseIR
{
public:
    explicit ValueIR(const std::string & _val)
        : __val_(std::make_unique<std::string>
                            (std::move(_val))) 
        { }
    __DEF_ALL__(vir, ValueIR);

    __FRIEND_OS_OPT__(vir, _ir)
    {   
        __os << *(_ir.__val_) 
            << __LN__;
        return __os;
    }
    
    __MEMBER_FUNC_LOG_DECL__;
    
public:
    std::unique_ptr<std::string> __val_;
};

class BasicBlockIR : public BaseIR
{
public:
    std::string _conv_stmt(const std::string &_stmt)
    {
        if (_stmt == "return")
            return std::string("ret");
        return "__stmt_";
    }    
public:
    explicit BasicBlockIR(BlockAST & _bast)
        : __block_ast_(std::make_unique<BlockAST>
                                    (std::move(_bast)))
        { 
            auto stmtPtr = Alan::dynamic_uptr_cast<StmtAST, BaseAST>
                            (__block_ast_->__stmt_);
            __instructions_.push_back(
                std::make_unique<ValueIR>(
                    "\t" +
                    _conv_stmt(*(stmtPtr->__ret_)) + 
                    " " + 
                    std::to_string(
                        Alan::dynamic_uptr_cast<NumberAST, BaseAST>
                            (stmtPtr->__number_)->__int_const_
                    )
                )
            );
        }

    __DEF_ALL__(bbir, BasicBlockIR);

    __FRIEND_OS_OPT__(bbir, _ir)
    {   
        for (auto & _ins : _ir.__instructions_)
        {
            _ins->log(__os);
        }
        return __os;
    }
    
    __MEMBER_FUNC_LOG_DECL__;

public:
    std::vector<std::unique_ptr<ValueIR>> __instructions_;
private:
    std::unique_ptr<BlockAST> __block_ast_;
};

class FuncIR : public BaseIR
{
public:
    static std::string 
    _type_conv
        (const std::string &_ast_type){
        if(_ast_type == "int")
            return std::string("i32");
        
        return "any";
    }
public:
    explicit FuncIR(FuncDefAST & _fast)
        : __func_def_ast_(std::make_unique<FuncDefAST>
                                        (std::move(_fast)))
        { 
            this->__basic_blocks_.push_back(
                std::make_unique<BasicBlockIR>(*(
                    Alan::dynamic_uptr_cast<BlockAST, BaseAST>
                                    (__func_def_ast_->block))
                )
            );
        }
    
    __DEF_ALL__(fir, FuncIR);
    
    __FRIEND_OS_OPT__(fir, _ir)
    {
        __os << "fun " 
            << "@" 
            << *(_ir.__func_def_ast_->ident)
            << "()" 
            << ": ";
            // TODO: construct and 
            //          convert ast_type to ir_type
        __os << _type_conv(
            *(Alan::dynamic_uptr_cast<FuncTypeAST, BaseAST>
                (_ir.__func_def_ast_->func_type)->__type_)
            );
        __os << " {" << __LN__;
        __os << "%entry:" << __LN__;
        for(auto & _bb : _ir.__basic_blocks_)
        {
            _bb->log(__os);
            __os << __LN__;
        }
        __os << "}" << __LN__;
        return __os;
    }

    __MEMBER_FUNC_LOG_DECL__;

public:
    std::vector<std::unique_ptr<BasicBlockIR>> __basic_blocks_;
private:
    std::unique_ptr<FuncDefAST> __func_def_ast_;
};

class ProgramIR : public BaseIR
{
public:
    explicit ProgramIR(CompUnitAST & _cuast)
    {
        this->__funcs_.push_back(
            std::make_unique<FuncIR>(*
                (Alan::dynamic_uptr_cast<FuncDefAST, BaseAST>
                                        (_cuast.func_def))
            )
        );
    }
    __DEF_ALL__(pir, ProgramIR);

    __FRIEND_OS_OPT__(pir, _ir)
    {
        for(auto & _gvar : _ir.__global_vars_)
        {
            _gvar->log(__os);
        }
        __os << __LN__;
        for (auto & _func : _ir.__funcs_)
        {
            _func->log(__os);
        }
        return __os;
    }

    __MEMBER_FUNC_LOG_DECL__;

public:
    std::vector<std::unique_ptr<ValueIR>> __global_vars_;
    std::vector<std::unique_ptr<FuncIR>> __funcs_;

};


__END_NS__

#endif