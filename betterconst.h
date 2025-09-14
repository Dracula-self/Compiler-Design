#pragma once
#include "total.h"
//常量定值表，用于常量表达式优化
struct consttable {
	ARG* var;//表示变量
	int constvalue;
	struct consttable* pre;
	struct consttable* next;
};
//存储基本块的全局数组
extern codeFile* BLOCK[200];
//全局常量定值表
extern consttable* constTable;
//基本块划分函数
int DivBaseBlock(codeFile* firstCode);
//常量表达式优化主函数
codeFile* constoptimize(codeFile* firstcode);
//对特定基本块进行优化
void OptiBlock(int i);
//处理算术和比较操作
int ArithC(codeFile* code);
//对参数进行值替换
void Substiarg(codeFile* code, int i);
//在常量定值表中查找变量
int findconst(ARG* arg, consttable** entry);
//添加变量及其常量值到常量定值表
void addconsttable(ARG* arg, int res);
//从常量定值表中删除变量
void delconsttable(ARG* arg);
//输出当前常量定值表
void printConsttable(int i);