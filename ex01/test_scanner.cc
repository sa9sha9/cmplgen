using namespace std;

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "const.h"
#include "scanner.h"
#include "error.h"
#include "error.cc"
#include "y.tab.h"

// 定数の定義
#define FOREVER	1

#define isWhiteSpace(c)	(c == ' ' || c == '\t' || c == '\n')
static int getCharacter(void);
static int getIdentifier( int );
void initializeScanner(char*);

int lineNo;	// 処理中の行の行番号(scaner.ccとerror.ccで使用)

/**/static int cnt = 0;
static FILE *srcFilePointer;	// 原始プログラムのファイルディスクリプタ
static int currentChar;		// 先読み文字バッファ

#ifdef YYDEBUG
extern int yydebug;
#endif

YYSTYPE yylval;

int main()
{
    initializeScanner((char *)"hoge.txt");

    int token;

    token = yylex();

    printf("token number = %d\n", token);

    switch( token ) {
        case 258:
            printf("ID\n");
            break;
        case 263:
            printf("ADDOP\n");
            break;
        case 264:
            printf("MULOP\n");
            break;
        case 266:
            printf("LOGOP\n");
            break;
        default:
            break;
    }

    return 0;
}

void initializeScanner(char *filename)
{
  FILE *fp;
  fp = fopen(filename, "r");
  if(fp == NULL) {
    printf("%s file not found!\n", filename);
    errorExit(EFileNotFound, filename);
  } else {
    printf("%s file opened!\n", filename);
    srcFilePointer = fp;
    currentChar = getCharacter();
    lineNo = 1;
  }
}
/* 字句読み取りモジュール                                */
/* 返り値: トークンを表す定数                            */
/* 副作用: 共用体 yylval に、必要に応じて属性を代入      */
/* 概要:   原始プログラムから文字をよみ，字句を切り出す．*/
/*         読み込んだ文字に応じてトークンの種類を決定し，*/
/*         トークンを返す．またトークンの属性があれば    */
/*         それをyylvalに格納する．                      */
int yylex()
{
START:
    // 空白の読み飛ばし
    while (isWhiteSpace(currentChar))  {
        currentChar = getCharacter();
    }
    // この時点では currentChar には空白以外の文字が入っている

    // 識別子の取得（ currntChar が英字であった場合 ）
    if ( isalnum( currentChar ) )  {
        //return ID;
        return getIdentifier(currentChar);
    }
    // 加減算演算子（+と-）
    //else if ( currentChar == Cadd || currentChar == Csubtract ) {
    if ( currentChar == Cadd || currentChar == Csubtract ) {
        return ADDOP;
    }

    // 乗除算演算子（*と%）
    else if ( currentChar == Cmultiply || currentChar == Cmodulo ) {
        return MULOP;
    }

    else if ( currentChar == Cdivide ) {
        currentChar = getCharacter();

        if ( currentChar == Cdivide ) {
            while ( 1 ) {
                currentChar = getCharacter();
                if ( currentChar == '\n' ) {
                    goto START;
                }
                else if (currentChar == EOF)  {
                    return EOF;
                }
            }
        }

        return MULOP;
    }

    // 論理演算子&&
    else if ( currentChar == Cand ) {
        currentChar = getCharacter();

        if ( currentChar != Cand ) {
            compileError( EIllegalChar, currentChar, currentChar );
        }

        return LOGOP;
    }

    // 論理演算子||
    else if ( currentChar == Cor ) {
        currentChar = getCharacter();

        if ( currentChar != Cor ) {
            //compileError( EIllegalChar, currentChar, currentChar );
            printf("compileError\n");
        }

        return LOGOP;
    }

    // ファイルの終わりを表すEOFを読んだとき
    else if (currentChar == EOF)  {
        return EOF;
    }
    // その他の文字は不正な文字なのでエラー
    else  {
        compileError(EIllegalChar,currentChar,currentChar);
    }

    return 0;
}

/*
static int getCharacter()
{
    switch ( cnt ) {
        case 0:
            cnt++;
            return '&';
            break;
        case 1:
            cnt++;
            return 'b';
            break; 
        case 2:
            cnt++;
            return 'c';
            break; 
        case 3:
            cnt++;
            return ' ';
            break; 
    }

    return 0;
}
*/

// 文字読み取りモジュール
//  返り値: ファイルから読んだ文字
//  副作用: 改行文字を読んだとき lineNo が1増加
static int getCharacter()
{
    char tmp;    //影山 10.05_01

    tmp = getc( srcFilePointer );
    if(tmp == '\n')
        lineNo++;
    printf("%c", tmp);
    return tmp;
    /*
    char tmp;    //影山 10.05_01
    fscanf(srcFilePointer, "%c", &tmp);
    if(tmp == '\n')
        lineNo++;
    printf("%c\n", tmp);
    ++srcFilePointer;
    return tmp;
    */
}

// 識別子を取得する
//   c: 識別子の１文字目（英字）
//   返り値: トークン ID
//   副作用: yylval.symbol に字句へのポインタを代入
static int getIdentifier(int c)
{
    /**
    ここで，識別子の字句を読み取る有限オートマトンをシミュレートする．
    字句を保存するための局所変数を用意すること．
    字句へのポインタを yylval.symbol に代入し，識別子のトークンを返す．
	*/
	
	string str;
	
	str += c;
	currentChar = getCharacter();

    // 英数字が続く限り字句を蓄積
	while( isalnum( currentChar ) ){
		
		str += currentChar;
		currentChar = getCharacter();
		
	}
	
  yylval.symbol = &str;//グローバル変数 yylval に字句を保存
                  //yylval.symbol の型は y.tab.h を参照のこと．
  return ID;
}

