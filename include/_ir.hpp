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
#include <unordered_map>

#include <cctype>
#include <cassert>

#include "_alib.hpp"
#include "_ast.hpp"
#include "_stllib.hpp"

#include "koopa.h"

__DEF_NS__(Alan)

class BaseIR 
{
public:
    virtual ~BaseIR() = default;
    virtual void log(std::ostream &) const = 0;
};  

using IRKind = koopa_raw_value_tag_t;
using IRCate = size_t;

class ValueIR : public BaseIR
{
public:
    explicit ValueIR(const std::string & _val,
                    IRKind _kind = KOOPA_RVT_UNDEF)
        : __val_(__make_str_(std::move(_val))),
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
    
    __MEMBER_FUNC_LOG_OVERRIDE__

public:
    str_uptr __val_;
    IRKind __kind_;
    IRCate __cate_;

};

class BasicBlockIR : public BaseIR
{
public:
    __CLASS_UNIQUE__ std::string __unkown_tmp_id_;
    __CLASS_UNIQUE__ std::string __named_id_prefix_;
    __CLASS_UNIQUE__ std::string __unamed_id_prefix_;

    __CLASS_GLB_VA__ std::unordered_map<std::string, int> __opt_priority_;

public:
    __CLASS_GLB_FN__ 
    std::string __conv_keyword_(const std::string &);

    __CLASS_GLB_FN__ 
    std::string _make_tmp(int , const std::string & = "%");

    __CLASS_GLB_FN__ 
    bool __is_int_(const std::string &);

    __CLASS_GLB_FN__
    bool __is_opt_(const std::string &);

    __CLASS_GLB_FN__
    void __init_opt_priority_();

    __CLASS_GLB_FN__
    bool __opt_less_(const std::string & ,const std::string & );

    __CLASS_GLB_FN__
    bool __is_const_expr_(const std::string &);

    __CLASS_GLB_FN__
    int __i32_opt_res_(int, std::string, int);

public:
    std::deque<std::string> __mid_op_stk_; // 
    std::deque<std::string> __rpn_op_stk_; // operand and operator of RPN 
    std::deque<std::string> __tmp_opt_stk_; // tmp operator
    // std::stack<std::string> __tmp_ass_stk_; // assign operand
    int __tmp_id_;  // tmp assigned val
    
    #define __bbir_ret_val 0
    #define __bbir_ass_val 1
    void __push_val_
    (const std::vector<std::string> & , int = __bbir_ret_val);

    #define __Unary_Lval "0"
    // mid order traverse
    void traverseExpr(ast_uptr _expr);
    // mid expr to RPN
    void midExpr2RPN();
    // eval PRN to optimize
    void evalRPN();

public:
    explicit BasicBlockIR(BlockAST & _bast);

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
    
    __MEMBER_FUNC_LOG_OVERRIDE__

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

    __MEMBER_FUNC_LOG_OVERRIDE__

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

    __MEMBER_FUNC_LOG_OVERRIDE__

public:
    std::vector<std::unique_ptr<ValueIR>> __global_vars_;
    std::vector<std::unique_ptr<FuncIR>> __funcs_;

};


__END_NS__

#endif