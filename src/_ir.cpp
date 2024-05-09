#include "../include/_alib.hpp"
#include "../include/_ast.hpp"
#include "../include/_stllib.hpp"

#include "../include/_ir.hpp"
#include <cassert>

__DEF_NS__(Alan)

// void BasicBlockIR::traverseExpr(ast_uptr _expr) 
//     {   
//         std::unique_ptr<ExprAST> exprPtr;
//         std::unique_ptr<PrimaryExprAST> priExprPtr;
//         std::unique_ptr<UnaryExpAST> unaryExprPtr;
//         std::unique_ptr<UnaryOpAST> unaryOpPtr;

//         // type assertion 
//         exprPtr = Alan::dynamic_uptr_cast<ExprAST, BaseAST>(_expr);
//         if (!exprPtr)
//             priExprPtr = Alan::dynamic_uptr_cast<PrimaryExprAST, BaseAST>(_expr);
//         if (!exprPtr && !priExprPtr)
//             unaryExprPtr = Alan::dynamic_uptr_cast<UnaryExpAST, BaseAST>(_expr);


//         if(exprPtr)
//         {
//         traverseExpr(std::move(exprPtr->__unary_expr_));
//         }
//         else if (priExprPtr)
//         {
//             switch (priExprPtr->__sub_expr_type_)
//             {
//             case __PriExpr_Expr:
//                 traverseExpr(std::move(priExprPtr->__expr_));
//                 break;
//             case __PriExpr_Num:
//                 __rpn_op_stk_.emplace(
//                     std::to_string(Alan::static_uptr_cast<NumberAST, BaseAST>
//                                     (priExprPtr->__number_)->__int_const_)
//                 );
//                 break;
//             }
//         }
//         else if (unaryExprPtr)
//         {
//             switch (unaryExprPtr->__sub_expr_type_)
//             {
//             case __UnaExpr_Pri:
//             {
//                 traverseExpr(std::move(unaryExprPtr->__primary_expr_));
//             }
//                 break;
//             case __UnaExpr_Una:
//             {
//                 unaryOpPtr = 
//                     Alan::static_uptr_cast<UnaryOpAST, BaseAST>
//                         (unaryExprPtr->__unary_op_);
//                 auto _op0 = std::string(__Unary_Lval);
//                 auto _op1 = __rpn_op_stk_.top();
//                 auto _oplval = _make_tmp(__tmp_id_);
//                 if(unaryOpPtr->__operator_ == '+')
//                 {   // omit 
//                 }
//                 else
//                 {
//                     std::string _opS;
//                     if (unaryOpPtr->__operator_ == '-') 
//                         _opS = "sub";
//                     else if (unaryOpPtr->__operator_ == '!')
//                         _opS = "eq";
                    
//                     __rpn_op_stk_.pop();

//                     __instructions_.push_back(
//                         std::make_unique<ValueIR>(
//                         "\t" + _oplval + 
//                         " = " + _opS + " " + 
//                         _op0 + ", " +
//                         _op1
//                         ,
//                         KOOPA_RVT_INTEGER
//                         )
//                     );
//                     __rpn_op_stk_.push(_oplval);
//                     ++__tmp_id_;
                    
//                 } 
//             }
//                 break;
//             }
//         }
//         // else if (unaryOpPtr)
//         // {
//         //     switch (unaryOpPtr->__operator_)
//         //     {
//         //     case '+':
//         //         // omit
//         //         break;
//         //     case '-':

//         //         break;
//         //     case '!':

//         //         break;
//         //     }
//         // }
//         else 
//         {
//             assert(0);
//         }

//         // 1. ir counts is same with operator counts
//         // 2. 0 + expr | expr + 0 is ignored
//         // 3. ir temp %n -> can only write once, can read multiple
//         // 4. finally ret %__tmp_id_ 
//         // example
//         // __instructions_.push_back(
//         //     std::make_unique<ValueIR>(
//         //         "\t" //+  //+ 
//         //         " " + 
//         //         ,
//         //         KOOPA_RVT_INTEGER
//         //     )
//         // );
// }

// void BasicBlockIR::OldVersionEval()
// {
//     // while(!__tmp_opt_stk_.empty() &&
//     //                 __rpn_op_stk_.size() >= 2)
//     // {
//     //     auto _opt = __tmp_opt_stk_.top(); __tmp_opt_stk_.pop();
//     //     auto _ropd = __rpn_op_stk_.top(); __rpn_op_stk_.pop();
//     //     auto _lopd = __rpn_op_stk_.top(); __rpn_op_stk_.pop();
//     //     auto _assi = _make_tmp(++__tmp_id_);
//     //     // __rpn_op_stk_.push(__Unary_Lval);
//     //     __rpn_op_stk_.push(_assi); 
//     //     auto _optr = __conv_keyword_(_opt);
//     //     __push_val_({_assi, _optr, _lopd, _ropd}, __bbir_ass_val);
//     // }

// }

const std::string BasicBlockIR::__unkown_tmp_id_("%?");
const std::string BasicBlockIR::__named_id_prefix_("@");
const std::string BasicBlockIR::__unamed_id_prefix_("%");

std::unordered_map<std::string, int> BasicBlockIR::__opt_priority_;

/* reference from: https://pku-minic.github.io/online-doc/#/misc-app-ref/koopa
    ne, eq, gt, lt, ge, le, add, sub, mul, div, mod, and, or, xor, shl, shr, sar.
*/

__FN_INLINE__
std::string BasicBlockIR::__conv_keyword_(const std::string &_keyword)
{
    if (_keyword == "return") return std::string("ret");
    else if(_keyword == "!=") return "ne";
    else if (_keyword == "!" || _keyword == "==") return "eq";
    else if(_keyword == ">") return "gt";
    else if(_keyword == "<") return "lt";
    else if(_keyword == ">=") return "ge";
    else if(_keyword == "<=") return "le";
    else if (_keyword == "+") return "add";
    else if (_keyword == "-") return "sub";
    else if (_keyword == "*") return "mul";
    else if (_keyword == "/") return "div";
    else if (_keyword == "%") return "mod"; // rem in riscv-i64
    else if(_keyword == "&&") return "and";
    else if(_keyword == "||") return "or";
    else if(_keyword == "^") return "xor";
    else if(_keyword == "<<") return "shl"; // shift left
    else if (_keyword == ">>") return "shr"; // shift right
    else if (_keyword == ">>?") return "sar"; // shift arithmetic right
    else return "__nani?__";
}   

__FN_INLINE__
std::string BasicBlockIR::_make_tmp(int _id, const std::string &_prf)
{
    return _prf + std::to_string(_id);
}

// 1. +, - ; other: error 
// 2. decimal, hex, oct, bin
//  (1). dec
//  (2). hex: 0x
//  (3). oct: 0
//  (4). bin: 0b
// 3. boundary, bits
// 4. ignore invalid and illegal 
__FN_INLINE__
bool BasicBlockIR::__is_int_(const std::string & __number_)
{
    // int dec11 = 11; -11
    // int hex11 = 0x11; 0X11 -0x11
    // int oct11 = 011; -011
    // int bin11 = 0b11; 0B11 -0b11
    auto __isbdigit = [](int __ch) -> int { return __ch == '0' || __ch == '1'; };
    auto __isodigit = [] (int __ch) -> int { return __ch >= '0' && __ch <= '7'; };
    #define __reach_end_() \
            if(__num_it_ == __num_end_) return false

    typedef int (*__pred) (int);
    __pred __pred_fn;

    auto __num_it_ = __number_.begin();
    auto __num_end_ = __number_.end();
    if(*__num_it_ == '-' || *__num_it_ == '+')
    { 
        ++__num_it_; __reach_end_(); 
    }

    if(*__num_it_ == '0')
    {
        ++__num_it_; __reach_end_();
        if(*__num_it_ == 'x' || *__num_it_ == 'X')
        {   
        ++__num_it_; __reach_end_();
        __pred_fn = ::isxdigit;
        } else if (*__num_it_ == 'b' || *__num_it_ == 'B')
        {
        ++__num_it_; __reach_end_();
        __pred_fn = __isbdigit;
        } else if (__isodigit(*__num_it_))
        {
        __pred_fn = __isodigit;
        } else 
        {
        return false;
        }
    } else 
    {
        __pred_fn = ::isdigit;
    }

    return __num_it_ != __num_end_ && 
        std::all_of(__num_it_, __num_end_, __pred_fn);
}

__FN_INLINE__
bool BasicBlockIR::__is_opt_(const std::string & __operator_)
{
    return __opt_priority_.find(__operator_) !=
            __opt_priority_.end();
}

// num less, priority greater
__FN_INLINE__
void BasicBlockIR::__init_opt_priority_()
{
    __opt_priority_["#"] = 1<<31,

    __opt_priority_[","] =  1<<15,

    __opt_priority_["|="] = 1<<14, 
    __opt_priority_["^="] = 1<<14, 
    __opt_priority_["&="] = 1<<14, 
    __opt_priority_[">>="] = 1<<14, 
    __opt_priority_["<<="] = 1<<14, 
    __opt_priority_["-="] = 1<<14, 
    __opt_priority_["+="] = 1<<14, 
    __opt_priority_["%="] = 1<<14, 
    __opt_priority_["*="] = 1<<14, 
    __opt_priority_["/="] = 1<<14, 
    __opt_priority_["="] = 1<<14, 

    __opt_priority_["?:"] = 1<<13,

    __opt_priority_["||"] = 1<<12,

    __opt_priority_["&&"] = 1<<11,
    
    __opt_priority_["|"] = 1<<10, 
    
    __opt_priority_["^"] = 1<<9, 
    
    __opt_priority_["&"] = 1<<8, // bitAnd

    __opt_priority_["=="] = 1<<7, 
    __opt_priority_["!="] = 1<<7, 

    __opt_priority_["<="] = 1<<6, 
    __opt_priority_["<"] = 1<<6,
    __opt_priority_[">="] = 1<<6, 
    __opt_priority_[">"] = 1<<6, 

    __opt_priority_[">>"] = 1<<5, 
    __opt_priority_["<<"] = 1<<5,

    __opt_priority_["-"] = 1<<4, 
    __opt_priority_["+"] = 1<<4,

    __opt_priority_["%"] = 1<<3, 
    __opt_priority_["*"] = 1<<3, 
    __opt_priority_["/"] = 1<<3, 

    __opt_priority_["sizeof"] = 1<<2, 
    __opt_priority_["~"] = 1<<2, 
    __opt_priority_["!"] = 1<<2,
    __opt_priority_["&"] = 1<<2, // getAddress
    __opt_priority_["*"] = 1<<2, // deAddress/getValue
    __opt_priority_["--"] = 1<<2, // preVal -> --val
    __opt_priority_["--"] = 1<<2, //postVal -> val--
    __opt_priority_["++"] = 1<<2, // preVal -> ++val
    __opt_priority_["++"] = 1<<2, //postVal -> val++
    __opt_priority_["(T)"] = 1<<2, // (T)typecast
    __opt_priority_["-"] = 1<<2, // unary

    __opt_priority_["."] = 1<<1,
    __opt_priority_["->"] = 1<<1, 
    __opt_priority_["()"] = 1<<1, 
    __opt_priority_["[]"] = 1<<1;

    __opt_priority_["$"] = 1<<0;
}

__FN_INLINE__
bool BasicBlockIR::__opt_less_
(const std::string & _opt1,
const std::string & _opt2)
{
    if(_opt1 == "#" || _opt1 == "(" || _opt2 == ")")
        return true;
    else if (_opt1 == ")" || _opt2 == "(")
        return false;
    
    assert(__opt_priority_.find(_opt1) != __opt_priority_.end());
    assert(__opt_priority_.find(_opt2) != __opt_priority_.end());

    auto _prio1 = __opt_priority_[_opt1];
    auto _prio2 = __opt_priority_[_opt2];
    
    if (_prio1 == _prio2)
        return false;
    return _prio1 > _prio2;

}

__FN_INLINE__
bool BasicBlockIR::__is_const_expr_(const std::string & __token_)
{
    return __is_int_(__token_);
}

__FN_INLINE__
int BasicBlockIR::__i32_opt_res_(int lval, std::string opt, int rval)
{
    char sopt = opt.size() == 1 ? opt[0] : 0;
    if(opt.size() > 1)
    {
        if(opt == "!=") return lval != rval;
        else if(opt == "==") return lval == rval;
        else if(opt == ">=") return lval >= rval;
        else if(opt == "<=") return lval <= rval;
        else if(opt == "&&") return lval && rval;
        else if(opt == "||") return lval || rval;
        else if(opt == ">>") return lval >> rval;
        else if(opt == "<<") return lval << rval;
        else return 0; 
    } else
    {
        switch (sopt)
        {
        case '!': return lval == rval;
        case '+': return lval + rval;
        case '-': return lval - rval;
        case '*': return lval * rval;
        case '/':
        {
            if(rval == 0)
            {
                ERRLN("divide 0");
                assert(0);
            }
            return lval / rval;
        }
        case '%':
        {
            if(rval == 0)
            {
                ERRLN("divide 0");
                assert(0);
            }
            return lval % rval;
        }
        case '>': return lval > rval;
        case '<': return lval < rval;
        case '^': return lval ^ rval;
        default:
            break;
        }
    }
    return 0;
}

void BasicBlockIR::__push_val_
(const std::vector<std::string> & _list, int _val_type)
{
    switch (_val_type)
    {
    case __bbir_ret_val:
        assert(_list.size() >= 2);
        __instructions_.push_back(
        std::make_unique<ValueIR>(
            "\t" + __conv_keyword_(_list[0]) + 
            " " + _list[1]
            ,
            KOOPA_RVT_INTEGER)
        );
        break;
    case __bbir_ass_val:
        assert(_list.size() >= 4);
        __instructions_.push_back(
            std::make_unique<ValueIR>(
            "\t" + _list[0] + 
            " = " + _list[1] + " " + 
            _list[2] + ", " +
            _list[3]
            ,
            KOOPA_RVT_INTEGER)
        );
        break;
    default:
        assert(0);
    }
           
}

void BasicBlockIR::traverseExpr(ast_uptr _expr) 
{
    std::unique_ptr<ExprAST> expr;
    std::unique_ptr<PrimaryExprAST> priExpr;
    std::unique_ptr<UnaryExpAST> unaryExpr;
    std::unique_ptr<UnaryOpAST> unaryOp;
    std::unique_ptr<NumberAST> numberN;
    std::unique_ptr<MulExpAST> mulExpr;
    std::unique_ptr<AddExpAST> addExpr;
    std::unique_ptr<BinOpAST> binOp;
    std::unique_ptr<RelExpAST> relExpr;
    std::unique_ptr<EqExpAST> eqExpr;
    std::unique_ptr<CmpOpAST> cmpOp;
    std::unique_ptr<LAndExpAST> landExpr;
    std::unique_ptr<LOrExpAST> lorExpr;

    switch (_expr->tid())
    {
    case EXPR_AST:
        expr = Alan::static_uptr_cast<ExprAST, BaseAST>(_expr);
        traverseExpr(std::move(expr->__lor_expr_));
        break;
    case PRIMARY_EXPR_AST:
        priExpr = Alan::static_uptr_cast<PrimaryExprAST, BaseAST>(_expr);
        switch (priExpr->__sub_expr_type_)
        {
        case __PriExpr_Expr:
            __mid_op_stk_.push_back("(");
            traverseExpr(std::move(priExpr->__expr_));
            __mid_op_stk_.push_back(")");
            break;
        case __PriExpr_Num:
            // __rpn_op_stk_.push(__Unary_Lval);
            traverseExpr(std::move(priExpr->__number_));
            break;
        // default:
        //     assert(0);
        }
        break;
    case NUMBER_AST:
        numberN = Alan::static_uptr_cast<NumberAST, BaseAST>(_expr);
        // __rpn_op_stk_.push(std::to_string(numberN->__int_const_));
        __mid_op_stk_.push_back(std::to_string(numberN->__int_const_));
        break;
    case UNARY_EXPR_AST:
        unaryExpr = Alan::static_uptr_cast<UnaryExpAST, BaseAST>(_expr);
        switch (unaryExpr->__sub_expr_type_)
        {
        case __UnaExpr_Pri:
            traverseExpr(std::move(unaryExpr->__primary_expr_));
            break;
        case __UnaExpr_Una:
            // __rpn_op_stk_.push(__Unary_Lval);
            __mid_op_stk_.push_back(__Unary_Lval);
            traverseExpr(std::move(unaryExpr->__unary_op_));
            traverseExpr(std::move(unaryExpr->__unary_expr_));
            break;
        // default:
        //     assert(0);
        }
        break;
    case UNARY_OP_AST:
        unaryOp = Alan::static_uptr_cast<UnaryOpAST, BaseAST>(_expr);
        // __tmp_opt_stk_.push(*unaryOp->__operator_);
        __mid_op_stk_.push_back(*unaryOp->__operator_);
        break;
    case MUL_EXPR_AST:
        mulExpr = Alan::static_uptr_cast<MulExpAST, BaseAST>(_expr);
        switch (mulExpr->__sub_expr_type_)
        {
        case __MulExpr_Una:
            traverseExpr(std::move(mulExpr->__unary_expr_));
            break;
        case __MulExpr_Mul:
            traverseExpr(std::move(mulExpr->__mul_expr_));
            traverseExpr(std::move(mulExpr->__bin_op_));
            traverseExpr(std::move(mulExpr->__unary_expr_));
            break;
        // default:
        //     assert(0);
        }
        break;
    case ADD_EXPR_AST:
        addExpr = Alan::static_uptr_cast<AddExpAST, BaseAST>(_expr);
        switch (addExpr->__sub_expr_type_)
        {
        case __AddExpr_Mul:
            traverseExpr(std::move(addExpr->__mul_expr_));
            break;
        case __AddExpr_Add:
            traverseExpr(std::move(addExpr->__add_expr_));
            traverseExpr(std::move(addExpr->__bin_op_));
            traverseExpr(std::move(addExpr->__mul_expr_));
            break;
        // default:
        //     assert(0);
        }
        break;
    case BINARY_OP_AST:
        binOp = Alan::static_uptr_cast<BinOpAST, BaseAST>(_expr);
        // __tmp_opt_stk_.push(*binOp->__operator_);
        __mid_op_stk_.push_back(*binOp->__operator_);
        break;
    case REL_EXPR_AST:
        relExpr = Alan::static_uptr_cast<RelExpAST, BaseAST>(_expr);
        switch (relExpr->__sub_expr_type_)
        {
        case __RelExpr_Add:
            traverseExpr(std::move(relExpr->__add_expr_));
            break;
        case __RelExpr_Rel:
            traverseExpr(std::move(relExpr->__rel_expr_));
            traverseExpr(std::move(relExpr->__cmp_op_));
            traverseExpr(std::move(relExpr->__add_expr_));
            break;
        // default:
        //     assert(0);
        }
        break;
    case EQ_EXPR_AST:
        eqExpr = Alan::static_uptr_cast<EqExpAST, BaseAST>(_expr);
        switch (eqExpr->__sub_expr_type_)
        {
        case __EqExpr_Rel:
            traverseExpr(std::move(eqExpr->__rel_expr_));
            break;
        case __EqExpr_Eq:
            traverseExpr(std::move(eqExpr->__eq_expr_));
            traverseExpr(std::move(eqExpr->__cmp_op_));
            traverseExpr(std::move(eqExpr->__rel_expr_));
            break;
        // default:
        //     assert(0);
        }
        break;
    case CMP_OP_AST:
        cmpOp = Alan::static_uptr_cast<CmpOpAST, BaseAST>(_expr);
        // __tmp_opt_stk_.push(*cmpOp->__operator_);
        __mid_op_stk_.push_back(*cmpOp->__operator_);
        break;
    case LAND_EXPR_AST:
        landExpr = Alan::static_uptr_cast<LAndExpAST, BaseAST>(_expr);
        switch (landExpr->__sub_expr_type_)
        {
        case __LAndExpr_Eq:
            traverseExpr(std::move(landExpr->__eq_expr_));
            break;
        case  __LAndExpr_LAnd:
            traverseExpr(std::move(landExpr->__land_expr_));
            // __tmp_opt_stk_.push("&&");
            __mid_op_stk_.push_back("&&");
            traverseExpr(std::move(landExpr->__eq_expr_));
            break;
        // default:
        //     assert(0);
        }
        break;
    case LOR_EXPR_AST:
        lorExpr = Alan::static_uptr_cast<LOrExpAST, BaseAST>(_expr);
        switch (lorExpr->__sub_expr_type_)
        {
        case __LOrExpr_LAnd:
            traverseExpr(std::move(lorExpr->__land_expr_));
            break;
        case  __LOrExpr_LOr:
            traverseExpr(std::move(lorExpr->__lor_expr_));
            // __tmp_opt_stk_.push("||");
            __mid_op_stk_.push_back("||");
            traverseExpr(std::move(lorExpr->__land_expr_));
            break;
        // default:
        //     assert(0);
        }
        break;
    // default:
    //     assert(0);
    }

}

void BasicBlockIR::midExpr2RPN()
{
    __tmp_opt_stk_.push_back("#");
    for(auto token : __mid_op_stk_)
    {
        if (__is_int_(token))
            __rpn_op_stk_.push_back(token);
        else if (__is_opt_(token))
        {
            while(!__tmp_opt_stk_.empty() && 
                    !__opt_less_(__tmp_opt_stk_.back(), token))
            {
                __rpn_op_stk_.push_back(__tmp_opt_stk_.back());
                __tmp_opt_stk_.pop_back();
            }
            // make sure of #
            __tmp_opt_stk_.push_back(token);

        } 
        else if (token == "(")
            __tmp_opt_stk_.push_back("(");
        else if (token == ")")
        {
            while(!__tmp_opt_stk_.empty() && __tmp_opt_stk_.back()!= "(")
            {
                __rpn_op_stk_.push_back(__tmp_opt_stk_.back());
                __tmp_opt_stk_.pop_back();
            }
            if(!__tmp_opt_stk_.empty() && __tmp_opt_stk_.back() == "(")
                __tmp_opt_stk_.pop_back();
        }
        else // error
        {
            ERRLN("midExpr is illegal");
            assert(0);
        }
    }

    while(__tmp_opt_stk_.size() > 1) // # 
    {
        __rpn_op_stk_.push_back(__tmp_opt_stk_.back());
        __tmp_opt_stk_.pop_back();
    }

}

void BasicBlockIR::evalRPN()
{   
    // const_expr
    while(__rpn_op_stk_.size() >= 3)
    {
        auto lval = __str2int_(__rpn_op_stk_.front()); __rpn_op_stk_.pop_front();
        auto rval = __str2int_(__rpn_op_stk_.front()); __rpn_op_stk_.pop_front();
        auto opt = __rpn_op_stk_.front(); __rpn_op_stk_.pop_front();
        __rpn_op_stk_.push_front(__int2str_(__i32_opt_res_(lval, opt, rval)));
    }
}

BasicBlockIR::BasicBlockIR(BlockAST & _bast)
    : __block_ast_(std::make_unique<BlockAST>
                                (std::move(_bast)))
{ 
    auto stmtPtr = Alan::static_uptr_cast<StmtAST, BaseAST>
                    (__block_ast_->__stmt_);
    auto exprPtr = Alan::static_uptr_cast<ExprAST, BaseAST>
                    (stmtPtr->__expr_);
    
    this->__tmp_id_ = -1;
    __init_opt_priority_();
    // mid order traverse exprTree to get midExpr
    traverseExpr(std::move(exprPtr));
    
    #if (DEBUG == 1)
        LogContainer(__mid_op_stk_);
    #endif 
    // convert midExpr to RPN expr
    midExpr2RPN();
    #if (DEBUG == 1)
        LogContainer(__tmp_opt_stk_);
        LogContainer(__rpn_op_stk_);
    #endif 
    // optimize RPN expr and push valIR
    evalRPN();
    // push return valIR
    __push_val_({*stmtPtr->__ret_, __rpn_op_stk_.front()}, __bbir_ret_val);
}

__END_NS__