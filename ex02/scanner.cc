using namespace std;

/*******************************************************************
この次の行に、課題作成に参加したメンバ全員の学籍番号を記入すること

 s1180108 中野　慈
 s1220170 大滝　寛人
 s1230073 櫻井　俊輔
 s1230150 影山　尚登

*******************************************************************/

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "const.h"
#include "scanner.h"
#include "error.h"
#include "y.tab.h"


// 定数の定義
#define FOREVER    1

/*		  */
/* 文字判定マクロ */
/*		  */

// 空白文字なら真
#define isWhiteSpace(c)    (c == ' ' || c == '\t' || c == '\n')

// ファイル内部だけで有効な関数のプロトタイプ宣言

static int getIdentifier(int c);

static int getNumber(int c);

static int isReservedWord(string);

static int getCharacter(void);

// プログラム全体で有効な変数

int lineNo;    // 処理中の行の行番号(scaner.ccとerror.ccで使用)

// ファイル内部だけで有効な変数

static FILE *srcFilePointer;    // 原始プログラムのファイルディスクリプタ
static int currentChar;        // 先読み文字バッファ

#ifdef YYDEBUG
extern int yydebug;
#endif

/* 字句解析系を初期化する                              */
/*   filename: 原始プログラム名                        */
/*   返り値: なし                                      */
/*   副作用:   srcFilePointer にファイルポインタを代入 */
void initializeScanner(char *filename) {
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        //printf("%s file not found!\n", filename); // テスト通す時はいらない
        errorExit(EFileNotFound, filename);
    } else {
        //printf("%s file opened!\n", filename);　// テスト通す時はいらない
        srcFilePointer = fp;
        lineNo = 1; //getCharacter()の中でlineNo++しているので順番注意
        currentChar = getCharacter();
    }
    // fcloseはyylexの最後で
}


int yylex() {
    int code; // 文字コードを一時保存

    START:
    // 空白の読み飛ばし
    while (isWhiteSpace(currentChar)) {
        currentChar = getCharacter();
    }
    // この時点では currentChar には空白以外の文字が入っている

    // 識別子または予約語の取得
    if ( isalpha(currentChar) ) {

        return getIdentifier( currentChar );

    // 整数と実数の取得
    } else if ( isdigit(currentChar) ) {
        return getNumber( currentChar );

    // >= と > の取得
    } else if ( currentChar == '>' ) {
        currentChar = getCharacter();

        if ( currentChar != '=' ) {
            yylval.op = CgreaterThan;

            return RELOP;
        }

        yylval.op = CgreaterThanOrEqual;
        currentChar = getCharacter();

        return RELOP;

    // <= と < の取得
    } else if ( currentChar == '<' ) {
        currentChar = getCharacter();

        if ( currentChar != '=' ) {
            yylval.op = ClessThan;

            return RELOP;
        }

        yylval.op = ClessThanOrEqual;
        currentChar = getCharacter();

        return RELOP;

    // == と = の取得
    } else if ( currentChar == '=' ) {
        currentChar = getCharacter();

        if ( currentChar != '=' ) {
            return '=';
        }

        yylval.op = Cequal;
        currentChar = getCharacter();

        return RELOP;

    // != と ! の取得
    } else if ( currentChar == '!' ) {
        currentChar = getCharacter();

        if ( currentChar != '=' ) {
            yylval.op = Cnot;

            return ULOGOP;
        }

        yylval.op = CnotEqual;
        currentChar = getCharacter();

        return RELOP;

    // 文字コードの取得
    } else if ( currentChar == '\'' ) {
        currentChar = getCharacter();

        if ( currentChar == EOF ) {
            compileError( EUnexpectedEOF );
        } else {
            code = currentChar; // 文字コードを保存
        }

        currentChar = getCharacter();

        if ( currentChar != '\'' ) {
            compileError( ETooLongCharacter );
        }

        yylval.inum = code;

        return INUM;

    // ( ) [ ] { } ; , <- それぞれの文字の取得
    } else if ( currentChar == '('
                || currentChar == ')'
                || currentChar == '['
                || currentChar == ']'
                || currentChar == '{'
                || currentChar == '}'
                || currentChar == ';'
                || currentChar == ','
            ) {
        return currentChar;

    // 識別子の取得（ currentChar が英字であった場合 ）
    } else if (isalnum(currentChar)) {
        return getIdentifier(currentChar);
    }
        // 整数と実数
    else if (isdigit(currentChar)) {
        return getNumber(currentChar);
    }
    // 加減算演算子（+）
    if (currentChar == Cadd) {
        yylval.op = Cadd;
        currentChar = getCharacter();

        return ADDOP;

    // 加減算演算子(-)
    } else if (currentChar == Csubtract) {
        yylval.op = Csubtract;
        currentChar = getCharacter();

        return ADDOP;

    // 乗除算演算子(*)
    } else if (currentChar == Cmultiply) {
        yylval.op = Cmultiply;
        currentChar = getCharacter();

        return MULOP;

    //乗除算演算子(%)
    } else if (currentChar == Cmodulo) {
        yylval.op = Cmodulo;
        currentChar = getCharacter();

        return MULOP;

    //乗除算演算子(/)とコメントアウト
    } else if (currentChar == Cdivide) {
        currentChar = getCharacter();

        if (currentChar == Cdivide) {
            // コメントアウトを\nかEOFまで読む
            while (1) {
                currentChar = getCharacter();

                if (currentChar == '\n') {
                    goto START; //yylexの最初に飛ばす
                } else if (currentChar == EOF) {
                    return EOF; //EOFで抜ける
                }
            }
        }

        yylval.op = Cdivide;
        return MULOP;

    // 論理演算子(&&)
    } else if (currentChar == Cand) {
        currentChar = getCharacter();

        if (currentChar != Cand) {
            compileError(EIllegalChar, currentChar, currentChar);
        }

        yylval.op = Cand;
        currentChar = getCharacter();

        return LOGOP;

    // 論理演算子(||)
    } else if (currentChar == Cor) {
        currentChar = getCharacter();

        if (currentChar != Cor) {
            compileError(EIllegalChar, currentChar, currentChar);
        }

        yylval.op = Cor;
        currentChar = getCharacter();

        return LOGOP;

        // ファイルの終わりを表すEOFを読んだとき
    } else if (currentChar == EOF) {
        return EOF;

        // その他の文字は不正な文字なのでエラー
    } else {
        compileError(EIllegalChar, currentChar, currentChar);
    }

    fclose(srcFilePointer);

    return 0;
}


/* 字句が予約語かどうか判定する                               */
/*   lexim: 字句                                              */
/*   返り値: トークンを表す定数（leximが予約語を表す字句なら，*/
/*           その予約語のためのトークンを表す定数を，         */
/*           それ以外なら識別子トークンを表す定数を返す）     */
static int isReservedWord(string lexim) {
    static struct rword {
        int token;
        string lexim;
    } reservedWordTable[] = {
            {INT,    "int"},
            {REAL,   "real"},
            {VOID,   "void"},
            {IF,     "if"},
            {ELSIF,  "elsif"},
            {ELSE,   "else"},
            {WHILE,  "while"},
            {REPEAT, "repeat"},
            {RETURN, "return"}
    };
    // size は予約語表 reservedWordTable の大きさ
    const int size = sizeof(reservedWordTable) / sizeof(struct rword);

    /*
    字句 lexim が予約語なら，対応する予約語のトークンを返す．
    そうでなければ，識別子トークン ID を返す．
    */
    for (int i = 0; i < size; i++)    //01 影山 10.12
        if (lexim == reservedWordTable[i].lexim)
            return reservedWordTable[i].token;

    return ID;
}


/* 識別子を取得する                                 */
/*   c: 識別子の１文字目（英字）                    */
/*   返り値: トークン（ID か予約語のトークン）      */
/*   副作用: yylval.symbol に字句へのポインタを代入 */
static int getIdentifier(int c) {
    /*
    課題１で作った getIdentifier() の本体をここにコピーする。
    その後、予約語または識別子のトークンを返すように変更する。
    識別子（または予約語）を表す字句を読んだ後、字句が予約語か
    どうかを判断するために、関数 isReservedWord() を呼び出す。
    関数 isReservedWord() の戻り値に応じて、予約語のトークンを
    返すか、識別子のトークンを返す（と共にyylval.symbolに属性を
    代入する）かの、いずれかの処理を行う。
     */
    string str; // 蓄積する文字

    str += c;
    currentChar = getCharacter();

    // 英数字が続く限り字句を蓄積
    while (isalnum(currentChar)) {

        str += currentChar;
        currentChar = getCharacter();

    }

    // 字句が予約語の場合
    if (int reservedToken = isReservedWord(str)) {
        return reservedToken;
        // 字句が予約語でない場合
    } else {
        // newして新しいメモリ空間を確保
        yylval.symbol = new string(str);//グローバル変数 yylval に字句を保存
        //yylval.symbol の型は y.tab.h を参照のこと．
        return ID;
    }

//  cout << yylval.symbol->c_str() << " : "; //@@

}

/* 整数または実数を取得する                                          */
/*   c: 字句の１文字目（数字）                                       */
/*   返り値: トークンINUM か RNUM                                    */
/*   副作用: yylval.inum か yylval.rnum に字句が表す整数か実数を代入 */
static int getNumber(int c) {
    // todo: なぜ仮引数'c'をcurrentCharの代わりに使うのか不明
    // currentCharに直接getCharacterの戻り値を代入してあげないと、
    // getNumberを抜ける時にcurrentChar = cという処理が必ず必要
    // になるので、currentCharを使った方が面倒がない。

    /*
    ここで，整数または実数を表す字句を読み取る有限オートマトンを
    シミュレートする。
    整数または実数を表す字句を保存するための局所変数を用意すること。
    整数を表す字句を読んだときは、字句を整数に変換したものを yylval.inum に
    代入し、整数のトークンを返す。変換には atoi() を使うとよい。
    実数を表す字句を読んだときは、字句を実数に変換したものを yylval.rnum に
    代入し、実数のトークンを返す。変換には atof() を使うとよい。
    */
    string tmp = "";    //01 影山 10.12
    do {
        tmp.push_back(currentChar);
    } while (isdigit(currentChar = getCharacter()));

    // 実数
    if (currentChar == '.') {
        // '.'以降の数字を獲得
        do {
            tmp.push_back(currentChar);
        } while (isdigit(currentChar = getCharacter()));

        // e(E)を使った表記
        if (currentChar == 'e' || currentChar == 'E') {
            tmp.push_back(currentChar);
            currentChar = getCharacter();

            // [eE]の次は[数字]か[+-]

            // [eE]の次が[数字]の場合
            if (isdigit(currentChar)) {
                // [eE] 以降の数字を獲得
                do {
                    tmp.push_back(currentChar);
                } while (isdigit(currentChar = getCharacter()));

                // [eE]の次が[+-]の場合
            } else if (currentChar == '+' || currentChar == '-') {
                tmp.push_back(currentChar);
                if (isdigit(currentChar = getCharacter())) {
                    // [eE][+-]以降の数字を獲得
                    do {
                        tmp.push_back(currentChar);
                    } while (isdigit(currentChar = getCharacter()));
                } else {
                    compileError(EIllegalReal, currentChar, currentChar);
                }

                // [eE]の次が[数字]でも[+-]でもない場合はエラー
            } else {
                compileError(EIllegalReal, currentChar, currentChar);
            }
        }

        yylval.rnum = atof(tmp.c_str());
        return RNUM;
    } else {
        // 整数
        yylval.inum = atoi(tmp.c_str());
        return INUM;
    }
}

/* 文字読み取りモジュール                       */
/*  返り値: ファイルから読んだ文字              */
/*  副作用: 改行文字を読んだとき lineNo が1増加 */
static int getCharacter() {
    // fscanfがうまく使えなかったのでgetcに置き換え
    int tmp;    //影山 10.05_01
    tmp = getc(srcFilePointer);
    if (tmp == '\n')
        lineNo++;
    return tmp;
}
