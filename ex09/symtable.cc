#pragma implementation "symtable.h"

using namespace std;

/*******************************************************************
この次の行に、課題作成に参加したメンバ全員の学籍番号を記入すること

 s1180108 中野　慈
 s1220170 大滝　寛人
 s1230073 櫻井　俊輔
 s1230150 影山　尚登

*******************************************************************/

#include <string>
#include <map>
#include "symtable.h"
#include "error.h"

// static 関数のプロトタイプ宣言
static VarEntry *addVariable(string name, VarClass vc, Type type, 
                             bool array, int size, SymbolTable *table);
static SymbolEntry *find(string name, SymbolTable *table);


static SymbolTable globalSymTable;	// 大域的な記号表
static SymbolTable localSymTable;	// 局所的な記号表

static int localVarLocation;		// 局所変数のフレーム内相対番地
static int paramLocation;		// 仮引数のフレーム内相対番地


// プログラムのコンパイル終了後に、宣言済のユーザ手続きが
// すべて定義されたかどうかを調べる関数
// 宣言されたのに定義されていない関数があったらエラーを報告する
void checkProgram()
{
  SymbolTable::iterator it, ie = globalSymTable.end();
  // 大域的な記号表のエントリを順に調べる
  for (it=globalSymTable.begin(); it != ie; it++)  {
    SymbolEntry *symbol = it->second;
    // そのエントリがユーザ手続きエントリで、未定義ならエラーを報告する
    if (symbol->isProcedure() && !((ProcEntry *)symbol)->isDefined())
      compileError(EProcNotDefined,symbol->getName().c_str());
  }
}

// 記号表の初期化を行う関数。
// 大域的な記号表に、あらかじめmain()を登録しておく。
// こうすることで、main()はユーザが宣言する必要はなくなる。
// （定義はユーザがしなければならない）
void initializeSymbolTable()
{
  addProcedure("main", TVoid, NULL);	// main()を登録
}

// main()本体の中間コードを記号表から取り出す関数
// インタプリタが中間コードを実行するときは、main()本体の中間コード
// から実行を開始する。コンパイルが終了した後に、インタプリタに
// main()本体の中間コードを渡すために、この関数を呼び出す。
Code *getMainCode()
{
  // main()のエントリを大域的な記号表から取り出す（checkProgram()により、
  // main()が定義済であることをチェックしてからこの関数を呼び出す。
  // そうすれば、ここでmain()が定義済かチェックする必要はない）
  ProcEntry *proc = (ProcEntry *)globalSymTable.find(string("main"))->second;
  return proc->getCode();
}


// 大域変数のための変数エントリを生成し、大域的な記号表に登録する
// 登録した変数エントリへのポインタを返す
VarEntry *addGlobalVariable(string name, Type type)
{
  // この関数の本体を変更すること
  return addVariable(name, GlobalVar, type, false, 0, &globalSymTable);
}

// 配列のための変数エントリを生成し、大域的な記号表に登録する
// 登録した変数エントリへのポインタを返す
VarEntry *addArray(string name, Type type, int size)
{
  VarEntry *var = addVariable(name, GlobalVar, type, true, size, &globalSymTable);
  return var;
}

// 局所変数のための変数エントリを生成し、局所的な記号表に登録する
// 登録した変数エントリへのポインタを返す
VarEntry *addLocalVariable(string name, Type type)
{
  // この関数の本体を変更すること
  VarEntry *var = addVariable(name, LocalVar, type, false, 0, &localSymTable);
  var->setLocation(localVarLocation++);
  
  return var;
}

// 仮引数のための変数エントリを生成し、局所的な記号表に登録する
// 登録した変数エントリへのポインタを返す
VarEntry *addParameter(string name, Type type)
{
  // この関数の本体を変更すること
  VarEntry *var = addVariable(name,Param, type, false, 0, &localSymTable);
  var->setLocation(paramLocation++);
  
  return var;
}

// 識別子名nameと変数の種別vcをもつ変数エントリを生成し、
// tableが指す記号表にその変数エントリを登録する
// 登録した変数エントリへのポインタを返す
static VarEntry *addVariable(string name, VarClass vc, Type type,
                             bool array, int size, SymbolTable *table)
{
  // この関数の本体を変更すること

  // 同じ名前の識別子が与えられた記号表に存在するかチェック、あればエラー
  SymbolEntry *symbol = find(name,table);
  if (symbol != NULL)  {
    if (symbol->isVariable())
      compileError(EVarDuplicated,name.c_str());
    else
      compileError(EAlreadyAsProc,name.c_str());
  }
  // なければ識別子名name、種別vclassの変数を生成し、与えられた記号表に登録
  VarEntry *var = new VarEntry( vc, name, type, array, size );
  table->insert(make_pair(name, var));
  return var;
}

ProcEntry *addProcedure(string name, Type type, ParamList *params)
{

  // 同じ名前の識別子が与えられた記号表に存在するかチェック、あればエラー
  SymbolEntry *symbol = find(name,&globalSymTable);
  if (symbol != NULL)  {
    if (symbol->isProcedure())
      compileError(EProcDuplicated,name.c_str());
    else
      compileError(EAlreadyAsVar,name.c_str());
  }
  // なければ識別子名name、種別vclassの手続きを生成し、与えられた記号表に登録
  ProcEntry *proc = new ProcEntry( type, name, params );
  globalSymTable.insert(make_pair(name, proc));
  return proc;
}

ProcEntry *defineProcedure(string name, Type type)
{

  // 同じ名前の識別子が与えられた記号表に存在するかチェック
  ProcEntry *proc = findProcedure(name);

  if(proc->isDefined())
    compileError(EProcDefDuplicated,name.c_str());

  if(type != proc->getType())
    compileError(EProcTypeMismatch,name.c_str());

  localVarLocation = 1;
  paramLocation = -(proc->getParamNumber());
  
  localSymTable.clear();
  
  proc->define();
  
  return proc;
}

// 識別子名nameをもつ変数エントリを記号表から探し、
// 見つかったらその変数エントリへのポインタを返す
// 見つからなければエラー
VarEntry *findVariable(string name)
{
  SymbolEntry *var = find(name,&localSymTable);

  // 局所的な記号表に識別子nameをもつ記号表エントリがなければ、
  // 大域的な記号表から識別子nameをもつ記号表エントリを探す
  if (var == NULL)
    var = find(name,&globalSymTable);

  // 記号表エントリが見つからなければエラー
  if (var == NULL)
    compileError(EVarNotDeclared,name.c_str());
  // 手続きエントリが見つかったらエラー
  else if (var->isProcedure())
    compileError(EDeclaredAsProc,name.c_str());
  // 見つかった変数エントリへのポインタを返す
  else
    return (VarEntry *)var;
}

ProcEntry *findProcedure(string name)
{
/*  SymbolEntry *proc = find(name,&localSymTable);

  // 局所的な記号表に識別子nameをもつ記号表エントリがなければ、
  // 大域的な記号表から識別子nameをもつ記号表エントリを探す
  if (proc == NULL)
    proc = find(name,&globalSymTable);*/
  SymbolEntry *proc = find(name,&globalSymTable);

  // 記号表エントリが見つからなければエラー
  if (proc == NULL)
    compileError(EProcNotDeclared,name.c_str());
  // 変数エントリが見つかったらエラー
  else if (proc->isVariable())
    compileError(EDeclaredAsVar,name.c_str());
  // 見つかった手続きエントリへのポインタを返す
  else
    return (ProcEntry *)proc;
}

// 識別子名nameをもつ記号表エントリを、tableが指す記号表から探す
// 見つかったらその記号表エントリへのポインタを、見つからなければ
// NULLを返す
static SymbolEntry *find(string name, SymbolTable *table)
{
  SymbolTable::iterator it = table->find(name);
  if (it == table->end())
    return NULL;
  else
    return it->second;
}

// 手続き定義の仮引数リストと手続き宣言の仮引数リストを比較し、
// 引数の個数と、対応する引数の型が、それぞれ一致することをチェックする。
// 一致しなければエラー。一致すれば、何もせずにリターン。
void checkParamList(ParamList *params, ProcEntry *proc)
{
  int paramsSize = params != NULL ? params->size() : 0;
  
  if(proc->getParamNumber() != paramsSize)
    compileError(EParamNumMismatch,(proc->getName()).c_str(),paramsSize,proc->getParamNumber());

  //引数が0個の場合は型エラー無し
  if(proc->getParamNumber()){
    ParamList *procParams = proc->getParamList();
    ParamList::iterator it1 = procParams->begin();
    ParamList::iterator it2 = params->begin();
    for(int cnt = 0;it1 != procParams->end();it1++, it2++){
      cnt++;
      if(it1->first != it2->first)
	compileError(EParamTypeMismatch,(proc->getName()).c_str(),cnt);
    }
  }
  
  return;
}
