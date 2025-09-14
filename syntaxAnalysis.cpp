#pragma once
#include "LexicalAnalysis.h"
#include "Error_handler.h"
#include "syntaxAnalysis.h"
#include "total.h"
#include <iostream>
using namespace std;
//当前token所在行号SynLine
int SynLine = 0;
TokenNode* NowToken = NULL;//会在第一次调用时候被赋予head
int SynErrorNum = 0;//语法分析是否有错误
//调用program()处理函数创建语法分析树


int MatchToken(LexType p) {
	//1匹配成功
	if (p == NowToken->Lex) {
		return 1;
	}
	else {
		return 0;
	}
}

void InitTreeNode(struct TreeNode* t) {
	if (t == NULL) return;
	for (int i = 0; i < MAXCHILDREN; i++) {
		t->child[i] = NULL;
	}
	t->childnum = 0;
	t->sibling = NULL;
	t->lineno = SynLine;
	t->nodekind = DecK;  // 默认是声明节点
	t->kind.dec = decNull;  // 未定义类型
	t->name.clear();
	t->idnum = 0;
	// 初始化符号表指针
	for (int i = 0; i < 10; i++) {
		t->table[i] = NULL;
	}
	t->type_name = "";
	t->attr.ArrayAttr.low = 0;
	t->attr.ArrayAttr.up = 0;
	t->attr.ExpAttr.ischar = false;//假设是数字
	t->attr.ArrayAttr.childtype = decNull;
	t->attr.ProcAttr.paramt = paramNull;
}


struct TreeNode* parse(TokenNode* head) {
	NowToken = head;
	SynLine = NowToken->Lineshow;
	TreeNode* t = program();
	//如果在解析过程中发现语法错误，立即返回NULL
	if (SynErrorNum > 0) {
		printf("语法分析失败：发现 %d 个语法错误，程序终止\n", SynErrorNum);
		fprintf(outfile, "语法分析失败：发现 %d 个语法错误，程序终止\n", SynErrorNum);
		return NULL;
	}
	if (NowToken->Lex != ENDFILE) {
		printf("语法错误：第 %d行，语法结束错误\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第 %d行，语法结束错误\n", NowToken->Lineshow);
		return NULL; //直接返回NULL，表示分析失败
	}
	return t;
}

TreeNode* ProgramHead() {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = PheadK;
	//当前单词的PROGRAM匹配
	if (NowToken->Lex != PROGRAM) {
		printf("语法错误：第%d行，缺少程序头program-programhead", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少程序头program-programhead", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;
	SynLine = NowToken->Lineshow;
	if (NowToken->Lex == ID) {
		t->name.push_back(NowToken->Sem);
		t->idnum++;
	}
	else {
		printf("语法错误：第%d行，缺少程序名-programhead", NowToken->Lineshow); 
		fprintf(outfile,"语法错误：第%d行，缺少程序名-programhead", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
	return t;
}


struct TreeNode* program(void) {
	TreeNode* t = ProgramHead();
	TreeNode* q = DeclarePart();
	TreeNode* s = ProgramBody();
	TreeNode* root = new TreeNode();
	InitTreeNode(root);
	root->nodekind = ProK;
	root->child[0] = t;
	root->child[1] = q;
	root->child[2] = s;
	root->childnum = 3;
	//当前单词和DOT匹配
	if (NowToken->Lex != DOT) {
		printf("语法错误：第%d行，\'.\'缺失-program\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，\'.\'缺失-program\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	//指针后移
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	root->lineno = 0;
	return root;
}

TreeNode* DeclarePart() {
	TreeNode* type_t = new TreeNode();
	InitTreeNode(type_t);
	type_t->nodekind = TypeK;
	type_t->child[0] = TypeDec();type_t->childnum++;
	TreeNode* var_t = new TreeNode();InitTreeNode(var_t);
	var_t->nodekind = VarK;
	var_t->child[0] = VarDec();
	TreeNode* proc_deck_t = new TreeNode();
	InitTreeNode(proc_deck_t);
	proc_deck_t->nodekind = ProcDecK;
	proc_deck_t->child[0] = ProcDec();
	type_t->sibling = var_t;
	var_t->sibling = proc_deck_t;
	return type_t;
}

TreeNode* TypeDeclaration(void) {
	TreeNode* t = NULL;
	if (NowToken->Lex != TYPE) {
		printf("语法错误：第%d行，缺失Type-TypeDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺失Type-TypeDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t = TypeDecList();
	if (t == NULL) {
		printf("语法错误：第%d行，缺失TypeDecList-TypeDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺失TypeDecList-TypeDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	return t;
}

TreeNode* TypeDec(void) {
	TreeNode* t = NULL;
	if (NowToken->Lex == TYPE) {
		t = TypeDeclaration();
		return t;
	}
	else if (NowToken->Lex == VAR || NowToken->Lex == PROCEDURE || NowToken->Lex == BEGIN) {
		return NULL;
	}
	else {
		printf("语法错误：第%d行，类型声明阶段缺少TYPE-TypeDec\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，类型声明阶段缺少TYPE-TypeDec\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
	return t;
}

TreeNode* TypeDecList(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = DecK;
	TypeId(t);
	if (NowToken->Lex != EQ) {
		printf("语法错误：第%d行，缺少'='-TypeDecList\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少'='-TypeDecList\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	TypeDef(t);
	if (NowToken->Lex != SEMI) {
		printf("语法错误：第%d行，缺少';'-TypeDecList\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少';'-TypeDecList\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	TreeNode* p = TypeDecMore();
	t->sibling = p;
	return t;
}

TreeNode* TypeDecMore(void) {
	TreeNode* t = NULL;
	if (NowToken->Lex == VAR || NowToken->Lex == PROGRAM || NowToken->Lex == BEGIN) {
		return NULL;//*这里到底是procdeure还是program？？？？？？
	}
	else if (NowToken->Lex == ID) {
		t = TypeDecList();
		return t;
	}
	else {
		printf("语法错误：第%d行，缺少VAR/ID/PROCEDURE/BEGIN等关键字-TypeDecMore\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少VAR/ID/PROCEDURE/BEGIN等关键字-TypeDecMore\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

void TypeId(TreeNode* t) {
	if (NowToken->Lex == ID && t != NULL) {
		t->name.push_back(NowToken->Sem);
		t->idnum++;
	}
	else {
		if (t == NULL) {
			printf("语法错误：第%d行，空树节点-TypeId\n", NowToken->Lineshow);
			fprintf(outfile, "语法错误：第%d行，空树节点-TypeId\n", NowToken->Lineshow);
			SynErrorNum++;exit(1);
		}
		else {
			printf("语法错误：第%d行，需要标识符-TypeId\n", NowToken->Lineshow);
			fprintf(outfile, "语法错误：第%d行，需要标识符-TypeId\n", NowToken->Lineshow);
			SynErrorNum++;exit(1);
		}
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
}

void TypeDef(TreeNode* t) {
	if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR) {
		BaseType(t);
	}
	else if (NowToken->Lex == ARRAY || NowToken->Lex == RECORD) {
		StructureType(t);
	}
	else if (NowToken->Lex == ID) {
		t->kind.dec = IdK;
		t->type_name = NowToken->Sem;
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	}
	else {
		printf("语法错误：类型定义出现错误单词-TypeDef\n");
		fprintf(outfile, "语法错误：类型定义出现错误单词-TypeDef\n");
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	}
}

//MR-----------------------------------------------------------------------
// 11. BaseType函数：处理基本类型（INTEGER 或 CHAR）
void BaseType(TreeNode* t) {
	if (NowToken->Lex == INTEGER) {
		t->kind.dec = DecKind::IntegerK;
	}
	else if (NowToken->Lex == CHAR) {
		t->kind.dec = DecKind::CharK;
	}
	else {
		printf("语法错误：第%d行，当前token不是基本类型（INTEGER 或 CHAR）, 读入下一个token... - void BaseType(TreeNode * t)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前token不是基本类型（INTEGER 或 CHAR）, 读入下一个token... - void BaseType(TreeNode * t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	////////////////////////////////////////////////////////////////////////每一种判断结束都要读入下一个token
	NowToken = NowToken->next;     SynLine = NowToken->Lineshow;
}

// 12. StructureType函数：处理结构类型（ARRAY 或 RECORD）
void StructureType(TreeNode* t) {
	if (NowToken->Lex == ARRAY) { // 如果当前token是ARRAY
		t->kind.dec = DecKind::ArrayK; // 设置节点类型为ArrayK
		ArrayType(t); // 调用ArrayType函数处理数组类型
	}
	else if (NowToken->Lex == RECORD) { // 如果当前token是RECORD
		t->kind.dec = DecKind::RecordK; // 设置节点类型为RecordK
		RecType(t); // 调用RecType函数处理记录类型
	}
	else { // 如果当前token既不是ARRAY也不是RECORD
		printf("语法错误：第%d行，当前token不是结构类型（ARRAY 或 RECORD）,读入下一个token... -void StructureType(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前token不是结构类型（ARRAY 或 RECORD）,读入下一个token... -void StructureType(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
	}
}

// 13. ArrayType函数：处理数组类型声明
void ArrayType(TreeNode* t) {
	if (NowToken->Lex != ARRAY) {
		printf("语法错误：第%d行，应为 ARRAY, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为 ARRAY, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
	if (NowToken->Lex != LMIDPAREN) { // 如果当前token不是左中括号[
		printf("语法错误：第%d行，应为 [, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为 [, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
	if (NowToken->Lex != INTC) {
		printf("语法错误：第%d行，应为数组整型常量下界, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为数组整型常量下界, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	t->attr.ArrayAttr.low = atoi(NowToken->Sem); // 将下界字符串转换为整数并存储
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
	if (NowToken->Lex != UNDERANGE) { // 如果当前token不是..
		printf("语法错误：第%d行，应为 .., 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为 .., 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;

	if (NowToken->Lex != INTC) { // 如果当前token不是整型常量
		printf("语法错误：第%d行，应为数组整型常量上界, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为数组整型常量上界, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	t->attr.ArrayAttr.up = atoi(NowToken->Sem); // 将上界字符串转换为整数并存储
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
	if (NowToken->Lex != RMIDPAREN) { // 如果当前token不是右中括号]
		printf("语法错误：第%d行，应为 ], 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为 ], 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;

	if (NowToken->Lex != OF) { // 如果当前token不是OF
		printf("语法错误：第%d行，应为 OF, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为 OF, 但找到了 %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next; // 读取下一个token
	SynLine = NowToken->Lineshow;
	BaseType(t);
	t->attr.ArrayAttr.childtype = t->kind.dec;
	t->kind.dec = DecKind::ArrayK;
}

// 14. RecType函数：处理记录类型声明
void RecType(TreeNode* t) {
	if (NowToken->Lex != RECORD) {
		printf("语法错误：第%d行，应为 RECORD, 但找到了 %s -void RecType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为 RECORD, 但找到了 %s -void RecType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
	TreeNode* p = FieldDecList(); // 调用FieldDecList函数处理记录中的域声明列表
	if (p != NULL) { // 如果FieldDecList返回的指针有效
		t->child[0] = p;
		t->childnum = 1;
	}
	else {
		printf("语法错误：第%d行，未能解析字段声明列表 -void RecType(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，未能解析字段声明列表 -void RecType(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	if (NowToken->Lex != END) { // 如果当前token不是END
		printf("语法错误：第%d行，应为 END, 但找到了 %s -void RecType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为 END, 但找到了 %s -void RecType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
}

// 15. FieldDecList函数：处理域声明列表
TreeNode* FieldDecList(void) {
	TreeNode* t = new TreeNode(); // 创建新的语法树节点
	InitTreeNode(t); // 初始化节点
	t->nodekind = NodeKind::DecK; // 设置节点类型为声明节点

	if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR) { // 如果当前token是INTEGER或CHAR
		BaseType(t); // 调用BaseType函数处理基本类型
		IdList(t); // 调用IdList函数处理标识符列表
		if (NowToken->Lex == SEMI) { // 如果当前token是分号;
			NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
			t->sibling = FieldDecMore(); // 调用FieldDecMore函数处理更多的域声明
			return t;
		}
		else { // 如果当前token不是分号;
			printf("语法错误：第%d行，域声明列表中缺少分号 -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
			fprintf(outfile, "语法错误：第%d行，域声明列表中缺少分号 -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
			SynErrorNum++;exit(1);
			return t;
		}
	}
	else if (NowToken->Lex == ARRAY) { // 如果当前token是ARRAY
		ArrayType(t); // 调用ArrayType函数处理数组类型
		IdList(t); // 调用IdList函数处理标识符列表
		if (NowToken->Lex == SEMI) {
			NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
			t->sibling = FieldDecMore();
			return t;
		}
		else { // 如果当前token不是分号;
			printf("语法错误：第%d行，域声明列表中缺少分号 -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
			fprintf(outfile, "语法错误：第%d行，域声明列表中缺少分号 -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
			SynErrorNum++;exit(1);
			return t;
		}
	}
	else { // 如果当前token既不是INTEGER、CHAR也不是ARRAY
		printf("语法错误：第%d行，域声明列表中出现意外的token, 不是 INTEGER、CHAR 或 ARRAY -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，域声明列表中出现意外的token, 不是 INTEGER、CHAR 或 ARRAY -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
		return NULL;
	}
}

// 16. FieldDecMore函数：处理更多的域声明
TreeNode* FieldDecMore(void) {
	TreeNode* t = NULL; // 初始化t为NULL

	if (NowToken->Lex == END) { // 如果当前token是END
		return NULL; // 直接返回NULL
	}
	else if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR || NowToken->Lex == ARRAY) { // 如果当前token是INTEGER、CHAR或ARRAY
		t = FieldDecList(); // 调用FieldDecList函数处理域声明
		return t; // 返回结果
	}
	else { // 如果当前token既不是END也不是INTEGER、CHAR或ARRAY
		printf("语法错误：第%d行，域声明列表中出现意外的token, 不是 INTEGER、CHAR 或 ARRAY -TreeNode* FieldDecMore(void)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，域声明列表中出现意外的token, 不是 INTEGER、CHAR 或 ARRAY -TreeNode* FieldDecMore(void)\n", NowToken->Lineshow);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
		return NULL; // 返回NULL
	}
}

// 17. IdList函数：处理标识符列表
void IdList(TreeNode* t) {
	if (NowToken->Lex == ID) { // 如果当前token是ID
		t->name.push_back(NowToken->Sem); // 将标识符名称存储到节点中
		t->idnum++; // 标识符数量加1
	}
	else {
		printf("语法错误：第%d行，应为ID -void IdList(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，应为ID -void IdList(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}

	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
	IdMore(t); // 调用IdMore函数处理更多的标识符
}

// 18. IdMore函数：处理更多的标识符
void IdMore(TreeNode* t) {
	if (NowToken->Lex == COMMA) {
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		IdList(t);
	}
	else if (NowToken->Lex == SEMI) { // 如果当前token是分号;
		return; // 直接返回
	}
	else { // 如果当前token既不是分号;也不是逗号,
		printf("语法错误：第%d行，当前单词既不是','，也不是';'，读取下一个单词... -void IdMore(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词既不是','，也不是';'，读取下一个单词... -void IdMore(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
	}
}

// 19. VarDec函数：处理变量声明部分
TreeNode* VarDec(void) {
	TreeNode* t = NULL; // 初始化 t 为 NULL
	if (NowToken->Lex == PROCEDURE || NowToken->Lex == BEGIN) {
		// 如果当前token是PROCEDURE或BEGIN，说明没有变量声明部分，直接返回NULL
		return NULL;
	}
	else if (NowToken->Lex == VAR) {
		// 如果当前token是VAR，说明有变量声明部分，调用VarDeclaration函数处理
		t = VarDeclaration();
		return t;
	}
	else {
		// 如果当前token既不是PROCEDURE、BEGIN，也不是VAR，说明语法错误
		printf("语法错误：第%d行，当前单词不是VAR，读取下一个单词... -TreeNode* VarDec(void)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词不是VAR，读取下一个单词... -TreeNode* VarDec(void)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next; // 读取下一个toke
		SynLine = NowToken->Lineshow;
		return NULL;
	}
}

// 20. VarDeclaration函数：处理变量声明的具体逻辑
TreeNode* VarDeclaration(void) {
	TreeNode* t = NULL;
	if (NowToken->Lex != VAR) {
		// 如果当前token不是VAR，说明语法错误
		printf("语法错误：第%d行，当前单词不是VAR -TreeNode* VarDeclaration(void)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词不是VAR -TreeNode* VarDeclaration(void)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;     SynLine = NowToken->Lineshow;
	// 调用 VarDecList 函数处理变量声明列表
	t = VarDecList();
	// 检查 VarDecList 返回的指针是否有效
	if (t == NULL) {
		printf("语法错误：第%d行，变量声明列表解析失败 -TreeNode* VarDeclaration(void)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，变量声明列表解析失败 -TreeNode* VarDeclaration(void)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	return t;
}

// 21. VarDecList函数：处理变量声明列表
TreeNode* VarDecList(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = NodeKind::DecK; // 设置节点类型为声明节点
	if (NowToken->Lex == CHAR || NowToken->Lex == INTEGER || NowToken->Lex == ARRAY || NowToken->Lex == RECORD || NowToken->Lex == ID) {
		TypeDef(t);
		VarIdList(t);
		if (NowToken->Lex != SEMI) {
			printf("语法错误：第%d行，缺少\";\" -TreeNode* VarDecList(void)\n", NowToken->Lineshow);
			fprintf(outfile, "语法错误：第%d行，缺少\";\" -TreeNode* VarDecList(void)\n", NowToken->Lineshow);
			SynErrorNum++;exit(1);
		}
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		t->sibling = VarDecMore();
		return t;
	}
	else {
		printf("语法错误：第%d行，缺少typename -TreeNode* VarDecList\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少typename -TreeNode* VarDecList\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

// 22. VarDecMore函数：处理更多的变量声明
TreeNode* VarDecMore(void) {
	TreeNode* t = NULL; // 初始化 t 为 NULL
	if (NowToken->Lex == PROCEDURE || NowToken->Lex == BEGIN) {
		// 如果当前token是PROCEDURE或BEGIN，说明变量声明部分结束，直接返回NULL
		return NULL;
	}
	else if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR || NowToken->Lex == RECORD || NowToken->Lex == ARRAY || NowToken->Lex == ID) {
		// 如果当前token是INTEGER、CHAR、RECORD、ARRAY或ID，说明还有更多的变量声明
		t = VarDecList(); // 调用 VarDecList 函数处理变量声明
		return t; // 返回处理结果
	}
	else {
		// 如果当前token不是上述类型，说明语法错误
		printf("语法错误：第%d行，当前单词不是变量（PROCEDURE、BEGIN、INTEGER、CHAR、RECORD、ARRAY 或 ID），读取下一个单词.. -TreeNode* VarDecMore(void)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词不是变量（PROCEDURE、BEGIN、INTEGER、CHAR、RECORD、ARRAY 或 ID），读取下一个单词.. -TreeNode* VarDecMore(void)\n", NowToken->Lineshow);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
		return NULL; // 返回NULL
	}
}

// 23. VarIdList函数：处理变量标识符列表
void VarIdList(TreeNode* t) {
	// 检查当前token是否为ID
	if (NowToken->Lex == ID) {
		// 如果当前token是ID，将标识符名称存储到节点的name数组中
		t->name.push_back(NowToken->Sem);
		t->idnum++; // 标识符数量加1
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
	}
	else {
		// 如果当前token不是ID，说明语法错误
		printf("语法错误：第%d行，应为 ID, 但发现了 %s -VarIdList\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为 ID, 但发现了 %s -VarIdList\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
	}
	// 调用 VarIdMore 函数处理更多的标识符
	VarIdMore(t);
}

// 24. VarIdMore函数：处理更多的变量标识符
void VarIdMore(TreeNode* t) {
	// 检查当前token的类型
	if (NowToken->Lex == SEMI) {
		// 如果当前token是分号;，说明变量标识符列表结束，直接返回
		return;
	}
	else if (NowToken->Lex == COMMA) {
		// 如果当前token是逗号,，说明还有更多的标识符
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
		VarIdList(t); // 调用 VarIdList 函数处理更多的标识符
	}
	else {
		// 如果当前token既不是分号;也不是逗号,，说明语法错误
		printf("语法错误：第%d行，当前单词既不是','，也不是';'，读取下一个单词.. -void VarIdMore(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词既不是','，也不是';'，读取下一个单词.. -void VarIdMore(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
	}
}

// 25. ProcDec函数：处理过程声明部分
TreeNode* ProcDec(void) {
	TreeNode* t = NULL; // 初始化 t 为 NULL
	if (NowToken->Lex == BEGIN) {
		return NULL;
	}
	else if (NowToken->Lex == PROCEDURE) {
		t = ProcDeclaration();
		return t;
	}
	else {
		printf("语法错误：第%d行，当前单词不是过程声明（BEGIN 或 PROCEDURE），读取下一个单词.. -TreeNode* ProcDec(void)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词不是过程声明（BEGIN 或 PROCEDURE），读取下一个单词.. -TreeNode* ProcDec(void)\n", NowToken->Lineshow);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next;     SynLine = NowToken->Lineshow;
		return NULL;exit(1);
	}
}

// 26. ProcDeclaration函数：处理过程声明的具体逻辑
TreeNode* ProcDeclaration() {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = ProcDecK;
	if (NowToken->Lex != PROCEDURE) {
		printf("语法错误：第%d行，缺少procedure-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少procedure-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex == ID) {
		t->name.push_back(NowToken->Sem); // 将参数名存储到节点的name数组中
		t->idnum++; 
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	}
	else {
		printf("语法错误：第%d行，缺少program_ID-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少program_ID-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	if (NowToken->Lex != LPAREN) {
		printf("语法错误：第%d行，缺少'('-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少'('-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	ParamList(t);
	if (NowToken->Lex != RPAREN) {
		printf("语法错误：第%d行，缺少')'-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少')'-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex != SEMI) {
		printf("语法错误：第%d行，缺少';'-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，缺少';'-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[1] = ProcDecPart();
	t->child[2] = procBody();
	t->childnum += 2;
	t->sibling = ProcDec();
	return t;
}

// 27. ParamList函数：处理参数列表
void ParamList(TreeNode* t) {
	TreeNode* p = NULL; // 初始化 p 为 NULL
	if (NowToken->Lex == RPAREN) {
		// 如果当前token是右括号)，说明参数列表为空，直接返回
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return;
	}
	else if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR || NowToken->Lex == RECORD || NowToken->Lex == ARRAY || NowToken->Lex == ID || NowToken->Lex == VAR) {
		// 如果当前token是INTEGER、CHAR、RECORD、ARRAY、ID或VAR，说明有参数声明
		p = ParamDecList(); // 调用 ParamDecList 函数处理参数声明序列
		if (p != NULL) {
			t->child[0] = p; // 将结果赋值给 t->child[0]
			t->childnum = 1; // 更新子节点数量
		}
	}
	else {
		// 如果当前token不是上述类型，说明语法错误
		printf("语法错误：第%d行，当前单词既不是参数类型（INTEGER、CHAR、RECORD、ARRAY、ID或VAR），也不是 ')'，读取下一个单词.. -void ParamList(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词既不是参数类型（INTEGER、CHAR、RECORD、ARRAY、ID或VAR），也不是 ')'，读取下一个单词.. -void ParamList(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++; 
		exit(1);
		NowToken = NowToken->next; 
		SynLine = NowToken->Lineshow;
	}
}

// 28. ParamDecList函数：处理参数声明序列
TreeNode* ParamDecList(void) {
	TreeNode* t = Param(); // 调用 Param 函数处理单个参数声明
	TreeNode* p = ParamMore(); // 调用 ParamMore 函数处理更多的参数声明
	if (p != NULL) {
		t->sibling = p; // 如果 ParamMore 返回的指针有效，将其赋值给 t 的 sibling 成员
	}
	return t;
}

// 29. ParamMore函数：处理更多的参数声明
TreeNode* ParamMore(void) {
	TreeNode* t = NULL; // 初始化 t 为 NULL
	if (NowToken->Lex == RPAREN) {
		// 如果当前token是右括号)，说明参数声明结束，直接返回NULL
		return NULL;
	}
	else if (NowToken->Lex == SEMI) {
		// 如果当前token是分号;，说明还有更多的参数声明
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
		t = ParamDecList(); // 调用 ParamDecList 函数处理参数声明序列
		if (t == NULL) {
			// 如果 ParamDecList 返回 NULL，说明参数声明解析失败
			printf("语法错误：第%d行，缺少参数列表或\";\" -TreeNode* ParamMore(void)\n", NowToken->Lineshow);
			fprintf(outfile, "语法错误：第%d行，缺少参数列表或\";\" -TreeNode* ParamMore(void)\n", NowToken->Lineshow);
			SynErrorNum++; 
			exit(1);
		}
		return t;
	}
	else {
		// 如果当前token既不是分号;也不是右括号)，说明语法错误
		printf("语法错误：第%d行，当前单词既不是 ';'，也不是 ')'，读取下一个单词.. -TreeNode* ParamMore(void)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词既不是 ';'，也不是 ')'，读取下一个单词.. -TreeNode* ParamMore(void)\n", NowToken->Lineshow);
		SynErrorNum++; 
		exit(1);
		NowToken = NowToken->next; 
		SynLine = NowToken->Lineshow;
		return NULL; 
	}
}

// 30. Param函数：处理单个参数声明
TreeNode* Param(void) {
	TreeNode* t = new TreeNode(); // 创建新的语法树节点
	InitTreeNode(t); // 初始化节点
	t->nodekind = NodeKind::DecK; // 设置节点类型为声明节点

	if (NowToken->Lex == VAR) {
		// 如果当前token是VAR，说明是变参
		t->attr.ProcAttr.paramt = ParamType::varparamType; // 设置参数类型为变参
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
		// 调用 TypeDef 和 FormList 函数处理类型定义和参数列表
		TypeDef(t);
		FormList(t);
	}
	else if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR || NowToken->Lex == RECORD || NowToken->Lex == ARRAY || NowToken->Lex == ID) {
		// 如果当前token是INTEGER、CHAR、RECORD、ARRAY或ID，说明是值参
		t->attr.ProcAttr.paramt = ParamType::valparamType; // 设置参数类型为值参
		// 调用 TypeDef 和 FormList 函数处理类型定义和参数列表
		TypeDef(t);
		FormList(t);
	}
	else {
		// 如果当前token不是上述类型，说明语法错误
		printf("语法错误：第%d行，当前单词既不是变参（VAR）,也不是值参（INTEGER, CHAR, RECORD, ARRAY 或 ID） -TreeNode* Param(void)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词既不是变参（VAR）,也不是值参（INTEGER, CHAR, RECORD, ARRAY 或 ID） -TreeNode* Param(void)\n", NowToken->Lineshow);
		SynErrorNum++; // 增加语法错误计数
		exit(1);
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
	}
	return t;
}

// 31. FormList函数：处理参数名列表
void FormList(TreeNode* t) {
	if (NowToken->Lex == ID) {
		// 如果当前token是ID，说明是参数名
		t->name.push_back(NowToken->Sem); // 将参数名存储到节点的name数组中
		t->idnum++;
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
	}
	else {
		// 如果当前token不是ID，说明语法错误
		printf("语法错误：第%d行，应为 ID, 但找到了 %s -void FormList(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，应为 ID, 但找到了 %s -void FormList(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++; 
		exit(1);
	}
	// 调用 FidMore 函数处理更多的参数名
	FidMore(t);
}

// 32. FidMore函数：处理更多的参数名
void FidMore(TreeNode* t) {
	if (NowToken->Lex == SEMI || NowToken->Lex == RPAREN) {
		// 如果当前token是分号;或右括号)，说明参数名列表结束，直接返回
		return;
	}
	else if (NowToken->Lex == COMMA) {
		// 如果当前token是逗号,，说明还有更多的参数名
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
		FormList(t); // 调用 FormList 函数处理更多的参数名
	}
	else {
		// 如果当前token既不是分号;、右括号)也不是逗号,，说明语法错误
		printf("语法错误：第%d行，当前单词不是 ','， ')' 或 ';' -void FidMore(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，当前单词不是 ','， ')' 或 ';' -void FidMore(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++; // 增加语法错误计数
		exit(1);
		NowToken = NowToken->next; // 读取下一个token
		SynLine = NowToken->Lineshow;
	}
}

//XYN------------------------------------------------------------------

//XYN------------------------------------------------------------------

//33过程中声明部分的分析处理程序
TreeNode* ProcDecPart(void) {
	TreeNode* t = DeclarePart();//调用声明部分函数
	if (t == NULL) {
		printf("语法错误：第%d行， 过程中声明部分的分析失败-ProcDecPart\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 过程中声明部分的分析失败-ProcDecPart\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	return t;
}

//34过程体部分处理分析程序
TreeNode* procBody(void) {
	TreeNode* t = ProgramBody();// 调用程序体部分函数
	if (t == NULL) {
		printf("语法错误：第%d行， 缺少程序体-procBody\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少程序体-procBody\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	return t;
}

//35主程序部分处理分析程序   
TreeNode* ProgramBody(void) {  //BEGIN  END
	TreeNode* t = new TreeNode(); // 创建新的语法树声明节点 
	InitTreeNode(t);//初始化节点
	t->nodekind = StmLK;//设置节点类型为语句标志类型
	if (NowToken->Lex == BEGIN) {
		NowToken = NowToken->next;// 移动到下一个单词
		SynLine = NowToken->Lineshow;
		t->child[0] = StmList();  // 调用 StmList() 处理语句序列
	}
	else {
		//error
		printf("语法错误：第%d行， 缺少BEGIN-ProgramBody\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少BEGIN-ProgramBody\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	if (NowToken->Lex != END) {
		printf("语法错误：第%d行， 缺少END-ProgramBody\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少END-ProgramBody\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//36语句序列部分处理分析程序   
TreeNode* StmList(void) {
	// 如果当前单词是 END，表示语句序列结束，返回 NULL
	if (NowToken->Lex == END) return NULL;
	TreeNode* t = Stm();//语句递归处理分析程序
	TreeNode* p = StmMore();//更多语句部分处理分析程序
	t->sibling = p;//兄弟节点
	return t;
}

//37更多语句部分处理分析程序
TreeNode* StmMore(void) {
	if (NowToken->Lex == END || NowToken->Lex == ENDWH || NowToken->Lex == FI || NowToken->Lex == ELSE) {
		return NULL;
	}
	else if (NowToken->Lex == SEMI) {
		if (NowToken->Lineshow == 34) {
			int ii = 0;
		}
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return StmList();
	}
	else {
		printf("语法错误：第%d行，语句结束符错误 - 期望分号(;)、END、ENDWH、FI或ELSE，但得到'%s'-StmMore\n",
			NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "语法错误：第%d行，语句结束符错误 - 期望分号(;)、END、ENDWH、FI或ELSE，但得到'%s'-StmMore\n",
			NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;
		exit(1);
		return NULL;
	}
}

//38语句递归处理分析程序
TreeNode* Stm(void) {
	if (NowToken->Lex == IF) {
		TreeNode* t = ConditionalStm();
		return t;
	}
	else if (NowToken->Lex == WHILE) {
		TreeNode* t = LoopStm();
		return t;
	}
	else if (NowToken->Lex == RETURN) {
		TreeNode* t = ReturnStm();
		return t;
	}
	else if (NowToken->Lex == READ) {
		TreeNode* t = InputStm();
		return t;
	}
	else if (NowToken->Lex == WRITE) {
		TreeNode* t = OutputStm();
		return t;
	}
	else if (NowToken->Lex == ID) {
		temp_name = NowToken->Sem;// 保存标识符名称
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		TreeNode* t = AssCall();
		return t;
	}
	else {
		printf("语法错误：第%d行， 语句开始不正确- Stm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 语句开始不正确- Stm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

//39赋值语句和函数调用语句部分的处理分析程序
TreeNode* AssCall(void) {
	if (NowToken->Lex == LPAREN) {
		return CallStmRest();
	}
	else if (NowToken->Lex == LMIDPAREN || NowToken->Lex == ASSIGN || NowToken->Lex == DOT) {
		return AssignmentRest();
	}
	else {
		printf("语法错误：第%d行， 缺少':='  或 '('-AssCall\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少':='  或 '('-AssCall\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

//40赋值语句处理分析函数  ??????
//如果目标语言的语法规则中，小于符号和等于符号是语法结构的关键组成部分，那么在构建语法树时就需要将它们包含进去。
//例如，在条件语句（如 if 语句）中，通常会使用比较运算符（如 < 和 =）来构成条件表达式。
//以伪代码 if x < 10 then ... 为例，其对应的语法树中，< 符号会作为一个重要的节点存在，用于表示条件判断的逻辑。
//还没有加
TreeNode* AssignmentRest(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;//设置节点类型为语句类型
	t->kind.stmt = AssignK;//设置语句类型为赋值语句
	TreeNode* child = new TreeNode();
	InitTreeNode(child);
	child->nodekind = ExpK;
	child->kind.exp = VariK;
	child->name.push_back(temp_name);
	child->idnum++;
	variMore(child);
	t->child[0] = child;
	if (NowToken->Lex != ASSIGN) {
		printf("语法错误：第%d行， 缺少':='-assignmentrest\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少':='-assignmentrest\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[1] = Exp();
	return t;
}
//41条件语句处理分析程序  
TreeNode* ConditionalStm(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = IfK;
	if (NowToken->Lex != IF) {
		//error
		printf("语法错误：第%d行， 缺少 IF  -ConditionalStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 IF  -ConditionalStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[0] = Exp();
	// 调用表达式处理函数 Exp()，返回值赋给新节点成员 child[0]，作为 IF 语句的条件表达式
	if (NowToken->Lex != THEN) {//条件为真
		//error
		printf("语法错误：第%d行， 缺少 THEN -ConditionalStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 THEN -ConditionalStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[1] = StmList();//??????Stml
	// 调用语句处理函数 StmL()，返回值赋给新节点成员 child[1]，作为条件为真的处理语句
	if (NowToken->Lex == ELSE) {
		NowToken = NowToken->next;
		t->child[2] = StmList();//条件为假
	}
	if (NowToken->Lex != FI) {//FI 则用于结束整个 IF 语句
		//error
		printf("语法错误：第%d行， 缺少 FI -ConditionalStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 FI -ConditionalStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//42循环语句处理分析程序  
TreeNode* LoopStm(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = WhileK;
	if (NowToken->Lex != WHILE) {
		//error
		printf("语法错误：第%d行， 缺少 WHILE -LoopStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 WHILE -LoopStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[0] = Exp();//while条件表达式

	if (NowToken->Lex != DO) {
		//error
		printf("语法错误：第%d行， 缺少 DO -LoopStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 DO -LoopStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[1] = StmList();  //不同于条件语句中语句序列函数??????
	if (NowToken->Lex != ENDWH) {
		printf("语法错误：第%d行， 缺少 ENDWH -LoopStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 ENDWH -LoopStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//43输入语句的处理分析程序  
TreeNode* InputStm(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = ReadK;
	if (NowToken->Lex != READ) {
		//error
		printf("语法错误：第%d行， 缺少 READ -InputStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 READ -InputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex != LPAREN) {
		//error
		printf("语法错误：第%d行， 缺少  RPAREN -InputStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少  RPAREN -InputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex == ID) {
		(t->name).push_back(NowToken->Sem); // 将标识符名称赋值给语法树节点的 name 成员
		t->idnum++; // 一个节点中标识符的个数
	}
	else {
		//error
		printf("语法错误：第%d行， 缺少ID -InputStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少ID -InputStm\n", NowToken->Lineshow);
		SynErrorNum++;
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex != RPAREN) {
		printf("语法错误：第%d行， 缺少LPAREN -InputStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少LPAREN -InputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;
	return t;
}

//44输出语句处理分析程序  
TreeNode* OutputStm(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = WriteK;
	if (NowToken->Lex != WRITE) {
		//error
		printf("语法错误：第%d行， 缺少WRITE -OutputStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少WRITE -OutputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex != LPAREN) {
		//error
		printf("语法错误：第%d行， 缺少LPAREN -OutputStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少LPAREN -OutputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[0] = Exp();
	if (NowToken->Lex != RPAREN) {
		//error
		printf("语法错误：第%d行， 缺少RPAREN  -OutputStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少RPAREN  -OutputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//45过程返回语句处理分析程序 
TreeNode* ReturnStm(void) {  //多了(  )  //带返回值的return?
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = ReturnK;
	if (NowToken->Lex != RETURN) {
		//error
		printf("语法错误：第%d行， 缺少RETURN -ReturnStm\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少RETURN -ReturnStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//46过程调用语句处理分析程序  
TreeNode* CallStmRest(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = CallK;
	t->name.push_back(temp_name);
	t->idnum++;
	if (NowToken->Lex != LPAREN) {
		//error
		printf("语法错误：第%d行， 缺少LPAREN -CallStmRest\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少LPAREN -CallStmRest\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[0] = ActParamList();

	if (NowToken->Lex != RPAREN) {
		//error
		printf("语法错误：第%d行， 缺少RPAREN -CallStmRest\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少RPAREN -CallStmRest\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//47实参部分处理程序  //有参数或无参数
TreeNode* ActParamList(void) {  //token == "(" || type == "ID" || type == "INTC" || type == "CHARC"
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	//如果当前单词是 RPAREN，表示没有实参，直接返回 NULL
	if (NowToken->Lex == RPAREN) {
		return NULL;
	}
	//如果当前单词是 ID 或 INTC，表示有实参，创建一个新的语法树节点 t，并设置其类型为表达式类型（ExpK）。
	else if (NowToken->Lex == ID || NowToken->Lex == INTC || NowToken->Lex == CHARC || NowToken->Lex == LPAREN) {
		t = Exp();
		t->sibling = ActParamMore();
		return t;
	}
	else {
		//error?
		printf("语法错误：第%d行， 缺少 RPAREN or TypeName -ActParamList\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 RPAREN or TypeName -ActParamList\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

//48更多实参部分处理分析程序   
TreeNode* ActParamMore(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	if (NowToken->Lex == RPAREN) {
		return NULL;
	}
	else if (NowToken->Lex == COMMA) {
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		t = ActParamList();
		return t;
	}
	else {
		//error
		printf("语法错误：第%d行， 缺少  ')' or ',' -ActParamMore\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少  ')' or ',' -ActParamMore\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

//49表达式递归处理分析程序     LT/EQ
TreeNode* Exp(void) {
	TreeNode* t = Simple_exp();
	if (NowToken->Lex == LT || NowToken->Lex == EQ) {
		TreeNode* p = new TreeNode();
		InitTreeNode(p);
		p->nodekind = ExpK;// 设置节点类型为表达式类型
		p->kind.exp = OpK;// 设置表达式类型为运算符类型
		p->attr.ExpAttr.op = NowToken->Lex; // 将关系运算符赋值给节点的 attr
		p->child[0] = t;
		t = p;
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		if (t != NULL) t->child[1] = Simple_exp();
		return t;
	}
	//这里没有ifc的错误！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	return t;
}

//50简单表达式递归处理分析程序     PLUS/MINUS
TreeNode* Simple_exp(void) {
	TreeNode* t = term();
	while (NowToken->Lex == PLUS || NowToken->Lex == MINUS) {
		TreeNode* p = new TreeNode();
		InitTreeNode(p);
		p->nodekind = ExpK;
		p->kind.exp = OpK;
		p->attr.ExpAttr.op = NowToken->Lex;
		p->child[0] = t;
		t = p;
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		t->child[1] = term();
	}
	return t;
}
//51项递归处理分析程序     TIMES/OVER
TreeNode* term(void) {
	TreeNode* t = factor();
	while (NowToken->Lex == TIMES || NowToken->Lex == OVER) {
		TreeNode* p = new TreeNode();
		InitTreeNode(p);
		p->nodekind = ExpK;
		p->kind.exp = OpK;
		p->attr.ExpAttr.op = NowToken->Lex;
		p->child[0] = t;
		t = p;
		NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
		t->child[1] = factor();
	}
	return t;
}

//52因子递归处理分析程序    INIC/ID/LPAREN  //charc?????
TreeNode* factor(void) {
	if (NowToken->Lex == INTC) {
		TreeNode* t = new TreeNode();
		InitTreeNode(t);
		t->nodekind = ExpK;
		t->kind.exp = ConstK;
		t->attr.ExpAttr.val = atoi(NowToken->Sem);//字符串转换成整数
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return t;
	}
	else if (NowToken->Lex == ID) {
		return variable();
	}
	else if (NowToken->Lex == CHARC) {
		//字符常量
		TreeNode* t = new TreeNode();InitTreeNode(t);
		t->nodekind = ExpK;
		t->kind.exp = ConstK;
		t->attr.ExpAttr.val = NowToken->Sem[1];
		t->attr.ExpAttr.ischar = true;
		NowToken = NowToken->next;
		SynLine = NowToken->Lineshow;
		return t;
	}
	else if (NowToken->Lex == LPAREN) {
		NowToken = NowToken->next;
		TreeNode* t = Exp();
		if (NowToken->Lex != RPAREN) {
			printf("语法错误：第%d行， 缺少 ） -factor\n", NowToken->Lineshow);
			fprintf(outfile, "语法错误：第%d行， 缺少 ） -factor\n", NowToken->Lineshow);
			SynErrorNum++;
			exit(1);
		}
		NowToken = NowToken->next;
		return t;
	}
	else {
		printf("语法错误：第%d行，尝试解析一个表达式因子（factor），但是遇到了无法识别的token） -factor\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行，尝试解析一个表达式因子（factor），但是遇到了无法识别的token） -factor\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;
		return NULL;
	}
}

//53变量处理程序      
TreeNode* variable(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = ExpK;
	t->kind.exp = VariK;//生成新的表达式节点t
	if (NowToken->Lex == ID) {//ID!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		t->name.push_back(NowToken->Sem);
		t->idnum++;
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	variMore(t);
	return t;
}

//54变量其余部分处理程序    
void variMore(TreeNode* t) {
	if (NowToken->Lex == ASSIGN || NowToken->Lex == TIMES || NowToken->Lex == EQ || NowToken->Lex == LT || NowToken->Lex == GT || NowToken->Lex == PLUS
		|| NowToken->Lex == MINUS || NowToken->Lex == OVER || NowToken->Lex == RPAREN || NowToken->Lex == RMIDPAREN ||
		NowToken->Lex == SEMI || NowToken->Lex == COMMA || NowToken->Lex == THEN || NowToken->Lex == ELSE || NowToken->Lex == FI
		|| NowToken->Lex == DO || NowToken->Lex == ENDWH || NowToken->Lex == END)
	{
		t->attr.ExpAttr.varkind = IdV;
		return;
	}
	else if (NowToken->Lex == LMIDPAREN) {
		t->attr.ExpAttr.varkind = ArrayMembV;
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		t->child[0] = Exp();
		if (NowToken->Lex != RMIDPAREN) {
			printf("语法错误：第%d行， 缺少 ] -varimore\n", NowToken->Lineshow);
			fprintf(outfile, "语法错误：第%d行， 缺少 ] -varimore\n", NowToken->Lineshow);
			SynErrorNum++;
			exit(1);
		}
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	}
	else if (NowToken->Lex == DOT) {
		t->attr.ExpAttr.varkind = FieldMembV;
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		t->child[0] = fieldvar();
	}
	else {
		printf("语法错误：第%d行， 缺少 [或者 . -varimore\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 [或者 . -varimore\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	}
}

//55域变量部分处理过程  
TreeNode* fieldvar(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	if (t != NULL && NowToken->Lex == ID) {
		t->nodekind = ExpK;
		t->kind.exp = VariK;
		(t->name).push_back(NowToken->Sem);
		t->idnum++;
	}
	else {
		//error
		printf("语法错误：第%d行， 缺少 ID -fieldvar\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少 ID -fieldvar\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	fieldvarMore(t);
	return t;
}


void fieldvarMore(TreeNode* t) {//[]
	switch (NowToken->Lex) {
	case PLUS:
	case ELSE:
	case EQ:
	case DO:
	case FI:
	case MINUS:
	case RPAREN:
	case SEMI:
	case TIMES:
	case ENDWH:
	case RMIDPAREN:
	case COMMA:
	case END:
	case THEN:
	case ASSIGN:
	case OVER:
	case LT:
		break;
	case LMIDPAREN:
		NowToken = NowToken->next;
		t->child[0] = Exp();
		t->attr.ExpAttr.varkind = ArrayMembV;
		if (NowToken->Lex!=RMIDPAREN) {
			SynErrorNum++;
			printf("语法错误：第%d行， 缺少] -fieldvarMore\n", NowToken->Lineshow);
			fprintf(outfile, "语法错误：第%d行， 缺少] -fieldvarMore\n", NowToken->Lineshow);
			exit(1);
			
		}

		NowToken = NowToken->next;SynLine = NowToken->Lineshow;
		break;
	default:
		printf("语法错误：第%d行， 表达式结束不正确 -fieldvarMore\n", NowToken->Lineshow);
		fprintf(outfile, "语法错误：第%d行， 缺少] -fieldvarMore\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
		break;
	}
}