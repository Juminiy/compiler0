%code requires {
  #include <memory>
  #include <string>
}

%{

#include <iostream>
#include <memory>
#include <string>

#include "../include/_ast.hpp"
#include "../include/_ref.hpp"

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval
%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 1. 终结符
// 2. 非终结符

// 终结符
%token INT RETURN CONST
%token <str_val> IDENT OPT
%token <int_val> INT_CONST

// 非终结符
%type <ast_val> CompUnit FuncDef FuncType Block  
%type <ast_val> BlockItems BlockItem Decl Stmt 
%type <ast_val> Exp PrimaryExp UnaryExp UnaryOp
%type <ast_val> MulExp AddExp BinOp 
%type <ast_val> RelExp EqExp CmpOp LAndExp LOrExp
%type <ast_val> ConstDecl BType ConstDefs ConstDef ConstInitVal ConstExp 
%type <ast_val> LVal Number

%%

CompUnit
  : FuncDef {
    auto compUnit = make_unique<CompUnitAST>();
    compUnit->func_def = ast_uptr($1);
    ast = std::move(compUnit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto funcDef = make_unique<FuncDefAST>();
    funcDef->func_type = ast_uptr($1);
    funcDef->ident = str_uptr($2);
    funcDef->block = ast_uptr($5);
    $$ = funcDef.release();
  }
  ;

FuncType
  : INT {
    auto funcType = make_unique<FuncTypeAST>();
    funcType->__type_ = __make_str_("int");
    $$ = funcType.release();
  }
  ;

Block
  : '{' BlockItems '}' {
    auto block = make_unique<BlockAST>();
    block->__block_items_ = ast_uptr($2);
    $$ = block.release();
  }
  ;

BlockItems
  : /* empty */ {
    auto block_items = make_unique<BlockItemsAST>();
    block_items->__sub_type_ = __BlockItems_Null;
    $$ =  block_items.release();
  }
  | BlockItem BlockItems {
    auto block_items = make_unique<BlockItemsAST>();
    block_items->__sub_type_ = __BlockItems_Recv;
    block_items->__block_item_ = ast_uptr($1);
    block_items->__next_ = ast_uptr($2);
    $$ =  block_items.release();
  }
  ;

BlockItem 
  : Decl {
    auto block_item = make_unique<BlockItemAST>();
    block_item->__sub_type_ = __BlockItem_Decl;
    block_item->__decl_ = ast_uptr($1);
    $$ = block_item.release();
  } 
  | Stmt {
    auto block_item = make_unique<BlockItemAST>();
    block_item->__sub_type_ = __BlockItem_Stmt;
    block_item->__stmt_ = ast_uptr($1);
    $$ = block_item.release();
  }
  | /* empty */ {
    auto block_item = make_unique<BlockItemAST>();
    block_item->__sub_type_ = __BlockItem_Null;
    $$ = block_item.release();
  }
  ;

Decl
  : ConstDecl {
    auto decl = make_unique<DeclAST>();
    decl->__const_decl_ = ast_uptr($1);
    $$ = decl.release();
  }
  | VarDecl {
    
  }
  ;

ConstDecl
  : CONST BType ConstDef ConstDefs ";" {
    auto const_decl = make_unique<ConstDeclAST>();
    const_decl->__btype_ = ast_uptr($2);
    const_decl->__const_def_ = ast_uptr($3);
    const_decl->__const_defs_ = ast_uptr($4);
    $$ = const_decl.release();
  }
  ;

BType 
  : INT {
    auto btype = make_unique<BTypeAST>();
    btype->__type_ = __make_str_("int");
    $$ = btype.release();
  }
  ;

ConstDefs 
  : /* empty */ {
    auto const_defs = make_unique<ConstDefsAST>(); 
    const_defs->__sub_expr_type_ = __ConstDefs_Null;
    // const_defs->__sym_table = std::make_shared<SymTable>();
    $$ = const_defs.release();
  }
  | "," ConstDef ConstDefs {
    auto const_defs = make_unique<ConstDefsAST>(); 
    const_defs->__sub_expr_type_ = __ConstDefs_Recr;
    const_defs->__const_def_ = ast_uptr($2);
    const_defs->__next_ = ast_uptr($3);
    // auto child_const_def = 
    //   Alan::static_uptr_cast<ConstDefAST, BaseAST>($2);
    // auto child_const_defs = 
    //   Alan::static_uptr_cast<ConstDefsAST, BaseAST>($3);
    // const_defs->__sym_table = child_const_defs->__sym_table;
    // const_defs->__sym_table->Put(
    //   child_const_def->GetIdent(),
    //   child_const_def->GetTypeValue()
    // );
    $$ = const_defs.release();
  }
  ;

ConstDef 
  : IDENT "=" ConstInitVal {
    auto const_def = make_unique<ConstDefAST>();
    const_def->__ident_ = str_uptr($1);
    const_def->__const_init_val_ = ast_uptr($3);
    $$ = const_def.release();
  }
  ;

ConstInitVal 
  : ConstExp {
    auto const_init_val = make_unique<ConstInitValAST>();
    const_init_val->__const_expr_ = ast_uptr($1);
    $$ = const_init_val.release();
  }
  ;

ConstExp 
  : Exp {
    auto const_expr = make_unique<ConstExpAST>();
    const_expr->__expr_ = ast_uptr($1);
    $$ = const_expr.release();
  }
  ;

VarDecl
  : BType VarDef VarDefs ";" {
    
  }
  ;

VarDefs 
  : "," VarDef VarDefs {

  }
  ;

VarDef 
  : IDENT {

  }
  | IDENT "=" InitVal {

  }
  ;

InitVal 
  : Exp {

  }
  ;


Stmt
  : LVal "=" Exp ";" {
    auto stmt = make_unique<StmtAST>();
    stmt->__sub_type_ = _Stmt_Lval;
    stmt->__lval_ = ast_uptr($1);
    stmt->__expr_ = ast_uptr($3);
    $$ = stmt.release();
  }
  | RETURN Exp ';' {
    auto stmt = make_unique<StmtAST>();
    stmt->__sub_type_ = _Stmt_Ret;
    stmt->__ret_ = __make_str_("return");
    stmt->__expr_ = ast_uptr($2);
    $$ = stmt.release();
  }
  ;

Exp
  : LOrExp {
    auto expr = make_unique<ExprAST>();
    expr->__lor_expr_ = ast_uptr($1);
    $$ = expr.release();
    }
  ;

PrimaryExp
  : '(' Exp ')' {
      auto pri_expr = make_unique<PrimaryExprAST>();
      pri_expr->__expr_ = ast_uptr($Exp);
      pri_expr->__sub_expr_type_ = __PriExpr_Expr;
      $$ = pri_expr.release(); 
    }
    | Number {
      auto pri_expr = make_unique<PrimaryExprAST>();
      pri_expr->__number_ = ast_uptr($Number);
      pri_expr->__sub_expr_type_ = __PriExpr_Num;
      $$ = pri_expr.release();
    }
    | LVal {
      auto pri_expr = make_unique<PrimaryExprAST>();
      pri_expr->__lval_ = ast_uptr($LVal);
      pri_expr->__sub_expr_type_ = __PriExpr_LVal;
      $$ = pri_expr.release(); 
    }
  ;

LVal
  : IDENT {
    auto lval = make_unique<LValAST>();
    lval->__ident_ = str_uptr($1);
    $$ = lval.release(); 
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
      unary_expr->__primary_expr_ = ast_uptr($1);
      unary_expr->__sub_expr_type_ = __UnaExpr_Pri;
      $$ = unary_expr.release();
    }
    | UnaryOp UnaryExp {
      auto unary_expr = make_unique<UnaryExpAST>();
      unary_expr->__unary_op_ = ast_uptr($1);
      unary_expr->__unary_expr_ = ast_uptr($2);
      unary_expr->__sub_expr_type_ = __UnaExpr_Una;
      $$ = unary_expr.release();
    } 
  ;

UnaryOp
  : OPT {
    auto unary_op = make_unique<UnaryOpAST>();
    unary_op->__operator_ = str_uptr($1);
    $$ = unary_op.release();
  }
  ;
  // : '+'   { $$ = make_unique<UnaryOpAST>(__UnaOp_Plus).release(); }
  //   | '-' { $$ = make_unique<UnaryOpAST>(__UnaOp_Minus).release(); }
  //   | '!' { $$ = make_unique<UnaryOpAST>(__UnaOp_Not).release(); }
  // ;

MulExp
  : UnaryExp {
      auto mul_expr = make_unique<MulExpAST>();
      mul_expr->__unary_expr_ = ast_uptr($1);
      mul_expr->__sub_expr_type_ = __MulExpr_Una;
      $$ = mul_expr.release();
    } 
    | MulExp BinOp UnaryExp {
      auto mul_expr = make_unique<MulExpAST>();
      mul_expr->__mul_expr_ = ast_uptr($1);
      mul_expr->__bin_op_ = ast_uptr($2);
      mul_expr->__unary_expr_ = ast_uptr($3);
      mul_expr->__sub_expr_type_ = __MulExpr_Mul;
      $$ = mul_expr.release();
    }
  ;

AddExp
  : MulExp {
      auto add_expr = make_unique<AddExpAST>();
      add_expr->__mul_expr_ = ast_uptr($1);
      add_expr->__sub_expr_type_ = __AddExpr_Mul;
      $$ = add_expr.release();
    } 
    | AddExp BinOp MulExp {
      auto add_expr = make_unique<AddExpAST>();
      add_expr->__add_expr_ = ast_uptr($1);
      add_expr->__bin_op_ = ast_uptr($2);
      add_expr->__mul_expr_ = ast_uptr($3);
      add_expr->__sub_expr_type_ = __AddExpr_Add;
      $$ = add_expr.release();
    }
  ;

BinOp
  : OPT {
    auto bin_op = make_unique<BinOpAST>();
    bin_op->__operator_ = str_uptr($1);
    $$ = bin_op.release();
  }
  ;
  // : '*' { $$ = make_unique<BinOpAST>(__BinOp_Mul).release(); }
  //   | '/' { $$ = make_unique<BinOpAST>(__BinOp_Div).release(); }
  //   | '%' { $$ = make_unique<BinOpAST>(__BinOp_Mod).release(); }
  //   | '+' { $$ = make_unique<BinOpAST>(__BinOp_Plus).release(); }
  //   | '-' { $$ = make_unique<BinOpAST>(__BinOp_Minus).release(); }
  // ;

RelExp
  : AddExp {
      auto rel_expr = make_unique<RelExpAST>();
      rel_expr->__add_expr_ = ast_uptr($1);
      rel_expr->__sub_expr_type_ = __RelExpr_Add;
      $$ = rel_expr.release();
    } 
    | RelExp CmpOp AddExp {
      auto rel_expr = make_unique<RelExpAST>();
      rel_expr->__rel_expr_ = ast_uptr($1);
      rel_expr->__cmp_op_ = ast_uptr($2);
      rel_expr->__add_expr_ = ast_uptr($3);
      rel_expr->__sub_expr_type_ = __RelExpr_Rel;
      $$ = rel_expr.release();
    }
  ;

EqExp 
  : RelExp {
      auto eq_expr = make_unique<EqExpAST>();
      eq_expr->__rel_expr_ = ast_uptr($1);
      eq_expr->__sub_expr_type_ = __EqExpr_Rel;
      $$ = eq_expr.release();
    }
    | EqExp CmpOp RelExp {
      auto eq_expr = make_unique<EqExpAST>();
      eq_expr->__eq_expr_ = ast_uptr($1);
      eq_expr->__cmp_op_ = ast_uptr($2);
      eq_expr->__rel_expr_ = ast_uptr($3);
      eq_expr->__sub_expr_type_ = __EqExpr_Eq;
      $$ = eq_expr.release();
    }
  ;

CmpOp
  : OPT {
    auto cmp_op = make_unique<CmpOpAST>();
    cmp_op->__operator_ = str_uptr($1);
    $$ = cmp_op.release();
  }
  ;
  // : "<" { $$ = make_unique<CmpOpAST>(__CmpOp_Lt).release(); }
  //   | ">" { $$ = make_unique<CmpOpAST>(__CmpOp_Gt).release(); }
  //   | "<=" { $$ = make_unique<CmpOpAST>(__CmpOp_Le).release(); }
  //   | ">=" { $$ = make_unique<CmpOpAST>(__CmpOp_Ge).release(); }
  //   | "==" { $$ = make_unique<CmpOpAST>(__CmpOp_Eq).release(); }
  //   | "!=" { $$ = make_unique<CmpOpAST>(__CmpOp_Ne).release(); }

LAndExp 
  : EqExp {
      auto land_expr = make_unique<LAndExpAST>();
      land_expr->__eq_expr_ = ast_uptr($1);
      land_expr->__sub_expr_type_ = __LAndExpr_Eq;
      $$ = land_expr.release();
    } 
    | LAndExp OPT EqExp {
      auto land_expr = make_unique<LAndExpAST>();
      land_expr->__land_expr_ = ast_uptr($1);
      land_expr->__operator_ = str_uptr($2);
      land_expr->__eq_expr_ = ast_uptr($3);
      land_expr->__sub_expr_type_ = __LAndExpr_LAnd;
      $$ = land_expr.release();
    }
  ;

LOrExp 
  : LAndExp {
      auto lor_expr = make_unique<LOrExpAST>();
      lor_expr->__land_expr_ = ast_uptr($1);
      lor_expr->__sub_expr_type_ = __LOrExpr_LAnd;
      $$ = lor_expr.release();
    }
    | LOrExp OPT LAndExp {
      auto lor_expr = make_unique<LOrExpAST>();
      lor_expr->__lor_expr_ = ast_uptr($1);
      lor_expr->__operator_ = str_uptr($2);
      lor_expr->__land_expr_ = ast_uptr($3);
      lor_expr->__sub_expr_type_ = __LOrExpr_LOr;
      $$ = lor_expr.release();
    }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
