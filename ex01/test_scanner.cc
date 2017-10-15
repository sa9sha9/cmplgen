#include <iostream>
using namespace std;

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "const.h"
#include "scanner.h"
#include "error.h"
#include "y.tab.h"

#define isWhiteSpace(c)	(c == ' ' || c == '\t' || c == '\n')
static int currentChar;		// 先読み文字バッファ
static int getCharacter(void);

/**/static int cnt = 0;


int main()
{
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
 /*
  // 空白の読み飛ばし
  while (isWhiteSpace(currentChar))  {
    currentChar = getCharacter();
  }
  */
  // この時点では currentChar には空白以外の文字が入っている

  /**/currentChar = getCharacter();
  // 識別子の取得（ currntChar が英字であった場合 ）
  if ( isalnum( currentChar ) )  {
    return ID;
    //return getIdentifier(currentChar);
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
      //compileError( EIllegalChar, currentChar, currentChar );
      printf("compileError\n");
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
/*
  else if ( currentChar == Csubtract ) {
    
  }

  // ファイルの終わりを表すEOFを読んだとき
  else if (currentChar == EOF)  {
    return EOF;
  }
  // その他の文字は不正な文字なのでエラー
  else  {
    compileError(EIllegalChar,currentChar,currentChar);
  }
*/
    return 0;
}

static int getCharacter()
{
    switch ( cnt ) {
      case 0:
        cnt++;
        return '1';
        break;
      case 1:
        cnt++;
        return '/';
        break; 
      case 2:
        cnt++;
        return EOF;
        break; 
      case 3:
        cnt++;
        return '+';
        break; 
    }

    return 0;
}

