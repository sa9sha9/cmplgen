/*******************************************************************
この次の行に、課題作成に参加したメンバ全員の学籍番号を記入すること


 s1180108 中野　慈
 s1220170 大滝　寛人
 s1230073 櫻井　俊輔
 s1230150 影山　尚登


*******************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "lf-scanner.h"
#include "token.h"
#include "error.h"
#include "parse.h"

using namespace std;


// static関数のプロトタイプ宣言
static bool parseLF(void);
static bool parseLF2(bool);
static bool parseNoImpl(void);
static bool parseNoImpl2(bool);
static bool parseSimpleLF(void);
static void match(int x);
static void parseError(void);

// トークンを保存するstatic変数
static int token;

/**
 * IMPL	 ‒>	含意記号（ならば）
 * AND	 &	論理積記号（かつ）
 * OR	 |	論理和記号（または）
 * NOT	 !	否定記号（でない）
 * TRUE	 t	真（true）
 * FALSE f	偽（false）
 * '('	 (	開きカッコ
 * ')'	 )	閉じカッコ
 */


// 構文解析系のトップレベル関数
//   引数: なし
//   返り値: 解析した論理式の真理値
//   副作用: なし
bool parse()
{
  bool b;

  token = getToken();	// 最初のトークンを先読み
  b = parseLF();	// 構文解析の開始

  // EOFでなければ余分なトークンが残っていたことになるのでエラー
  if (token != EOF)  {
    compileError(ETooMuchTokens,getLexeme(token));
    // getLexeme() のプロトタイプ宣言は lf-scanner.h にある
  }
  else
    return b;
}

/** ボーナス点を狙うチームはここから parseNoImpl2()の定義までコメントアウトし、
    手続きを統合した parseLF() と parseNoImp() を新たに定義する。
**/

// 「論理式」の構文解析を行い，その真理値を返す
//   引数: なし
//   返り値: 解析した論理式の真理値
//   副作用: なし
bool parseLF()
{
}

// 「論理式」の構文解析を行い，その真理値を返す
//   引数: トークンIMPLの前に現れる論理式の真理値
//   戻り値: 引数と構文解析の結果から決まる真理値
//   副作用: なし
bool parseLF2(bool b)
{
}

// 「含意無し論理式」の構文解析を行い，その真理値を返す
//   引数: なし
//   返り値: 解析した含意無し論理式の真理値
//   副作用: なし
bool parseNoImpl()
{
}

// 「含意無し論理式２」の構文解析を行い，その真理値を返す
//   引数: トークンAND, ORの前に現れる論理式の真理値
//   戻り値: 引数と構文解析の結果から決まる真理値
//   副作用: なし
bool parseNoImpl2(bool b)
{
}
/** ボーナス点を狙うチームは、parseLF()の定義からここまでをコメントアウトする **/

// 「単純論理式」の構文解析を行い，その真理値を返す
//   引数: なし
//   返り値: 解析した単純論理式の真理値
//   副作用: なし
bool parseSimpleLF()
{
    bool b;

    switch(token) {
        case NOT:
            // NOT 単純論理式
            match(NOT);
            // 次のtokenは '単純論理式'
            parseSimpleLF();
            break;
        case '(':
            // ( 論理式 )
            match('(');
            parseLF();
            break;
        case TRUE:
            // TRUE
            match(TRUE);
            b = TRUE;
            break;
        case FALSE:
            // FALSE
            match(FALSE);
            b = FALSE;
            break;
        default:
            b = FALSE;
            break;
    }

    return b;
}

// 読み込んだトークンが期待しているトークンと一致しているかどうか調べる
//   引数: 期待しているトークン x
//   返り値: なし
//   副作用: トークンが一致したら、token に次のトークンが格納される。
//           一致しなければエラー終了し、呼び出し側には戻らない。
void match(int x)
{
  if (x == token)  {
    token = getToken();		// 一致すれば次のトークンを先読み
  }
  // EOF を読み込んだ
  else if (token == EOF) {
    compileError(EUnexpectedEOF);
  }
  // EOFではないが，x と異なるトークンを読み込んだ
  else
    compileError(EIllegalToken,getLexeme(x),getLexeme(token));
}

// 構文解析のエラー処理
//   引数: なし
//   返り値: なし
//   副作用: つねにエラー終了し、呼び出し側には戻らない。
void parseError()
{
  if (token == EOF)  {
    // EOF のときは，予期しないファイルの終りに遭遇したことを知らせる
    compileError(EUnexpectedEOF);
  }
  else  {
    // それ以外のときは，単純論理式を始めるトークンでないものを
    // 読んだことを知らせる
      compileError(ENotSimpleLF,getLexeme(token));
  }
}
