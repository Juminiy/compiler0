%code requires {
  #include <memory>
  #include <string>
}

%{

#include <iostream>
#include <memory>
#include <string>

#include "../include/_ast.hpp"

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 不能在 union 里写一个带析构函数的类会出现什么情况
%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 1. 终结符
// 2. 非终结符

// 终结符
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST
%token <str_val> OPT

// 非终结符
%type <ast_val> CompUnit FuncDef FuncType Block Stmt 
%type <ast_val> Exp PrimaryExp Number UnaryExp UnaryOp
%type <ast_val> MulExp AddExp BinOp 
%type <ast_val> RelExp EqExp CmpOp LAndExp LOrExp

%%

CompUnit
  : FuncDef {
    auto compUnit = make_unique<CompUnitAST>();
    compUnit->func_def = unique_ptr<BaseAST>($1);
    ast = std::move(compUnit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto funcDef = make_unique<FuncDefAST>();
    funcDef->func_type = unique_ptr<BaseAST>($1);
    funcDef->ident = unique_ptr<std::string>($2);
    funcDef->block = unique_ptr<BaseAST>($5);
    $$ = funcDef.release();
  }
  ;

FuncType
  : INT {
    auto funcType = make_unique<FuncTypeAST>();
    funcType->__type_ = make_unique<std::string>("int");
    $$ = funcType.release();
  }
  ;

Block
  : '{' Stmt '}' {
    auto block = make_unique<BlockAST>();
    block->__stmt_ = unique_ptr<BaseAST>($2);
    $$ = block.release();
  }
  ;

Stmt
  : RETURN Exp ';' {
    auto stmt = make_unique<StmtAST>();
    stmt->__ret_ = make_unique<std::string>("return");
    stmt->__expr_ = unique_ptr<BaseAST>($Exp);
    $$ = stmt.release();
  }
  ;

Exp
  : LOrExp {
    auto expr = make_unique<ExprAST>();
    expr->__lor_expr_ = unique_ptr<BaseAST>($1);
    $$ = expr.release();
    }
  ;

PrimaryExp
  : '(' Exp ')' {
      auto pri_expr = make_unique<PrimaryExprAST>();
      pri_expr->__expr_ = unique_ptr<BaseAST>($Exp);
      pri_expr->__sub_expr_type_ = __PriExpr_Expr;
      $$ = pri_expr.release(); 
    }
    | Number {
      auto pri_expr = make_unique<PrimaryExprAST>();
      pri_expr->__number_ = unique_ptr<BaseAST>($Number);
      pri_expr->__sub_expr_type_ = __PriExpr_Num;
      $$ = pri_expr.release();
    }
  ;

Number
  : INT_CONST {
    auto number = make_unique<NumberAST>();
    number->__int_const_ = $1;
    $$ = number.release();
  }
  ;

UnaryExp
  : PrimaryExp {
      auto unary_expr = make_unique<UnaryExpAST>();
      unary_expr->__primary_expr_ = unique_ptr<BaseAST>($1);
      unary_expr->__sub_expr_type_ = __UnaExpr_Pri;
      $$ = unary_expr.release();
    }
    | UnaryOp UnaryExp {
      auto unary_expr = make_unique<UnaryExpAST>();
      unary_expr->__unary_op_ = unique_ptr<BaseAST>($1);
      unary_expr->__unary_expr_ = unique_ptr<BaseAST>($2);
      unary_expr->__sub_expr_type_ = __UnaExpr_Una;
      $$ = unary_expr.release();
    } 
  ;

UnaryOp
  : '+'   { $$ = make_unique<UnaryOpAST>(__UnaOp_Plus).release(); }
    | '-' { $$ = make_unique<UnaryOpAST>(__UnaOp_Minus).release(); }
    | '!' { $$ = make_unique<UnaryOpAST>(__UnaOp_Not).release(); }
  ;

MulExp
  : UnaryExp {
      auto mul_expr = make_unique<MulExpAST>();
      mul_expr->__unary_expr_ = unique_ptr<BaseAST>($1);
      mul_expr->__sub_expr_type_ = __MulExpr_Una;
      $$ = mul_expr.release();
    } 
    | MulExp BinOp UnaryExp {
      auto mul_expr = make_unique<MulExpAST>();
      mul_expr->__mul_expr_ = unique_ptr<BaseAST>($1);
      mul_expr->__bin_op_ = unique_ptr<BaseAST>($2);
      mul_expr->__unary_expr_ = unique_ptr<BaseAST>($3);
      mul_expr->__sub_expr_type_ = __MulExpr_Mul;
      $$ = mul_expr.release();
    }
  ;

AddExp
  : MulExp {
      auto add_expr = make_unique<AddExpAST>();
      add_expr->__mul_expr_ = unique_ptr<BaseAST>($1);
      add_expr->__sub_expr_type_ = __AddExpr_Mul;
      $$ = add_expr.release();
    } 
    | AddExp BinOp MulExp {
      auto add_expr = make_unique<AddExpAST>();
      add_expr->__add_expr_ = unique_ptr<BaseAST>($1);
      add_expr->__bin_op_ = unique_ptr<BaseAST>($2);
      add_expr->__mul_expr_ = unique_ptr<BaseAST>($3);
      add_expr->__sub_expr_type_ = __AddExpr_Add;
      $$ = add_expr.release();
    }
  ;

BinOp
  : '*' { $$ = make_unique<BinOpAST>(__BinOp_Mul).release(); }
    | '/' { $$ = make_unique<BinOpAST>(__BinOp_Div).release(); }
    | '%' { $$ = make_unique<BinOpAST>(__BinOp_Mod).release(); }
    | '+' { $$ = make_unique<BinOpAST>(__BinOp_Plus).release(); }
    | '-' { $$ = make_unique<BinOpAST>(__BinOp_Minus).release(); }
  ;

RelExp
  : AddExp {
      auto rel_expr = make_unique<RelExpAST>();
      rel_expr->__add_expr_ = unique_ptr<BaseAST>($1);
      rel_expr->__sub_expr_type_ = __RelExpr_Add;
      $$ = rel_expr.release();
    } 
    | RelExp CmpOp AddExp {
      auto rel_expr = make_unique<RelExpAST>();
      rel_expr->__rel_expr_ = unique_ptr<BaseAST>($1);
      rel_expr->__cmp_op_ = unique_ptr<BaseAST>($2);
      rel_expr->__add_expr_ = unique_ptr<BaseAST>($3);
      rel_expr->__sub_expr_type_ = __RelExpr_Rel;
      $$ = rel_expr.release();
    }
  ;

EqExp 
  : RelExp {
      auto eq_expr = make_unique<EqExpAST>();
      eq_expr->__rel_expr_ = unique_ptr<BaseAST>($1);
      eq_expr->__sub_expr_type_ = __EqExpr_Rel;
      $$ = eq_expr.release();
    }
    | EqExp CmpOp RelExp {
      auto eq_expr = make_unique<EqExpAST>();
      eq_expr->__eq_expr_ = unique_ptr<BaseAST>($1);
      eq_expr->__cmp_op_ = unique_ptr<BaseAST>($2);
      eq_expr->__rel_expr_ = unique_ptr<BaseAST>($3);
      eq_expr->__sub_expr_type_ = __EqExpr_Eq;
      $$ = eq_expr.release();
    }
  ;

CmpOp
  : OPT {
    auto cmp_op = make_unique<CmpOpAST>($1);
    // cmp_op->__operator_ = std::unique_ptr<std::string>($1);
    $$ = cmp_op.release();
  }
  // : "<" { $$ = make_unique<CmpOpAST>(__CmpOp_Lt).release(); }
  //   | ">" { $$ = make_unique<CmpOpAST>(__CmpOp_Gt).release(); }
  //   | "<=" { $$ = make_unique<CmpOpAST>(__CmpOp_Le).release(); }
  //   | ">=" { $$ = make_unique<CmpOpAST>(__CmpOp_Ge).release(); }
  //   | "==" { $$ = make_unique<CmpOpAST>(__CmpOp_Eq).release(); }
  //   | "!=" { $$ = make_unique<CmpOpAST>(__CmpOp_Ne).release(); }
  ;

LAndExp 
  : EqExp {
      auto land_expr = make_unique<LAndExpAST>();
      land_expr->__eq_expr_ = unique_ptr<BaseAST>($1);
      land_expr->__sub_expr_type_ = __LAndExpr_Eq;
      $$ = land_expr.release();
    } 
    | LAndExp OPT EqExp {
      auto land_expr = make_unique<LAndExpAST>();
      land_expr->__land_expr_ = unique_ptr<BaseAST>($1);
      land_expr->__operator_ = unique_ptr<std::string>($2);
      land_expr->__eq_expr_ = unique_ptr<BaseAST>($3);
      land_expr->__sub_expr_type_ = __LAndExpr_LAnd;
      $$ = land_expr.release();
    }
  ;

LOrExp 
  : LAndExp {
      auto lor_expr = make_unique<LOrExpAST>();
      lor_expr->__land_expr_ = unique_ptr<BaseAST>($1);
      lor_expr->__sub_expr_type_ = __LOrExpr_LAnd;
      $$ = lor_expr.release();
    }
    | LOrExp OPT LAndExp {
      auto lor_expr = make_unique<LOrExpAST>();
      lor_expr->__lor_expr_ = unique_ptr<BaseAST>($1);
      lor_expr->__operator_ = unique_ptr<std::string>($2);
      lor_expr->__land_expr_ = unique_ptr<BaseAST>($3);
      lor_expr->__sub_expr_type_ = __LOrExpr_LAnd;
      $$ = lor_expr.release();
    }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
