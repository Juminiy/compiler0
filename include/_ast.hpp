#pragma once
#ifndef _AST_HPP
#define _AST_HPP

#include <memory>
#include <string>
#include <iostream>

#include "_alib.hpp"

#define __decl_ast_tid__ \
        public: __CLASS_UNIQUE__ size_t __ast_tid_
#define __def_ast_tid__(__class__, __id__) \
        const size_t __class__::__ast_tid_(__id__)
#define __override_ast_tid__ \
        size_t tid() const override { return __ast_tid_; }

class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void log(std::ostream &) const = 0;
  virtual size_t tid() const = 0;
};

using ast_uptr = std::unique_ptr<BaseAST>;

class CompUnitAST : public BaseAST {
 public:
  ast_uptr func_def;
  
  friend std::ostream &
    operator << (std::ostream & __os, const CompUnitAST & _a)
    {
        __os << "CompUnit { ";
            _a.func_def->log(__os);
        __os << " }"
            << __LN__;
        return __os;
    }
  void log(std::ostream & __os) const override{
    __os << *this;
  }
__decl_ast_tid__;
__override_ast_tid__
};

class FuncDefAST : public BaseAST {
 public:
  ast_uptr func_type;
  std::unique_ptr<std::string> ident;
  ast_uptr block;
  friend std::ostream& 
    operator << (std::ostream& __os, const FuncDefAST & _a)
    {
        __os << "FuncDef { ";
            _a.func_type->log(__os);
        __os << ", " << *_a.ident << ", ";
            _a.block->log(__os);
        __os << " }";
        return __os;
    }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
__decl_ast_tid__;
__override_ast_tid__
};

class FuncTypeAST : public BaseAST {
public:
    std::unique_ptr<std::string> __type_;

    friend std::ostream& 
        operator << (std::ostream & __os, const FuncTypeAST &_a)
        {
            __os << "FuncType { "
                << *_a.__type_
                << " }";
            return __os;
        }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
__decl_ast_tid__;
__override_ast_tid__
};

class BlockAST : public BaseAST {
public:
    ast_uptr __stmt_;

    friend std::ostream& 
        operator << (std::ostream & __os, const BlockAST &_a)
        {
            __os << "Block { ";
                _a.__stmt_->log(__os);
            __os << " }";
            return __os;
        }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
__decl_ast_tid__;
__override_ast_tid__
};

class StmtAST : public BaseAST {
public:
    std::unique_ptr<std::string> __ret_;
    ast_uptr __expr_;

    friend std::ostream& 
        operator << (std::ostream & __os, const StmtAST &_a)
        {
            __os << "Stmt { ";
                _a.__expr_->log(__os);
            __os << " }";
            return __os;
        }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
__decl_ast_tid__;
__override_ast_tid__
};

class ExprAST : public BaseAST {
public:
    ast_uptr __unary_expr_;

    __DEF_ALL__(expr, ExprAST);

    __FRIEND_OS_OPT__(ExprAST, _a)
    {
        __os << "Expr { ";
            _a.__unary_expr_->log(__os);
        __os << " }";
        return __os;
    }

    __MEMBER_FUNC_LOG_OVERRIDE__
__decl_ast_tid__;
__override_ast_tid__
};

class PrimaryExprAST : public BaseAST {
public:
    #define __PriExpr_Expr 0
    ast_uptr __expr_;

    #define __PriExpr_Num 1
    ast_uptr __number_;
    
    // 0 | 1
    int __sub_expr_type_;
    
    __DEF_ALL__(pexpr, PrimaryExprAST);

    __FRIEND_OS_OPT__(PrimaryExprAST, _a)
    {
        __os << "PrimaryExpr { ";
        switch (_a.__sub_expr_type_)
        {
        case __PriExpr_Expr:
        __os << " ( ";
            _a.__expr_->log(__os);
        __os << " ) ";
            break;
        case __PriExpr_Num:
            _a.__number_->log(__os);
            break;
        default:
            break;
        }
        __os << " }";
        return __os;
    }

    __MEMBER_FUNC_LOG_OVERRIDE__
__decl_ast_tid__;
__override_ast_tid__
};

class NumberAST : public BaseAST {
public:
    int __int_const_;
    friend std::ostream& 
        operator << (std::ostream & __os, const NumberAST &_a)
        {
            __os << "Number { "
                << _a.__int_const_
                << " }";
            return __os;
        }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
__decl_ast_tid__;
__override_ast_tid__
};

class UnaryExpAST : public BaseAST {
public:
    #define __UnaExpr_Pri 0
    ast_uptr __primary_expr_;

    #define __UnaExpr_Una 1
    ast_uptr __unary_op_;
    ast_uptr __unary_expr_;

    // 0 | 1
    int __sub_expr_type_;

    __DEF_ALL__(uexpr, UnaryExpAST);

    __FRIEND_OS_OPT__(UnaryExpAST, _a)
    {
        __os << "UnaryExpr { ";
        switch (_a.__sub_expr_type_)
        {
        case __UnaExpr_Pri:
            _a.__primary_expr_->log(__os);
            break;
        case __UnaExpr_Una:
            _a.__unary_op_->log(__os);
            _a.__unary_expr_->log(__os);
            break;
        default:
            break;
        }
        __os << " }";
        return __os;
    }

    __MEMBER_FUNC_LOG_OVERRIDE__
__decl_ast_tid__;
__override_ast_tid__
};

class UnaryOpAST : public BaseAST {
public:
    // +
    #define __UnaOp_Plus 0
    // -
    #define __UnaOp_Minus 1
    // !
    #define __UnaOp_Not 2
    explicit UnaryOpAST(int __opt_) 
        noexcept {
            switch (__opt_)
            {
            case __UnaOp_Plus:
                this->__operator_ = '+';
                break;
            case __UnaOp_Minus:
                this->__operator_ = '-';
                break;
            case __UnaOp_Not:
                this->__operator_ = '!';
                break;
            default:
                break;
            }
        }
    char __operator_;

    __DEF_CLASS_FULL__(
        uop,                    // __class_alias__
        UnaryOpAST,             // __class__
        _a,                     // __var_id__
        __os << "UnaryOp { "    // __os_actions__ 
            << _a.__operator_   // ```
            << " }";,            // ```
        /* __is_actions__  */,  
        /* __assign_actions__ */ 
    )
__decl_ast_tid__;
__override_ast_tid__
};

// ...


#endif 