%{
using namespace std;

#include <string>
#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "error.h"

// yyparse() 内だけで有効な関数のプロトタイプ宣言

static int yyerror(const char *msg);	// yyparse()が用いるエラー処理関数
static void usage(char *cmd);		// 使用法の表示
%}


// 属性とその型の宣言

%union{
  int num;
  string *str;
}


// トークンの宣言

%token <str> STR
%token <num> NUM


// 非終端記号の宣言

%type <num> body body2 elem expr


// 優先順位と結合性の宣言

%left '+'	/* + は左結合 */
%left '*'	/* * は左結合 */


%%

list: '[' body ']'	{ printf("\nThe length of list is %d\n",$2); }
    ;

body :			{ $$ = 0; }
     | body2		{ $$ = $1; }
     ;

body2: elem		{ $$ = $1; }
     | body2 ',' { printf(","); } elem	{ $$ = $1 + $4; }
     ;

elem: STR		{ printf("%s",$1->c_str()); $$ = 1; }
    | STR '!' expr	{ int i;
                          if ($3 <= 0)
                            compileError(EIllegalExpValue,$3);
                          else  {
			    printf("%s",$1->c_str());
			    for (i=1; i<$3; i++)
			      printf(",%s",$1->c_str());
			  }
			  $$ = $3;
			}
    ;

expr: expr '+' expr	{ $$ = $1 + $3; }
    | expr '*' expr	{ $$ = $1 * $3; }
    | NUM		{ $$ = $1; }
    | '(' expr ')'	{ $$ = $2; }
    ;


%%

// yyparse()が用いるエラー処理用関数
static int yyerror(const char *msg)
{
  compileError(EParseError);
}


// メイン関数
main(int argc, char **argv)
{
#if YYDEBUG
  extern int yydebug;
  yydebug = 1;
#endif

  // 引数が１つでなければ使用法を表示して終了
  if (argc != 2)  {
    usage(*argv);
  }
  initializeScanner(*++argv);	// ファイル名を引数として字句解析系を初期化

  // 構文解析を行い，結果を表示する．結果の表示はyyparse()の中で行う．
  yyparse();
}

// コマンドの使用法を表示して終了する関数
void usage(char *cmd)
{
  // cmd からパス名を除き，コマンド名だけにしたものを表示する
  char *p = strrchr(cmd,'/');	// 最後に現れる / を見つける

  /* 最後に現れる / 以降をコマンド名として表示する
     cmd 中に / がなければ，全体をコマンド名として表示する */
  fprintf(stderr,"usage: %s filename\n", (p ? p+1 : cmd));
  exit(1);
}

