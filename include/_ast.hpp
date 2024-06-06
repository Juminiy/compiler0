#pragma once
#ifndef _AST_HPP
#define _AST_HPP

#include <memory>
#include <string>
#include <iostream>

#include <vector>
#include <map>

#include "_alib.hpp"
#include "_ref.hpp"
#include "_stllib.hpp"

#define __decl_ast_tid__ \
        public: __CLASS_UNIQUE__ size_t __ast_tid_
#define __def_ast_tid__(__class__, __id__) \
        const size_t __class__::__ast_tid_(__id__)
#define __override_ast_tid__ \
        size_t tid() const override { return __ast_tid_; }

#define COMP_UNIT_AST 0
#define FUNC_DEF_AST 1
#define FUNC_TYPE_AST 2
#define BLOCK_AST 3
#define STMT_AST 4
#define EXPR_AST 5
#define PRIMARY_EXPR_AST 6
#define NUMBER_AST 7
#define UNARY_EXPR_AST 8
#define UNARY_OP_AST 9
#define MUL_EXPR_AST 10
#define ADD_EXPR_AST 11
#define BINARY_OP_AST 12
#define REL_EXPR_AST 13
#define EQ_EXPR_AST 14
#define CMP_OP_AST 15
#define LAND_EXPR_AST 16
#define LOR_EXPR_AST 17
#define BLOCK_ITEM_AST 18
#define LVAL_AST 19
#define DECL_AST 20
#define CONST_DECL_AST 21
#define BTYPE_AST 22
#define CONST_DEF_AST 23
#define CONST_INIT_VAL_AST 24
#define CONST_EXPR_AST 25
#define CONST_DEFS_AST 26
#define BLOCK_ITEMS_AST 27

class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void log(std::ostream &) const = 0;
  virtual size_t tid() const = 0;
};

#define __make_ast_(__construct_param_) std::make_unique<BaseAST>(__construct_param_)
using ast_uptr = std::unique_ptr<BaseAST>;
#define __make_str_(__str_val_) std::make_unique<std::string>(__str_val_)
using str_uptr = std::unique_ptr<std::string>;

class CompUnitAST : public BaseAST {
public:
    ast_uptr func_def;
  
    __FRIEND_OS_OPT__(CompUnitAST, _a)
    {
        __os << "CompUnit { ";
            _a.func_def->log(__os);
        __os << "\n}";
        __os << __LN__;
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class FuncDefAST : public BaseAST {
public:
    ast_uptr func_type;
    str_uptr ident;
    ast_uptr block;

    __FRIEND_OS_OPT__(FuncDefAST, _a)
    {
        __os << __LN__;
        __os << "FuncDef { ";
            _a.func_type->log(__os);
        __os << ", " << *_a.ident << ", ";
            _a.block->log(__os);
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class FuncTypeAST : public BaseAST {
public:
    str_uptr __type_;

    __FRIEND_OS_OPT__(FuncTypeAST, _a)
    {
        __os << __LN__;
        __os << "FuncType { "
            << *_a.__type_;
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

using __Sym_Table_t = std::shared_ptr<SymTable>;
#define __decl_sym_table_of __Sym_Table_t __sym_table

class BlockAST : public BaseAST {
public:
    ast_uptr __block_items_;

    __decl_sym_table_of;
    
    // BlockAST()
    // {
    //     __sym_table = std::make_shared<SymTable>();
    // }
    
    __FRIEND_OS_OPT__(BlockAST, _a)
    {
        __os << __LN__;
        __os << "Block { ";
            _a.__block_items_->log(__os);
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class BlockItemsAST : public BaseAST {
public:
    #define __BlockItems_Null 0
    ast_uptr __block_item_;
    #define __BlockItems_Recv 1
    ast_uptr __next_;

    int __sub_type_;

    std::vector<ast_uptr> __block_items_;

    void collect() {
        #define local_cast Alan::static_uptr_cast<BlockItemsAST, BaseAST>
        __block_items_.push_back(std::move(__block_item_));
        ast_uptr cur_raw = std::move(this->__next_);
        while(cur_raw) {
            auto cur = local_cast(cur_raw);
            __block_items_.push_back(std::move(cur));
            cur_raw = std::move(cur->__next_);
        #undef local_cast
        }
    }

    __FRIEND_OS_OPT__(BlockItemsAST, _a)
    {
        __os << __LN__;
        __os << "BlockItems { ";
        
        switch (_a.__sub_type_)
        {
        case __BlockItems_Null:

            break;
        case __BlockItems_Recv:
            for (auto & item : _a.__block_items_){
                item->log(__os);
            }
            break;
        }

        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class BlockItemAST : public BaseAST {
public:
    #define __BlockItem_Decl 0
    ast_uptr __decl_;
    #define __BlockItem_Stmt 1
    ast_uptr __stmt_;
    #define __BlockItem_Null 2
    // nullptr

    // __BlockItem_Decl | __BlockItem_Stmt | __BlockItem_Null
    int __sub_type_;

    __decl_sym_table_of;

    __FRIEND_OS_OPT__(BlockItemAST, _a)
    {
        __os << __LN__;
        __os << "BlockItem { ";
        
        switch (_a.__sub_type_)
        {
        case __BlockItem_Decl:
            _a.__decl_->log(__os);
            break;
        case __BlockItem_Stmt:
            _a.__stmt_->log(__os);
            break;
        }

        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class StmtAST : public BaseAST {
public:
    #define _Stmt_Lval 0
    #define _Stmt_Ret 1
    ast_uptr __lval_;
    str_uptr __ret_;
    ast_uptr __expr_;

    // _Stmt_Lval | _Stmt_Ret
    int __sub_type_;

    __FRIEND_OS_OPT__(StmtAST, _a)
    {
        __os << __LN__;
        __os << "Stmt { ";
            _a.__expr_->log(__os);
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class ExprAST : public BaseAST {
public:
    ast_uptr __lor_expr_;

    __DEF_ALL__(expr, ExprAST);

    __FRIEND_OS_OPT__(ExprAST, _a)
    {
        __os << __LN__;
        __os << "Expr { ";
            _a.__lor_expr_->log(__os);
        __os << "\n}";
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
    #define __PriExpr_LVal 2
    ast_uptr __lval_;

    // __PriExpr_Expr | __PriExpr_Num | __PriExpr_LVal
    int __sub_expr_type_;
    
    __DEF_ALL__(pexpr, PrimaryExprAST);

    __FRIEND_OS_OPT__(PrimaryExprAST, _a)
    {
        __os << __LN__;
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
        case __PriExpr_LVal:
            _a.__lval_->log(__os);
        default:
            break;
        }
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class LValAST : public BaseAST {
public:
    str_uptr __ident_;

    __FRIEND_OS_OPT__(LValAST, _a)
    {
        __os << __LN__;
        __os << "LVal { "
            << *_a.__ident_;
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class NumberAST : public BaseAST {
public:
    int __int_const_;

    __FRIEND_OS_OPT__(NumberAST, _a)
    {
        __os << __LN__;
        __os << "Number { "
            << _a.__int_const_;
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

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

    // __UnaExpr_Pri | __UnaExpr_Una
    int __sub_expr_type_;

    __DEF_ALL__(uexpr, UnaryExpAST);

    __FRIEND_OS_OPT__(UnaryExpAST, _a)
    {
        __os << __LN__;
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
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class UnaryOpAST : public BaseAST {
public:
    // char __operator_;
    str_uptr __operator_;
    #define __UnaOp_Plus 0  // +
    #define __UnaOp_Minus 1 // -
    #define __UnaOp_Not 2   // !

    // explicit UnaryOpAST(int __opt_) 
    //     noexcept {
    //         switch (__opt_)
    //         {
    //         case __UnaOp_Plus:
    //             this->__operator_ = '+';
    //             break;
    //         case __UnaOp_Minus:
    //             this->__operator_ = '-';
    //             break;
    //         case __UnaOp_Not:
    //             this->__operator_ = '!';
    //             break;
    //         default:
    //             break;
    //         }
    //     }

    // __DEF_CLASS_FULL__(
    //     uop,                    // __class_alias__
    //     UnaryOpAST,             // __class__
    //     _a,                     // __var_id__
    //     __os << "UnaryOp { "    // __os_actions__ 
    //         << _a.__operator_   // ```
    //         << " }";,            // ```
    //     /* __is_actions__  */,  
    //     /* __assign_actions__ */ 
    // )
    __FRIEND_OS_OPT__(UnaryOpAST, _a)
    {
        __os << __LN__;
        __os << "UnaryOp { "    
            << *_a.__operator_;
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class MulExpAST : public BaseAST {
public:
    #define __MulExpr_Una 0
    ast_uptr __unary_expr_;
    #define __MulExpr_Mul 1
    ast_uptr __mul_expr_;
    ast_uptr __bin_op_;
    // ast_uptr __unary_expr_;

    // __MulExpr_Una | __MulExpr_Mul
    int __sub_expr_type_;

    __FRIEND_OS_OPT__(MulExpAST, _a)
    {
        __os << __LN__;
        __os << "MulExp { ";
        switch (_a.__sub_expr_type_)
        {
        case __MulExpr_Una:
            _a.__unary_expr_->log(__os);
            break;
        case __MulExpr_Mul:
            _a.__mul_expr_->log(__os);
            _a.__bin_op_->log(__os);
            _a.__unary_expr_->log(__os);
            break;
        default:
            break;
        }
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class AddExpAST : public BaseAST {
public:
    #define __AddExpr_Mul 0
    ast_uptr __mul_expr_;
    #define __AddExpr_Add 1
    ast_uptr __add_expr_;
    ast_uptr __bin_op_;
    // ast_uptr __mul_expr_;

    // __AddExpr_Mul | __AddExpr_Add
    int __sub_expr_type_;

    __FRIEND_OS_OPT__(AddExpAST, _a)
    {
        __os << __LN__;
        __os << "AddExpr { ";
        switch (_a.__sub_expr_type_)
        {
        case __AddExpr_Mul:
            _a.__mul_expr_->log(__os);
            break;
        case __AddExpr_Add:
            _a.__add_expr_->log(__os);
            _a.__bin_op_->log(__os);
            _a.__mul_expr_->log(__os);
            break;
        default:
            break;
        }
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__
};

class BinOpAST : public BaseAST {
public:
    // char __operator_;
    str_uptr __operator_;
    #define __BinOp_Mul 0   // *
    #define __BinOp_Div 1   // /
    #define __BinOp_Mod 2   // %
    #define __BinOp_Plus 3  // +
    #define __BinOp_Minus 4 // -

    // explicit BinOpAST(int __opt_) 
    //     noexcept {
    //         switch (__opt_)
    //         {
    //         case __BinOp_Mul:
    //             this->__operator_ = '*';
    //             break;
    //         case __BinOp_Div:
    //             this->__operator_ = '/';
    //             break;
    //         case __BinOp_Mod:
    //             this->__operator_ = '%';
    //             break;
    //         case __BinOp_Plus:
    //             this->__operator_ = '+';
    //             break;
    //         case __BinOp_Minus:
    //             this->__operator_ = '/';
    //             break;
    //         default:
    //             break;
    //         }
    //     }

    // __DEF_CLASS_FULL__(
    //     bop,                    // __class_alias__
    //     BinOpAST,             // __class__
    //     _a,                     // __var_id__
    //     __os << "BinOp { "    // __os_actions__ 
    //         << _a.__operator_   // ```
    //         << " }";,            // ```
    //     /* __is_actions__  */,  
    //     /* __assign_actions__ */ 
    // )

    __FRIEND_OS_OPT__(BinOpAST, _a)
    {
        __os << __LN__;
        __os << "BinOp { "    
            << *_a.__operator_;
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__    
};

class RelExpAST : public BaseAST {
public:
    #define __RelExpr_Add 0
    ast_uptr __add_expr_;
    #define __RelExpr_Rel 1
    ast_uptr __rel_expr_;
    ast_uptr __cmp_op_;
    // ast_uptr __add_expr_;

    // __RelExpr_Add | __RelExpr_Rel
    int __sub_expr_type_;

    __FRIEND_OS_OPT__(RelExpAST, _a)
    {
        __os << __LN__;
        __os << "RelExpr { ";
        switch (_a.__sub_expr_type_)
        {
        case __RelExpr_Add:
            _a.__add_expr_->log(__os);
            break;
        case __RelExpr_Rel:
            _a.__rel_expr_->log(__os);
            _a.__cmp_op_->log(__os);
            _a.__add_expr_->log(__os);
            break;
        default:
            break;
        }
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__  
};

class EqExpAST : public BaseAST {
public:
    #define __EqExpr_Rel 0
    ast_uptr __rel_expr_;
    #define __EqExpr_Eq 1
    ast_uptr __eq_expr_;
    ast_uptr __cmp_op_;
    // ast_uptr __rel_expr_;

    // __EqExpr_Rel | __EqExpr_Eq
    int __sub_expr_type_;

    __FRIEND_OS_OPT__(EqExpAST, _a)
    {
        __os << __LN__;
        __os << "EqExpr { ";
        switch (_a.__sub_expr_type_)
        {
        case __EqExpr_Rel:
            _a.__rel_expr_->log(__os);
            break;
        case __EqExpr_Eq:
            _a.__eq_expr_->log(__os);
            _a.__cmp_op_->log(__os);
            _a.__rel_expr_->log(__os);
            break;
        default:
            break;
        }
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__  
};

class CmpOpAST : public BaseAST {
public:
    // std::string __operator_;
    str_uptr __operator_;
    #define __CmpOp_Lt 0 // <
    #define __CmpOp_Gt 1 // >
    #define __CmpOp_Le 2 // <=
    #define __CmpOp_Ge 3 // >=
    #define __CmpOp_Eq 4 // ==
    #define __CmpOp_Ne 5 // !=

    // explicit CmpOpAST(std::string * _str_ptr)  
    //     noexcept {
    //         this->__operator_ = str_uptr(_str_ptr);
    //     }

    // explicit CmpOpAST(int __opt_) 
    //     noexcept {
    //         switch (__opt_)
    //         {
    //         case __CmpOp_Lt:
    //             this->__operator_ = "<";
    //             break;
    //         case __CmpOp_Gt:
    //             this->__operator_ = ">";
    //             break;
    //         case __CmpOp_Le:
    //             this->__operator_ = "<=";
    //             break;
    //         case __CmpOp_Ge:
    //             this->__operator_ = ">=";
    //             break;
    //         case __CmpOp_Eq:
    //             this->__operator_ = "==";
    //             break;
    //         case __CmpOp_Ne:
    //             this->__operator_ = "!=";
    //             break;
    //         default:
    //             break;
    //         }
    //     }

    // __DEF_CLASS_FULL__(
    //     cop,                    // __class_alias__
    //     CmpOpAST,             // __class__
    //     _a,                     // __var_id__
    //     __os << "CmpOp { "    // __os_actions__ 
    //         << *_a.__operator_   // ```
    //         << " }";,            // ```
    //     /* __is_actions__  */,  
    //     /* __assign_actions__ */ 
    // )
    __FRIEND_OS_OPT__(CmpOpAST, _a)
    {
        __os << __LN__;
        __os << "CmpOp { "    
            << *_a.__operator_;
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__


__decl_ast_tid__;
__override_ast_tid__  
};

class LAndExpAST : public BaseAST {
public:
    #define __LAndExpr_Eq 0
    ast_uptr __eq_expr_;
    #define __LAndExpr_LAnd 1
    ast_uptr __land_expr_;
    str_uptr __operator_; // -> &&
    // ast_uptr __eq_expr_;

    // __LAndExpr_Eq | __LAndExpr_LAnd
    int __sub_expr_type_;

    __FRIEND_OS_OPT__(LAndExpAST, _a)
    {
        __os << __LN__;
        __os << "LAndExpr { ";
        switch (_a.__sub_expr_type_)
        {
        case __AddExpr_Mul:
            _a.__eq_expr_->log(__os);
            break;
        case __AddExpr_Add:
            _a.__land_expr_->log(__os);
            __os << " " << *_a.__operator_ << " ";
            _a.__eq_expr_->log(__os);
            break;
        default:
            break;
        }
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__  
};

class LOrExpAST : public BaseAST {
public:
    #define __LOrExpr_LAnd 0
    ast_uptr __land_expr_;
    #define __LOrExpr_LOr 1
    ast_uptr __lor_expr_;
    str_uptr __operator_; // -> ||
    // ast_uptr __land_expr_;

    // __LOrExpr_LAnd | __LOrExpr_LOr
    int __sub_expr_type_;

    __FRIEND_OS_OPT__(LOrExpAST, _a)
    {
        __os << __LN__;
        __os << "LOrExpr { ";
        switch (_a.__sub_expr_type_)
        {
        case __LOrExpr_LAnd:
            _a.__land_expr_->log(__os);
            break;
        case __LOrExpr_LOr:
            _a.__lor_expr_->log(__os);
            __os << " " << *_a.__operator_ << " ";
            _a.__land_expr_->log(__os);
            break;
        default:
            break;
        }
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__  
};

class DeclAST : public BaseAST {
public:
    ast_uptr __const_decl_;

    __decl_sym_table_of;

    __FRIEND_OS_OPT__(DeclAST, _a)
    {
        __os << __LN__;
        __os << "Decl { ";
            _a.__const_decl_->log(__os);
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__ 
};

class ConstDeclAST : public BaseAST {
public:
    ast_uptr __btype_;
    ast_uptr __const_def_;
    ast_uptr __const_defs_;

    // 0,1,2,...n
    std::vector<ast_uptr> __const_defs_vec_;
    __decl_sym_table_of;

    __FRIEND_OS_OPT__(ConstDeclAST, _a)
    {
        __os << __LN__;
        __os << "ConstDecl { ";
            _a.__btype_->log(__os);
            _a.__const_def_->log(__os);

            for(auto & _def : _a.__const_defs_vec_)
            {
                _def->log(__os);
            }
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__ 
};

class BTypeAST : public BaseAST {
public:
    str_uptr __type_;

    __FRIEND_OS_OPT__(BTypeAST, _a)
    {
        __os << __LN__;
        __os << "BType { ";
            __os << *_a.__type_;
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__ 
};

// defs set
class ConstDefsAST : public BaseAST {
public:
    #define __ConstDefs_Null 0
    ast_uptr __const_def_;
    #define __ConstDefs_Recr 1
    ast_uptr __next_;

    int __sub_expr_type_;

    // __decl_sym_table_of;
    std::vector<ast_uptr> __const_defs_;

    __FRIEND_OS_OPT__(ConstDefsAST, _a)
    {
        __os << __LN__;
        __os << "ConstDefsAST { ";
            // __os << *_a.__type_;
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__ 
};

// minimal def
class ConstDefAST : public BaseAST {
public:
    str_uptr __ident_;
    ast_uptr __const_init_val_;

    std::string GetIdent() const {
        return *__ident_;
    }

    TypeValue GetTypeValue() const {
        return TypeValue{};
    }

    __FRIEND_OS_OPT__(ConstDefAST, _a)
    {
        __os << __LN__;
        __os << "ConstDef { ";
        __os << _a.GetIdent()
            << " = ";
        _a.__const_init_val_->log(__os);
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__ 
};

class ConstInitValAST : public BaseAST {
public:
    ast_uptr __const_expr_;

    __FRIEND_OS_OPT__(ConstInitValAST, _a)
    {
        __os << __LN__;
        __os << "ConstInitVal { ";
            _a.__const_expr_->log(__os);
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__ 
};

class ConstExpAST : public BaseAST {
public:
    ast_uptr __expr_;

    __FRIEND_OS_OPT__(ConstExpAST, _a)
    {
        __os << __LN__;
        __os << "ConstExpr { ";
            _a.__expr_->log(__os);
        __os << "\n}";
        return __os;
    }
    __MEMBER_FUNC_LOG_OVERRIDE__

__decl_ast_tid__;
__override_ast_tid__ 
};

#endif 