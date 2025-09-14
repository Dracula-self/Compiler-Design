#pragma once
#include "total.h"
using namespace std;
//参数表
struct ParamTable {
	//int entry;//指向形参标识符在符号表中的位置
	SymbolTable* entry;
	struct ParamTable* next;
};

struct TypeIR {
	unsigned int serial;//在属性表中的位置序号
	int size;//类型所占空间大小
	TypeKind kind;
	union {
		struct {
			struct TypeIR* indexTy;
			struct TypeIR* elemTy;
			int low;//记录数组上界
			int up;//记录数组下界
		}ArrayAttr;
		FieldChain* body;
	}More;
};
struct AttributeIR {
	struct TypeIR* idtype;//指向标识符的类型内部表示
	IdKind kind;//标识符的类型
	union {
		struct {
			AccessKind access;
			int level;
			int off;//变量相对它所在主程序/过程的偏移量
		}VarAttr;//变量标识符的属性
		struct {
			int level;//当前过程的层数
			struct ParamTable* param;//参数表
			int code;//过程目标代码地址（目标代码生成时候填写）
			int size;//当前过程所需要空间大小
			int prAcReSi;//运行时过程活动记录大小
			int procEntry;//过程入口地址
			int procEntryNum;//过程的入口编号用于中间代码生成
			int spOff;//SP到display表的偏移量
		}ProcAttr;//过程名标识符的属性
	}More;//标识符的不同类型有不同属性
};
// 符号表项类
struct SymbolTable {
	char idname[IDLENGTH];//标识符的名称
	AttributeIR attrIR;
	struct SymbolTable* next;
};

//符号表函数声明
void CreateTable(void);//1.创建一个符号表
void DestroyTable(void);//2.撤销一个符号表
bool Enter(char* Id, AttributeIR* AttribP, SymbolTable** Entry, TreeNode* t);//3.登记标识符和属性到符号表
bool FindEntry(char* id, bool flag, SymbolTable** Entry);//4.符号表中查找标识符
bool FindField(char* Id, FieldChain* head, FieldChain** Entry);//5.在域表中查找域名
void PrintSymbTable(const string& path);//6.打印符号表
bool SearchOneTable(char* id, int currentlevel, SymbolTable** Entry);//7.在单个符号表中查找标识符

// 语义分析器函数声明
void Analyze(TreeNode* currentP);//1.语义分析主函数
void initialize(void);//2.初始化基本类型内部表示函数
TypeIR* TypeProcess(TreeNode* t, DecKind deckind);//3.类型分析函数
TypeIR* nameType(TreeNode* t);//4.自定义类型内部结构分析函数
TypeIR* arrayType(TreeNode* t);//5.数组类型内部表示处理函数
TypeIR* recordType(TreeNode* t);//6.处理记录类型的内部表示函数
void TypeDecPart(TreeNode* t);//7.类型声明部分分析处理函数
void VarDecList(TreeNode* t);//8.变量声明部分分析处理函数
void ProcDecPart(TreeNode* t);//9.过程声明部分分析处理函数
SymbolTable* HeadProcess(TreeNode* t);//10.过程声明头分析函数
ParamTable* ParaDecList(TreeNode* t);//11.形参分析处理函数
void Body(TreeNode* t);//12.执行体部分分析处理函数
void statement(TreeNode* t);//13.语句序列分析处理函数
TypeIR* Expr(TreeNode* t, AccessKind* Ekind);//14.表达式分析处理函数
TypeIR* arrayVar(TreeNode* t);//15.数组变量的分析处理函数
TypeIR* recordVar(TreeNode* t);//16.记录变量中域变量的分析处理函数
void assignstatement(TreeNode* t);//17.赋值语句分析函数
void callstatement(TreeNode* t);//18.过程调用语句分析处理函数
void ifstatement(TreeNode* t);//19.条件语句分析处理函数
void whilestatement(TreeNode* t);//20.循环语句分析处理函数
void readstatement(TreeNode* t);//21.读语句分析处理函数
void returnstatement(TreeNode* t);//22.返回语句分析处理函数
void writestatement(TreeNode* t);//23.写语句分析处理函数
void LoopProgram(TreeNode* t);//遍历语法树，调用相应的函数处理语法树结点