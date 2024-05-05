#include "../include/_ir.hpp"
#include <cassert>

__USE_NS__(Alan);

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

const std::string BasicBlockIR::__unkown_tmp_id_("%?");
const std::string BasicBlockIR::__named_id_prefix_("@");
const std::string BasicBlockIR::__unamed_id_prefix_("%");

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
            traverseExpr(std::move(priExpr->__expr_));
            break;
        case __PriExpr_Num:
            // __tmp_opd_stk_.push(__Unary_Lval);
            traverseExpr(std::move(priExpr->__number_));
            break;
        // default:
        //     assert(0);
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
            __tmp_opd_stk_.push(__Unary_Lval);
            traverseExpr(std::move(unaryExpr->__unary_op_));
            traverseExpr(std::move(unaryExpr->__unary_expr_));
            break;
        // default:
        //     assert(0);
        }
        break;
    case UNARY_OP_AST:
        unaryOp = Alan::static_uptr_cast<UnaryOpAST, BaseAST>(_expr);
        __tmp_opt_stk_.push(std::string(1, unaryOp->__operator_));
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
        __tmp_opt_stk_.push(std::string(1, binOp->__operator_));
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
        __tmp_opt_stk_.push(*cmpOp->__operator_);
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
            __tmp_opt_stk_.push("&&");
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
            __tmp_opt_stk_.push("||");
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

