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
bool parse() {
    bool b;

    token = getToken();    // 最初のトークンを先読み
    b = parseLF();    // 構文解析の開始

    // EOFでなければ余分なトークンが残っていたことになるのでエラー
    if (token != EOF) {
        compileError(ETooMuchTokens, getLexeme(token));
        // getLexeme() のプロトタイプ宣言は lf-scanner.h にある
    } else
        return b;
}

/** ボーナス点を狙うチームはここから parseNoImpl2()の定義までコメントアウトし、
    手続きを統合した parseLF() と parseNoImp() を新たに定義する。
**/

// 「論理式」の構文解析を行い，その真理値を返す
//   引数: なし
//   返り値: 解析した論理式の真理値
//   副作用: なし
bool parseLF() {
    bool bl, tmp;

    switch (token) {
        case NOT:
            tmp = parseNoImpl();
            bl = parseLF2(tmp);
            break;
        case '(':
            tmp = parseNoImpl();
            bl = parseLF2(tmp);
            break;
        case TRUE:
            tmp = parseNoImpl();
            bl = parseLF2(tmp);
            break;
        case FALSE:
            tmp = parseNoImpl();
            bl = parseLF2(tmp);
            break;
        default:
            parseError();
            break;
    }

    return bl;
}


// 「論理式」の構文解析を行い，その真理値を返す
//   引数: トークンIMPLの前に現れる論理式の真理値
//   戻り値: 引数と構文解析の結果から決まる真理値
//   副作用: なし
bool parseLF2(bool b) {
    bool bl;

    switch (token) {
        case IMPL:
            // IMPL 論理式
            /**
             * A->Bについて、
             * A  B A⇒B
             * 真 真 真
             * 真 偽 偽
             * 偽 真 真
             * 偽 偽 真
             * ( !A || B : Aが成り立たない、またはBが成り立つ ) と言い換えることもできる.
             */
            match(IMPL);
            bl = !b || parseLF(); // bがfalse、またはparseLFの結果がtrueの時、blがtrueになる.
            break;
        case EOF:
            // 空語の場合は一つ前の論理式の真理値そのまま
            bl = b;
            break;
        default:
            parseError();
            break;
    }

    return bl;
}

// 「含意無し論理式」の構文解析を行い，その真理値を返す
//   引数: なし
//   返り値: 解析した含意無し論理式の真理値
//   副作用: なし
bool parseNoImpl() {
    bool bl;

    // 最初はとりあえず単純論理式の展開をする ( first(含意なし論理式)はすべてfirst(単純論理式)が実体だし.. )
    switch (token) {
        case NOT:
            bl = parseSimpleLF();
            break;
        case '(':
            bl = parseSimpleLF();
            break;
        case TRUE:
            bl = parseSimpleLF();
            break;
        case FALSE:
            bl = parseSimpleLF();
            break;
        default:
            parseError();
            break;
    }

    // 単純論理式が展開できたら、後に続く含意なし論理式２を展開する.
    bl = parseNoImpl2(bl); // switchの各case内でやってもいいけど、外に出したほうがいいね

    return bl;
}

// 「含意無し論理式２」の構文解析を行い，その真理値を返す
//   引数: トークンAND, ORの前に現れる論理式の真理値
//   戻り値: 引数と構文解析の結果から決まる真理値
//   副作用: なし
bool parseNoImpl2(bool b) {
    bool bl, putty; // puttyはサンドイッチの肉

    // todo: C++の論理式の解釈仕様を考慮してやる必要がある？

    switch (token) {
        case AND:
            // OR 単純論理式 含意無し論理式2
            match(AND);
            putty = parseSimpleLF();
            // bがtrueかつ、parseNoImpl2で計算された真理値がtrueなら、blはtrue.
            // (例えばparseNoImpl2が空後ならputtyの真理値そのままの値と&&することになる.)
            bl = b && parseNoImpl2(putty);
            break;
        case OR:
            // AND 単純論理式 含意無し論理式2
            match(OR);
            putty = parseSimpleLF();
            // bがtrueかもしくは、parseNoImpl2で計算された真理値がtrueなら、blはtrue.
            bl = b || parseNoImpl2(putty);
            break;
        case EOF:
            // 空後
            bl = b; // 一つ前の論理式の真理値そのまま
            break;
        default:
            parseError();
            break;
    }

    return bl;
}

/** ボーナス点を狙うチームは、parseLF()の定義からここまでをコメントアウトする **/

// 「単純論理式」の構文解析を行い，その真理値を返す
//   引数: なし
//   返り値: 解析した単純論理式の真理値
//   副作用: なし
bool parseSimpleLF() {
    bool bl;

    switch (token) {
        case NOT:
            // NOT 単純論理式
            match(NOT);
            // 次のtokenは '単純論理式'
            bl = !parseSimpleLF(); // 単純論理式の値にNOT演算をする
            break;
        case '(':
            // ( 論理式 )
            match('(');
            bl = parseLF();
            match(')');
            break;
        case TRUE:
            // TRUE
            match(TRUE);
            bl = true;
            break;
        case FALSE:
            // FALSE
            match(FALSE);
            bl = false;
            break;
        default:
            parseError();
            break;
    }

    return bl;
}

// 読み込んだトークンが期待しているトークンと一致しているかどうか調べる
//   引数: 期待しているトークン x
//   返り値: なし
//   副作用: トークンが一致したら、token に次のトークンが格納される。
//           一致しなければエラー終了し、呼び出し側には戻らない。
void match(int x) {
    if (x == token) {
        token = getToken();        // 一致すれば次のトークンを先読み
    }
        // EOF を読み込んだ
    else if (token == EOF) {
        compileError(EUnexpectedEOF);
    }
        // EOFではないが，x と異なるトークンを読み込んだ
    else
        compileError(EIllegalToken, getLexeme(x), getLexeme(token));
}

// 構文解析のエラー処理
//   引数: なし
//   返り値: なし
//   副作用: つねにエラー終了し、呼び出し側には戻らない。
void parseError() {
    if (token == EOF) {
        // EOF のときは，予期しないファイルの終りに遭遇したことを知らせる
        compileError(EUnexpectedEOF);
    } else {
        // それ以外のときは，単純論理式を始めるトークンでないものを
        // 読んだことを知らせる
        compileError(ENotSimpleLF, getLexeme(token));
    }
}
