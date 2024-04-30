#pragma once
#ifndef _IR_HPP
#define _IR_HPP

#include <memory>
#include <string>
#include <ostream>
#include <vector> 
#include <string>
#include <algorithm>
#include <utility>
#include <stack>

#include <cctype>
#include <cassert>

#include "_alib.hpp"
#include "_ast.hpp"
#include "_ptrlib.hpp"

#include "koopa.h"

__DEF_NS__(Alan)

class BaseIR 
{
public:
    virtual ~BaseIR() = default;
    virtual void log(std::ostream &) const = 0;
};  

using ValueKind = koopa_raw_value_tag_t;

class ValueIR : public BaseIR
{
public:
    explicit ValueIR(const std::string & _val,
                    ValueKind _kind = KOOPA_RVT_UNDEF)
        : __val_(std::make_unique<std::string>
                            (std::move(_val))),
            __kind_(_kind) 
        { }
    
    __DEF_ALL__(vir, ValueIR);

    __MV_ASSI__(vir, 
        this->__val_ = std::move(_c.__val_);
        this->__kind_ = _c.__kind_;
    )

    __MV_SMTC__(vir, ValueIR)

    __FRIEND_OS_OPT__(vir, _ir)
    {   
        __os << *(_ir.__val_) 
            << __LN__;
        return __os;
    }
    
    __MEMBER_FUNC_LOG_DECL__;

public:
    std::unique_ptr<std::string> __val_;
    ValueKind __kind_;
};

class BasicBlockIR : public BaseIR
{
public:
    __CLASS_GLB_FN__ 
    std::string _conv_stmt(const std::string &_stmt)
    {
        if (_stmt == "return")
            return std::string("ret");
        return "__block_";
    }    
    __CLASS_GLB_FN__ 
    std::string _make_tmp(int _id, const std::string &_prf = "%")
    {
        return _prf + std::to_string(_id);
    }
    __CLASS_UNIQUE__ std::string __unkown_tmp_id_;
    __CLASS_UNIQUE__ std::string __named_id_prefix_;
    __CLASS_UNIQUE__ std::string __unamed_id_prefix_;
public:
    int __tmp_id_;
    std::stack<std::string> __tmp_opd_stk_;
    std::stack<std::string> __tmp_opt_stk_;
    std::stack<std::string> __tmp_ass_stk_;
#define __Unary_Lval "0"
public:
    void traverseExpr(ast_uptr _expr);
public:
    explicit BasicBlockIR(BlockAST & _bast)
        : __block_ast_(std::make_unique<BlockAST>
                                    (std::move(_bast)))
        { 
            auto stmtPtr = Alan::static_uptr_cast<StmtAST, BaseAST>
                            (__block_ast_->__stmt_);
            auto exprPtr = Alan::static_uptr_cast<ExprAST, BaseAST>
                            (stmtPtr->__expr_);
            
            // I wanna try to optimize
            // start recursive
            this->__tmp_id_ = 0;
            
            traverseExpr(std::move(exprPtr));

            __instructions_.push_back(
            std::make_unique<ValueIR>(
                "\t" + _conv_stmt(*(stmtPtr->__ret_)) + 
                " " + __tmp_opd_stk_.top()
                ,
                KOOPA_RVT_INTEGER
            )
        );
        }

    __DEF_ALL__(bbir, BasicBlockIR);

    __MV_ASSI__(bbir, 
        this->__block_ast_ = std::move(_c.__block_ast_);
        this->__instructions_ = std::move(_c.__instructions_);
    )

    __MV_SMTC__(bbir, BasicBlockIR)

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
    
    /// @brief convert value type from ast to ir 
    /// @param _ast_type 
    /// @return 
    static std::string _type_conv 
    (const std::string &_ast_type)
    {
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
                    Alan::static_uptr_cast<BlockAST, BaseAST>
                                    (__func_def_ast_->block))
                )
            );
            this->__fn_name_ = *(__func_def_ast_->ident);
            // convert ast_type to ir_type
            this->__fn_type_ = _type_conv(
                *(Alan::static_uptr_cast<FuncTypeAST, BaseAST>
                    (this->__func_def_ast_->func_type)->__type_)
            );
        }
    
    __DEF_ALL__(fir, FuncIR);
    
    __FRIEND_OS_OPT__(fir, _ir)
    {
        __os << "fun " 
            << "@" 
            << _ir.__fn_name_
            << "()" 
            << ": " 
            << _ir.__fn_type_;
        
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
    std::string __fn_name_;
    std::string __fn_type_;
private:
    std::unique_ptr<FuncDefAST> __func_def_ast_;
};

class ProgramIR : public BaseIR
{
public:
    explicit ProgramIR(CompUnitAST & _cuast)
    {
        // this->__global_vars_.push_back(
        //     std::make_unique<ValueIR>(*
        //         (Alan::static_uptr_cast<, BaseAST>
        //                                 ())
        //     )
        // );
        this->__funcs_.push_back(
            std::make_unique<FuncIR>(*
                (Alan::static_uptr_cast<FuncDefAST, BaseAST>
                                        (_cuast.func_def))
            )
        );
    }
    __DEF_ALL__(pir, ProgramIR);

    __MV_ASSI__(pir, 
        this->__global_vars_ = std::move(_c.__global_vars_);
        this->__funcs_ = std::move(_c.__funcs_);
    )

    __MV_SMTC__(pir, ProgramIR)
    

    __FRIEND_OS_OPT__(pir, _ir)
    {
        // ir global variables
        for(auto & _gvar : _ir.__global_vars_)
        {
            _gvar->log(__os);
        }

        __os << __LN__;

        // ir functions
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