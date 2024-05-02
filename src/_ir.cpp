#include "../include/_ir.hpp"
#include <cassert>

__USE_NS__(Alan);

__MEMBER_FUNC_LOG_DEF__(ProgramIR)

__MEMBER_FUNC_LOG_DEF__(FuncIR)

__MEMBER_FUNC_LOG_DEF__(BasicBlockIR)

const std::string BasicBlockIR::__unkown_tmp_id_("%?");
const std::string BasicBlockIR::__named_id_prefix_("@");
const std::string BasicBlockIR::__unamed_id_prefix_("%");

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
//                 __tmp_opd_stk_.emplace(
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
//                 auto _op1 = __tmp_opd_stk_.top();
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
                    
//                     __tmp_opd_stk_.pop();

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
//                     __tmp_opd_stk_.push(_oplval);
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

void BasicBlockIR::traverseExpr(ast_uptr _expr) 
{
    std::unique_ptr<ExprAST> expr;
    std::unique_ptr<PrimaryExprAST> priExpr;
    std::unique_ptr<UnaryExpAST> unaryExpr;
    std::unique_ptr<UnaryOpAST> unaryOp;
    std::unique_ptr<NumberAST> numberN;

    switch (_expr->tid())
    {
    case EXPR_AST:
        expr = Alan::static_uptr_cast<ExprAST, BaseAST>(_expr);
        traverseExpr(std::move(expr->__unary_expr_));
        break;
    case PRIMARY_EXPR_AST:
        priExpr = Alan::static_uptr_cast<PrimaryExprAST, BaseAST>(_expr);
        switch (priExpr->__sub_expr_type_)
        {
        case __PriExpr_Expr:
            traverseExpr(std::move(priExpr->__expr_));
            break;
        case __PriExpr_Num:
            __tmp_opd_stk_.push(__Unary_Lval);
            traverseExpr(std::move(priExpr->__number_));
            break;
        default:
            assert(0);
        }
        break;
    case NUMBER_AST:
        numberN = Alan::static_uptr_cast<NumberAST, BaseAST>(_expr);
        __tmp_opd_stk_.push(std::to_string(numberN->__int_const_));
        break;
    case UNARY_EXPR_AST:
        unaryExpr = Alan::static_uptr_cast<UnaryExpAST, BaseAST>(_expr);
        switch (unaryExpr->__sub_expr_type_)
        {
        case __UnaExpr_Pri:
            traverseExpr(std::move(unaryExpr->__primary_expr_));
            break;
        case __UnaExpr_Una:
            traverseExpr(std::move(unaryExpr->__unary_op_));
            traverseExpr(std::move(unaryExpr->__unary_expr_));
            break;
        default:
            assert(0);
        }
        break;
    case UNARY_OP_AST:
        unaryOp = Alan::static_uptr_cast<UnaryOpAST, BaseAST>(_expr);
        __tmp_opt_stk_.push(unaryOp->__operator_);
        break;
    default:
        assert(0);
    }

}

__MEMBER_FUNC_LOG_DEF__(ValueIR)
