#pragma once
#include "total.h"

//新建一个ARG结构
ARG* NewTemp(AccessKind now);
//新建数值类ARG
ARG* ARGvalue(int now);
//产生一个新的标号
int NewLabel();
//新建标号类ARG
ARG* ARGLabel(int now);
//新建地址类ARG
ARG* ARGAddr(const char* id, int level, int off, AccessKind acc);
//中间代码的构建
codeFile* GenCode(Codekind coki, ARG* arg1, ARG* arg2, ARG* arg3);
//把codekind转换成string
string codekindToString(Codekind kind);
//打印ARG
void printarg(ARG* now);
//输出所有中间代码
void printmidcode(codeFile* head,FILE*outfile);
//中间代码生成主函数
codeFile* GenMidCode(TreeNode* t);
//生成过程入口、出口、过程体中间代码
void GenProcDec(TreeNode* now);
//语句体中间代码生成函数,循环调用语句中间代码处理函数处理每条语句
void GenBody(TreeNode* now);
//根据语句具体类型调用相应的语句处理函数
void GenStatement(TreeNode* now);
//赋值语句中间代码生成
void GenAssignS(TreeNode* a);
//变量midcode生成
ARG* GenVar(TreeNode* a);
//数组变量
ARG* GenArray(ARG* v1arg, TreeNode* t, int low, int size);
//域成员变量
ARG* GenField(ARG* v1arg, TreeNode* t, FieldChain* head);
//从lextype到操作符的对应
Codekind midclass(LexType p);
//表达式(常数、变量、复杂表达式)
ARG* GenExpr(TreeNode* t);
//过程调用语句的中间代码(CALL，f，─，Result )
void GenCallS(TreeNode* t);
//读语句中间代码生成read(v1)---(READ,v1)
void GenReadS(TreeNode* now);
void GenIfS(TreeNode* t);
//写语句中间代码
void GenWriteS(TreeNode* t);
void GenWhileS(TreeNode* t);

extern FILE* mids;//中间代码输出用
