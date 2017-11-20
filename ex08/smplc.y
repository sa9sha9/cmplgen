%{
using namespace std;

/*******************************************************************
この次の行に、課題作成に参加したメンバ全員の学籍番号を記入すること

 s1180108 中野　慈
 s1220170 大滝　寛人
 s1230073 櫻井　俊輔
 s1230150 影山　尚登

*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include "const.h"
#include "code.h"
#include "symtable.h"
#include "interpret.h"
#include "scanner.h"
#include "error.h"

// 関数のプロトタイプ宣言
template <typename Tnum>
static int allocateGlobalVar(VarEntry *var, Tnum val);
static void allocateArray(VarEntry *var);
static int allocateGlobalIntVar(int val);
static int allocateGlobalIntVar(double val);
static int allocateGlobalRealVar(int val);
static int allocateGlobalRealVar(double val);
static AssignTree *makeAssignTree(VarEntry *var,
                                  ExprTree *expr, ExprTree *index);
static ExprTree *makeBinExprTree(CConst op, ExprTree *lexp, ExprTree *rexp);
static RelationTree *makeRelationTree(CConst op, ExprTree *e1, ExprTree *e2);

/*
   静的変数
*/
static Code *compiled;  // 中間コードを保存する変数

// 相続属性
VarEntry *var;          // vinit の相続属性
%}

%union{
  CConst op;            // 演算子を表す定数
  string *symbol;       // 識別子名へのポインタ
  VarEntry *var;        // 変数エントリへのポインタ
  STree *tree;          // 構文木へのポインタ
  ExprTree *expression; // 式の構文木へのポインタ
  int inum;             // 整数
  double rnum;          // 実数
  Code *code;           // 中間コードへのポインタ
  CondList *condlist;   // 条件と文リストのペアの列
  CondTree *condition;  // 条件の構文木へのポインタ
  Type type;            // 変数の型
  int loc;              // 番地
}

%token <symbol> ID
%token <inum> INUM
%token <rnum> RNUM
%token <op> ADDOP MULOP RELOP LOGOP ULOGOP SIGNOP
%token IF ELSIF ELSE WHILE REPEAT RETURN
%token VOID INT REAL

%type <type> type
%type <inum> iconst
%type <rnum> rconst
%type <tree> stmt
%type <expression> expr
%type <condition> cond
%type <condlist> ifPart
%type <code> stmtList elsePart
%type <loc> vinit
%type <var> vname


%left LOGOP
%nonassoc ULOGOP
%left ADDOP
%left MULOP
%nonassoc SIGNOP

%%

// プログラム → 宣言部 代入部
program
  : declPart stmtList { compiled = $2; }
  ;

// 宣言部 → ε | 宣言部 宣言
declPart
  : /* 空列 */
  | declPart decl
  ;

// 宣言 → type ID 変数初期化子 ';' | type ID '[' INUM ']' ';'
decl
  : type ID
      { $<var>$ = addGlobalVariable(*$2,$1); var = $<var>$; }
    vinit ';'
      { $<var>3->setLocation($4); registerVariable($<var>3);
        delete $2; }
  | type ID '[' INUM ']' ';'
      { allocateArray(addArray(*$2,$1,$4)); delete $2; }
  ;

// 変数初期化子 → ε | '=' 整数定数 | '=' 実数定数
vinit
  : /* 空列 */ { $$ = allocateGlobalVar(var,0); }
  | '=' iconst
      { $$ = allocateGlobalVar(var,$2); }
  | '=' rconst
      { $$ = allocateGlobalVar(var,$2); }
  ;

// 整数定数 → INUM | SIGNOP INUM
iconst
  : INUM                    { $$ = $1; }
  | ADDOP INUM %prec SIGNOP { $$ = $1 == Cadd ?  $2 : -$2; }
  ;

// 実数定数 → RNUM | SIGNOP RNUM
rconst
  : RNUM                    { $$ = $1; }
  | ADDOP RNUM %prec SIGNOP { $$ = $1 == Cadd ?  $2 : -$2; }
  ;

// 型 → INT | REAL
type
  : INT  { $$ = TInt; }  // 整数型を表すType型の定数TIntをtypeの合成属性に代入
  | REAL { $$ = TReal; } // 実数型を表すType型の定数TRealをtypeの合成属性に代入
  ;

// 文リスト → ε | 文リスト 文
stmtList
  : /* 空列 */    { $$ = new Code; }
  | stmtList stmt { $1->push_back($2); $$ = $1; }
  ;

// 文 → ID '=' 式 ';' | ID '[' 式 ']' '=' 式 ';' | if部 else部
//     | WHILE '(' 条件 ')' '{' 文リスト '}'
//     | REPEAT '(' 式 ')' '{' 文リスト '}'
// Yacc では、最初の２つの右辺中の ID は、非終端記号 vname で
// 置き換えられる。
stmt
  : vname '='

    { if( $1->isArray() ) compileError( EDeclaredAsArray, $1->getName().c_str() ); } // vnameの属性varの値である変数エントリが配列の時compileErrorを呼び出す
    expr ';'
    { $$ = makeAssignTree($1, $4, NULL); } // makeAssignTreeを呼び、代入文の構文木を生成して、そのポインタをstmtの合成属性に代入
  | vname '['
    { if( !( $1->isArray() ) ) compileError( EDeclaredAsSimpleVar, $1->getName().c_str() ); } // vnameの属性varの値である変数エントリが、compileErrorを呼び出す
    expr ']'
    { if( ( $4->getType() ) != TInt ) compileError( EIndexTypeMismatch, $1->getName().c_str() ); } // exprの属性expressionの値が整数型以外の時、エラー処理を実行
    '=' expr ';'
    {$$ = makeAssignTree($1,$8,$4); } // 関数makeAssignTree()を呼び代入文の構文木を生成し、それへのポインタをstmtの合成属性に代入.
  | ifPart elsePart { $$ = new IfTree($1,$2); }
  | WHILE '(' cond ')' '{' stmtList '}' { $$ = new WhileTree($3,$6); }
  | REPEAT '(' expr ')'
    {if(($3->getType())!=TInt) compileError(ERepeatTypeMismatch); } // exprの属性expressionの値が、整数型以外の時、エラー処理を実行
    '{' stmtList '}' { $$ = new RepeatTree($3,$7); }
  ;

// if部 → IF '(' 条件 ')' '{' 文リスト '}' |
//         if部 ELSIF '(' 条件 ')' '{' 文リスト '}'
ifPart
  : IF '(' cond ')' '{' stmtList '}'
      { $$ = new CondList;
        $$->push_back(make_pair($3,$6)); }
  | ifPart ELSIF '(' cond ')' '{' stmtList '}'
      { $$->push_back(make_pair($4,$7)); }
  ;

// else部 → ε | ELSE '{' 文リスト '}'
elsePart
  : /* 空列 */  { $$ = NULL; }
  | ELSE '{' stmtList '}' { $$ = $3; }
  ;

// 式 → 式 ADDOP 式 | 式 MULOP 式 | SIGNOP 式 | '(' 式 ')'
//     | ID | ID '[' 式 ']' | INUM | RNUM
// Yacc では、右辺に現れる２つの ID は、非終端記号 vname で
// 置き換えられる。
expr
  : expr ADDOP expr { $$ = makeBinExprTree($2,$1,$3); }
  | expr MULOP expr { $$ = makeBinExprTree($2,$1,$3); }
  | ADDOP expr %prec SIGNOP { $$ = new UniExprTree($1,$2); }
  | '(' expr ')' { $$ = $2; }
  | vname
   { if( $1->isArray() ) compileError( EDeclaredAsArray, $1->getName().c_str() ); // vnameの属性varの値である変数エントリが配列の時compileErrorを呼び出す
    else $$ = new SmplVarNode( $1->getName(), $1->getLocation(), $1->getType()); } // 変数エントリが変数だった時、単純変数の構文木を生成し、そのポインタをexprの合成属性に代入
  | vname '['
    { if( !( $1->isArray() ) ) compileError( EDeclaredAsSimpleVar, $1->getName().c_str() ); } // vnameの属性varの値である変数エントリが配列の時compileErrorを呼び出す
    expr ']'
    { if( ( $4->getType() ) != TInt ) compileError( EIndexTypeMismatch, $1->getName().c_str() ); // exprの属性expressionの値が、整数型以外の時エラー処理を実行
     else $$ = new ArrayElemTree( $1->getName(), $1->getLocation(), $1->getType(), $4, $1->getArraySize() ); } // 変数エントリが配列でタイプが整数型だった時、配列要素の構文木を生成し、ポインタをexprの合成属性に代入
  | INUM { $$ = new INumNode($1); } // 整数の構文木を生成し、そのポインタをexprの合成属性に代入
  | RNUM { $$ = new RNumNode($1); } // 実数の構文木を生成し、そのポインタをexprの合成属性に代入
  ;

// stmt と expr の右辺中の ID を置き換えたもの
vname
  : ID { $$ = findVariable(*$1); } // IDの属性値である識別子名を持つ変数を記号表から探し、見つかればそのポインタをvnameの合成属性に代入する
  ;

// 条件 → 条件 LOGOP 条件 | ULOGOP 条件 | 式 RELOP 式 | '(' 条件 ')'
cond
  : cond LOGOP cond { $$ = new BinCondTree($2,$1,$3); }
  | ULOGOP cond     { $$ = new UniCondTree($1,$2); }
  | expr RELOP expr { $$ = makeRelationTree($2,$1,$3); }
  | '(' cond ')'    { $$ = $2; }
  ;

%%

template <typename Tnum>
static int allocateGlobalVar(VarEntry *var, Tnum val)
{
  switch (var->getType())  {
  case TInt:
    return allocateGlobalIntVar(val);
  case TReal:
    return allocateGlobalRealVar(val);
  default:
    compileError(EVoidVariable,var->getName().c_str());
  }
}

static int allocateGlobalIntVar(int val)
{
  return allocateMemory(val);
}

static int allocateGlobalIntVar(double val)
{
  return allocateMemory((int)val);
}

static int allocateGlobalRealVar(int val)
{
  return allocateMemory((double)val);
}

static int allocateGlobalRealVar(double val)
{
  return allocateMemory(val);
}

static void allocateArray(VarEntry *var)
{
  Type type = var->getType();
  int size = var->getArraySize();
  if (type == TVoid)
    compileError(EVoidVariable,var->getName().c_str());
  else  {
    if (size == 0)
      compileError(EIllegalSize,var->getName().c_str());
    var->setLocation(allocateGlobalVar(var,0));
    for (int i=1; i<size; i++)
      allocateGlobalVar(var,0);
    registerVariable(var);
  }
}

// 構文木を生成
static AssignTree *makeAssignTree(VarEntry *var,
                                  ExprTree *expr, ExprTree *index)
{
  // indexがNULLの時、単純変数の構文木を生成
  if(index == NULL){
    // 代入文の左辺の単純変数の構文木svarの生成
    SmplVarNode *svar = new SmplVarNode(var->getName(), var->getLocation(), var->getType());

    // 代入文の左辺の構文木varが整数型で、右辺の構文木exprが実数型のとき
    if(var->getType()==TInt && expr->getType()==TReal){
      UniExprTree *iexp = new UniExprTree(Creal2int, expr, TInt); // exprを引数とし、単項演算子real2intを持つ整数型の構文木iexpを生成
      return new AssignTree(svar, iexp); // svarとiexpを要素とする代入文の構文木を生成
    }
      // 代入文の左辺の構文木varが実数型、右辺の構文木exprが整数型のとき
    else if(var->getType()==TReal && expr->getType()==TInt){
      UniExprTree *rexp = new UniExprTree(Cint2real, expr, TReal); // exprを引数とする単項演算子int2realを持つ実数型の構文木rexpを生成
      return new AssignTree(svar, rexp); // svarとrexpを要素とする代入文の構文木を生成
    }
    else return new AssignTree(svar,expr); // 両辺とも同じ型の時、svarとexprを要素とする代入文の構文木を生成
  }
  else{ // indexがNULL以外の時、配列要素の構文木を生成
    // 代入文の左辺の配列要素の構文木avarを生成
    ArrayElemTree *avar = new ArrayElemTree(var->getName(), var->getLocation(), var->getType(), index, var->getArraySize());

    // 代入文の左辺の構文木varが整数型、右辺の構文木exprが実数型のとき
    if(var->getType()==TInt && expr->getType()==TReal){
      UniExprTree *iexp = new UniExprTree(Creal2int, expr, TInt); // exprを引数とする単項演算子real2intを持つ整数型の構文木iexpを生成
      return new AssignTree(avar,iexp); // avarとiexpを要素とする代入文の構文木を生成
    }
    else if(var->getType()==TReal && expr->getType()==TInt){ // 代入文の左辺の構文木varが実数型、右辺の構文木exprが整数型のとき
      UniExprTree *rexp = new UniExprTree(Cint2real, expr, TReal); // exprを引数とする単項演算子int2realを持つ実数型の構文木rexpを生成
      return new AssignTree(avar, rexp); // avarとrexpを要素とする代入文の構文木を生成
    }
    else return new AssignTree(avar, expr); //両辺とも同じ型の時、avarとexprを要素とする代入文の構文木を生成
  }
}


// 式の構文木を生成
static ExprTree *makeBinExprTree(CConst op, ExprTree *lexp, ExprTree *rexp)
{
  if(lexp->getType()==TInt && rexp->getType()==TReal){ // 左辺の式の構文木lexpが整数型、右辺の式の構文木rexpが実数型のとき
    UniExprTree *cvexp = new UniExprTree(Cint2real,lexp,TReal); // lexpを引数とする単項演算子int2realを持つ実数型の構文木cvexpを生成
    return new BinExprTree(op,cvexp,rexp); // opとcvexpとrexpを要素とする二項演算子をもつ式の構文木を生成
  }

  else if(lexp->getType()==TReal && rexp->getType()==TInt){// 左辺の式の構文木lexpが実数型で、右辺の式の構文木rexpが整数型のとき
    UniExprTree *cvexp = new UniExprTree(Cint2real,rexp,TReal);// rexpを引数とする単項演算子int2realを持つ実数型の構文木cvexpを生成
    return new BinExprTree(op,lexp,cvexp);// opとlexpとcvexpを要素とする二項演算子をもつ式の構文木を生成
  }

  else return new BinExprTree(op,lexp,rexp);// 両辺とも同じ型の時、opとlexpとrexpを要素とする二項演算子をもつ式の構文木を生成
}

// 条件の構文木を生成
static RelationTree *makeRelationTree(CConst op, ExprTree *e1, ExprTree *e2)
{
  if(e1->getType()==TInt && e2->getType()==TReal){// 左辺の式の構文木e1が整数型で、右辺の式の構文木e2が実数型のとき
    UniExprTree *cvexp = new UniExprTree(Cint2real,e1,TReal);// e1を引数とする単項演算子int2realを持つ実数型の構文木cvexpを生成
    return new RelationTree(op,cvexp,e2);// opとcvexpとe2を要素とする関係演算子をもつ式の構文木を生成
  }

  else if(e1->getType()==TReal && e2->getType()==TInt){// 左辺の式の構文木e1が実数型、右辺の式の構文木e2が整数型のとき
    UniExprTree *cvexp = new UniExprTree(Cint2real,e2,TReal);// e2を引数とする、 単項演算子int2realを持つ実数型の構文木cvexpを生成
    return new RelationTree(op,e1,cvexp);// opとe1とcvexpを要素とする関係演算子をもつ式の構文木を生成
  }

  else return new RelationTree(op,e1,e2);// 両辺とも同じ型の時、opとe1とe2を要素とする関係演算子をもつ式の構文木を生成
}

void usage(char *cmd)
{
  fprintf(stderr,"利用法: %s 原始プログラムファイル名\n",cmd);
  exit(2);
}

main(int argc, char **argv)
{
  char *cmd = *argv;    // コンパイラのコマンド名
#if YYDEBUG
  extern int yydebug;

  yydebug = 1;  /* デバッグ表示付きの構文解析系を生成したいとき 1 を代入 */
#endif

  if (argc < 2)
    usage(cmd);

  setFileName(*++argv);
  initializeScanner(*argv);
  yyparse();
  interpret(compiled);
}
