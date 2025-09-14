#pragma once
#include "LexicalAnalysis.h"
#include "Error_handler.h"
#include "syntaxAnalysis.h"
#include "total.h"
#include <iostream>
using namespace std;
//��ǰtoken�����к�SynLine
int SynLine = 0;
TokenNode* NowToken = NULL;//���ڵ�һ�ε���ʱ�򱻸���head
int SynErrorNum = 0;//�﷨�����Ƿ��д���
//����program()�����������﷨������


int MatchToken(LexType p) {
	//1ƥ��ɹ�
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
	t->nodekind = DecK;  // Ĭ���������ڵ�
	t->kind.dec = decNull;  // δ��������
	t->name.clear();
	t->idnum = 0;
	// ��ʼ�����ű�ָ��
	for (int i = 0; i < 10; i++) {
		t->table[i] = NULL;
	}
	t->type_name = "";
	t->attr.ArrayAttr.low = 0;
	t->attr.ArrayAttr.up = 0;
	t->attr.ExpAttr.ischar = false;//����������
	t->attr.ArrayAttr.childtype = decNull;
	t->attr.ProcAttr.paramt = paramNull;
}


struct TreeNode* parse(TokenNode* head) {
	NowToken = head;
	SynLine = NowToken->Lineshow;
	TreeNode* t = program();
	//����ڽ��������з����﷨������������NULL
	if (SynErrorNum > 0) {
		printf("�﷨����ʧ�ܣ����� %d ���﷨���󣬳�����ֹ\n", SynErrorNum);
		fprintf(outfile, "�﷨����ʧ�ܣ����� %d ���﷨���󣬳�����ֹ\n", SynErrorNum);
		return NULL;
	}
	if (NowToken->Lex != ENDFILE) {
		printf("�﷨���󣺵� %d�У��﷨��������\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵� %d�У��﷨��������\n", NowToken->Lineshow);
		return NULL; //ֱ�ӷ���NULL����ʾ����ʧ��
	}
	return t;
}

TreeNode* ProgramHead() {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = PheadK;
	//��ǰ���ʵ�PROGRAMƥ��
	if (NowToken->Lex != PROGRAM) {
		printf("�﷨���󣺵�%d�У�ȱ�ٳ���ͷprogram-programhead", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ�ٳ���ͷprogram-programhead", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;
	SynLine = NowToken->Lineshow;
	if (NowToken->Lex == ID) {
		t->name.push_back(NowToken->Sem);
		t->idnum++;
	}
	else {
		printf("�﷨���󣺵�%d�У�ȱ�ٳ�����-programhead", NowToken->Lineshow); 
		fprintf(outfile,"�﷨���󣺵�%d�У�ȱ�ٳ�����-programhead", NowToken->Lineshow);
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
	//��ǰ���ʺ�DOTƥ��
	if (NowToken->Lex != DOT) {
		printf("�﷨���󣺵�%d�У�\'.\'ȱʧ-program\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�\'.\'ȱʧ-program\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	//ָ�����
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
		printf("�﷨���󣺵�%d�У�ȱʧType-TypeDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱʧType-TypeDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t = TypeDecList();
	if (t == NULL) {
		printf("�﷨���󣺵�%d�У�ȱʧTypeDecList-TypeDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱʧTypeDecList-TypeDeclaration\n", NowToken->Lineshow);
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
		printf("�﷨���󣺵�%d�У����������׶�ȱ��TYPE-TypeDec\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У����������׶�ȱ��TYPE-TypeDec\n", NowToken->Lineshow);
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
		printf("�﷨���󣺵�%d�У�ȱ��'='-TypeDecList\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��'='-TypeDecList\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	TypeDef(t);
	if (NowToken->Lex != SEMI) {
		printf("�﷨���󣺵�%d�У�ȱ��';'-TypeDecList\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��';'-TypeDecList\n", NowToken->Lineshow);
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
		return NULL;//*���ﵽ����procdeure����program������������
	}
	else if (NowToken->Lex == ID) {
		t = TypeDecList();
		return t;
	}
	else {
		printf("�﷨���󣺵�%d�У�ȱ��VAR/ID/PROCEDURE/BEGIN�ȹؼ���-TypeDecMore\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��VAR/ID/PROCEDURE/BEGIN�ȹؼ���-TypeDecMore\n", NowToken->Lineshow);
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
			printf("�﷨���󣺵�%d�У������ڵ�-TypeId\n", NowToken->Lineshow);
			fprintf(outfile, "�﷨���󣺵�%d�У������ڵ�-TypeId\n", NowToken->Lineshow);
			SynErrorNum++;exit(1);
		}
		else {
			printf("�﷨���󣺵�%d�У���Ҫ��ʶ��-TypeId\n", NowToken->Lineshow);
			fprintf(outfile, "�﷨���󣺵�%d�У���Ҫ��ʶ��-TypeId\n", NowToken->Lineshow);
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
		printf("�﷨�������Ͷ�����ִ��󵥴�-TypeDef\n");
		fprintf(outfile, "�﷨�������Ͷ�����ִ��󵥴�-TypeDef\n");
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	}
}

//MR-----------------------------------------------------------------------
// 11. BaseType����������������ͣ�INTEGER �� CHAR��
void BaseType(TreeNode* t) {
	if (NowToken->Lex == INTEGER) {
		t->kind.dec = DecKind::IntegerK;
	}
	else if (NowToken->Lex == CHAR) {
		t->kind.dec = DecKind::CharK;
	}
	else {
		printf("�﷨���󣺵�%d�У���ǰtoken���ǻ������ͣ�INTEGER �� CHAR��, ������һ��token... - void BaseType(TreeNode * t)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰtoken���ǻ������ͣ�INTEGER �� CHAR��, ������һ��token... - void BaseType(TreeNode * t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	////////////////////////////////////////////////////////////////////////ÿһ���жϽ�����Ҫ������һ��token
	NowToken = NowToken->next;     SynLine = NowToken->Lineshow;
}

// 12. StructureType����������ṹ���ͣ�ARRAY �� RECORD��
void StructureType(TreeNode* t) {
	if (NowToken->Lex == ARRAY) { // �����ǰtoken��ARRAY
		t->kind.dec = DecKind::ArrayK; // ���ýڵ�����ΪArrayK
		ArrayType(t); // ����ArrayType����������������
	}
	else if (NowToken->Lex == RECORD) { // �����ǰtoken��RECORD
		t->kind.dec = DecKind::RecordK; // ���ýڵ�����ΪRecordK
		RecType(t); // ����RecType���������¼����
	}
	else { // �����ǰtoken�Ȳ���ARRAYҲ����RECORD
		printf("�﷨���󣺵�%d�У���ǰtoken���ǽṹ���ͣ�ARRAY �� RECORD��,������һ��token... -void StructureType(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰtoken���ǽṹ���ͣ�ARRAY �� RECORD��,������һ��token... -void StructureType(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
	}
}

// 13. ArrayType����������������������
void ArrayType(TreeNode* t) {
	if (NowToken->Lex != ARRAY) {
		printf("�﷨���󣺵�%d�У�ӦΪ ARRAY, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ ARRAY, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
	if (NowToken->Lex != LMIDPAREN) { // �����ǰtoken������������[
		printf("�﷨���󣺵�%d�У�ӦΪ [, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ [, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
	if (NowToken->Lex != INTC) {
		printf("�﷨���󣺵�%d�У�ӦΪ�������ͳ����½�, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ�������ͳ����½�, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	t->attr.ArrayAttr.low = atoi(NowToken->Sem); // ���½��ַ���ת��Ϊ�������洢
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
	if (NowToken->Lex != UNDERANGE) { // �����ǰtoken����..
		printf("�﷨���󣺵�%d�У�ӦΪ .., ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ .., ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;

	if (NowToken->Lex != INTC) { // �����ǰtoken�������ͳ���
		printf("�﷨���󣺵�%d�У�ӦΪ�������ͳ����Ͻ�, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ�������ͳ����Ͻ�, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	t->attr.ArrayAttr.up = atoi(NowToken->Sem); // ���Ͻ��ַ���ת��Ϊ�������洢
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
	if (NowToken->Lex != RMIDPAREN) { // �����ǰtoken������������]
		printf("�﷨���󣺵�%d�У�ӦΪ ], ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ ], ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;

	if (NowToken->Lex != OF) { // �����ǰtoken����OF
		printf("�﷨���󣺵�%d�У�ӦΪ OF, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ OF, ���ҵ��� %s -void ArrayType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next; // ��ȡ��һ��token
	SynLine = NowToken->Lineshow;
	BaseType(t);
	t->attr.ArrayAttr.childtype = t->kind.dec;
	t->kind.dec = DecKind::ArrayK;
}

// 14. RecType�����������¼��������
void RecType(TreeNode* t) {
	if (NowToken->Lex != RECORD) {
		printf("�﷨���󣺵�%d�У�ӦΪ RECORD, ���ҵ��� %s -void RecType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ RECORD, ���ҵ��� %s -void RecType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
	TreeNode* p = FieldDecList(); // ����FieldDecList���������¼�е��������б�
	if (p != NULL) { // ���FieldDecList���ص�ָ����Ч
		t->child[0] = p;
		t->childnum = 1;
	}
	else {
		printf("�﷨���󣺵�%d�У�δ�ܽ����ֶ������б� -void RecType(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�δ�ܽ����ֶ������б� -void RecType(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	if (NowToken->Lex != END) { // �����ǰtoken����END
		printf("�﷨���󣺵�%d�У�ӦΪ END, ���ҵ��� %s -void RecType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ END, ���ҵ��� %s -void RecType(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
}

// 15. FieldDecList�����������������б�
TreeNode* FieldDecList(void) {
	TreeNode* t = new TreeNode(); // �����µ��﷨���ڵ�
	InitTreeNode(t); // ��ʼ���ڵ�
	t->nodekind = NodeKind::DecK; // ���ýڵ�����Ϊ�����ڵ�

	if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR) { // �����ǰtoken��INTEGER��CHAR
		BaseType(t); // ����BaseType���������������
		IdList(t); // ����IdList���������ʶ���б�
		if (NowToken->Lex == SEMI) { // �����ǰtoken�Ƿֺ�;
			NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
			t->sibling = FieldDecMore(); // ����FieldDecMore������������������
			return t;
		}
		else { // �����ǰtoken���Ƿֺ�;
			printf("�﷨���󣺵�%d�У��������б���ȱ�ٷֺ� -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
			fprintf(outfile, "�﷨���󣺵�%d�У��������б���ȱ�ٷֺ� -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
			SynErrorNum++;exit(1);
			return t;
		}
	}
	else if (NowToken->Lex == ARRAY) { // �����ǰtoken��ARRAY
		ArrayType(t); // ����ArrayType����������������
		IdList(t); // ����IdList���������ʶ���б�
		if (NowToken->Lex == SEMI) {
			NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
			t->sibling = FieldDecMore();
			return t;
		}
		else { // �����ǰtoken���Ƿֺ�;
			printf("�﷨���󣺵�%d�У��������б���ȱ�ٷֺ� -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
			fprintf(outfile, "�﷨���󣺵�%d�У��������б���ȱ�ٷֺ� -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
			SynErrorNum++;exit(1);
			return t;
		}
	}
	else { // �����ǰtoken�Ȳ���INTEGER��CHARҲ����ARRAY
		printf("�﷨���󣺵�%d�У��������б��г��������token, ���� INTEGER��CHAR �� ARRAY -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У��������б��г��������token, ���� INTEGER��CHAR �� ARRAY -TreeNode* FieldDecList(void)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
		return NULL;
	}
}

// 16. FieldDecMore��������������������
TreeNode* FieldDecMore(void) {
	TreeNode* t = NULL; // ��ʼ��tΪNULL

	if (NowToken->Lex == END) { // �����ǰtoken��END
		return NULL; // ֱ�ӷ���NULL
	}
	else if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR || NowToken->Lex == ARRAY) { // �����ǰtoken��INTEGER��CHAR��ARRAY
		t = FieldDecList(); // ����FieldDecList��������������
		return t; // ���ؽ��
	}
	else { // �����ǰtoken�Ȳ���ENDҲ����INTEGER��CHAR��ARRAY
		printf("�﷨���󣺵�%d�У��������б��г��������token, ���� INTEGER��CHAR �� ARRAY -TreeNode* FieldDecMore(void)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У��������б��г��������token, ���� INTEGER��CHAR �� ARRAY -TreeNode* FieldDecMore(void)\n", NowToken->Lineshow);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
		return NULL; // ����NULL
	}
}

// 17. IdList�����������ʶ���б�
void IdList(TreeNode* t) {
	if (NowToken->Lex == ID) { // �����ǰtoken��ID
		t->name.push_back(NowToken->Sem); // ����ʶ�����ƴ洢���ڵ���
		t->idnum++; // ��ʶ��������1
	}
	else {
		printf("�﷨���󣺵�%d�У�ӦΪID -void IdList(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪID -void IdList(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}

	NowToken = NowToken->next; 	SynLine = NowToken->Lineshow;
	IdMore(t); // ����IdMore�����������ı�ʶ��
}

// 18. IdMore�������������ı�ʶ��
void IdMore(TreeNode* t) {
	if (NowToken->Lex == COMMA) {
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		IdList(t);
	}
	else if (NowToken->Lex == SEMI) { // �����ǰtoken�Ƿֺ�;
		return; // ֱ�ӷ���
	}
	else { // �����ǰtoken�Ȳ��Ƿֺ�;Ҳ���Ƕ���,
		printf("�﷨���󣺵�%d�У���ǰ���ʼȲ���','��Ҳ����';'����ȡ��һ������... -void IdMore(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʼȲ���','��Ҳ����';'����ȡ��һ������... -void IdMore(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
	}
}

// 19. VarDec���������������������
TreeNode* VarDec(void) {
	TreeNode* t = NULL; // ��ʼ�� t Ϊ NULL
	if (NowToken->Lex == PROCEDURE || NowToken->Lex == BEGIN) {
		// �����ǰtoken��PROCEDURE��BEGIN��˵��û�б����������֣�ֱ�ӷ���NULL
		return NULL;
	}
	else if (NowToken->Lex == VAR) {
		// �����ǰtoken��VAR��˵���б����������֣�����VarDeclaration��������
		t = VarDeclaration();
		return t;
	}
	else {
		// �����ǰtoken�Ȳ���PROCEDURE��BEGIN��Ҳ����VAR��˵���﷨����
		printf("�﷨���󣺵�%d�У���ǰ���ʲ���VAR����ȡ��һ������... -TreeNode* VarDec(void)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʲ���VAR����ȡ��һ������... -TreeNode* VarDec(void)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next; // ��ȡ��һ��toke
		SynLine = NowToken->Lineshow;
		return NULL;
	}
}

// 20. VarDeclaration������������������ľ����߼�
TreeNode* VarDeclaration(void) {
	TreeNode* t = NULL;
	if (NowToken->Lex != VAR) {
		// �����ǰtoken����VAR��˵���﷨����
		printf("�﷨���󣺵�%d�У���ǰ���ʲ���VAR -TreeNode* VarDeclaration(void)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʲ���VAR -TreeNode* VarDeclaration(void)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;     SynLine = NowToken->Lineshow;
	// ���� VarDecList ����������������б�
	t = VarDecList();
	// ��� VarDecList ���ص�ָ���Ƿ���Ч
	if (t == NULL) {
		printf("�﷨���󣺵�%d�У����������б����ʧ�� -TreeNode* VarDeclaration(void)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У����������б����ʧ�� -TreeNode* VarDeclaration(void)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	return t;
}

// 21. VarDecList������������������б�
TreeNode* VarDecList(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = NodeKind::DecK; // ���ýڵ�����Ϊ�����ڵ�
	if (NowToken->Lex == CHAR || NowToken->Lex == INTEGER || NowToken->Lex == ARRAY || NowToken->Lex == RECORD || NowToken->Lex == ID) {
		TypeDef(t);
		VarIdList(t);
		if (NowToken->Lex != SEMI) {
			printf("�﷨���󣺵�%d�У�ȱ��\";\" -TreeNode* VarDecList(void)\n", NowToken->Lineshow);
			fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��\";\" -TreeNode* VarDecList(void)\n", NowToken->Lineshow);
			SynErrorNum++;exit(1);
		}
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		t->sibling = VarDecMore();
		return t;
	}
	else {
		printf("�﷨���󣺵�%d�У�ȱ��typename -TreeNode* VarDecList\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��typename -TreeNode* VarDecList\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

// 22. VarDecMore�������������ı�������
TreeNode* VarDecMore(void) {
	TreeNode* t = NULL; // ��ʼ�� t Ϊ NULL
	if (NowToken->Lex == PROCEDURE || NowToken->Lex == BEGIN) {
		// �����ǰtoken��PROCEDURE��BEGIN��˵�������������ֽ�����ֱ�ӷ���NULL
		return NULL;
	}
	else if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR || NowToken->Lex == RECORD || NowToken->Lex == ARRAY || NowToken->Lex == ID) {
		// �����ǰtoken��INTEGER��CHAR��RECORD��ARRAY��ID��˵�����и���ı�������
		t = VarDecList(); // ���� VarDecList ���������������
		return t; // ���ش�����
	}
	else {
		// �����ǰtoken�����������ͣ�˵���﷨����
		printf("�﷨���󣺵�%d�У���ǰ���ʲ��Ǳ�����PROCEDURE��BEGIN��INTEGER��CHAR��RECORD��ARRAY �� ID������ȡ��һ������.. -TreeNode* VarDecMore(void)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʲ��Ǳ�����PROCEDURE��BEGIN��INTEGER��CHAR��RECORD��ARRAY �� ID������ȡ��һ������.. -TreeNode* VarDecMore(void)\n", NowToken->Lineshow);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
		return NULL; // ����NULL
	}
}

// 23. VarIdList���������������ʶ���б�
void VarIdList(TreeNode* t) {
	// ��鵱ǰtoken�Ƿ�ΪID
	if (NowToken->Lex == ID) {
		// �����ǰtoken��ID������ʶ�����ƴ洢���ڵ��name������
		t->name.push_back(NowToken->Sem);
		t->idnum++; // ��ʶ��������1
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
	}
	else {
		// �����ǰtoken����ID��˵���﷨����
		printf("�﷨���󣺵�%d�У�ӦΪ ID, �������� %s -VarIdList\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ ID, �������� %s -VarIdList\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
	}
	// ���� VarIdMore �����������ı�ʶ��
	VarIdMore(t);
}

// 24. VarIdMore�������������ı�����ʶ��
void VarIdMore(TreeNode* t) {
	// ��鵱ǰtoken������
	if (NowToken->Lex == SEMI) {
		// �����ǰtoken�Ƿֺ�;��˵��������ʶ���б������ֱ�ӷ���
		return;
	}
	else if (NowToken->Lex == COMMA) {
		// �����ǰtoken�Ƕ���,��˵�����и���ı�ʶ��
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
		VarIdList(t); // ���� VarIdList �����������ı�ʶ��
	}
	else {
		// �����ǰtoken�Ȳ��Ƿֺ�;Ҳ���Ƕ���,��˵���﷨����
		printf("�﷨���󣺵�%d�У���ǰ���ʼȲ���','��Ҳ����';'����ȡ��һ������.. -void VarIdMore(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʼȲ���','��Ҳ����';'����ȡ��һ������.. -void VarIdMore(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
	}
}

// 25. ProcDec���������������������
TreeNode* ProcDec(void) {
	TreeNode* t = NULL; // ��ʼ�� t Ϊ NULL
	if (NowToken->Lex == BEGIN) {
		return NULL;
	}
	else if (NowToken->Lex == PROCEDURE) {
		t = ProcDeclaration();
		return t;
	}
	else {
		printf("�﷨���󣺵�%d�У���ǰ���ʲ��ǹ���������BEGIN �� PROCEDURE������ȡ��һ������.. -TreeNode* ProcDec(void)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʲ��ǹ���������BEGIN �� PROCEDURE������ȡ��һ������.. -TreeNode* ProcDec(void)\n", NowToken->Lineshow);
		SynErrorNum++; exit(1);
		NowToken = NowToken->next;     SynLine = NowToken->Lineshow;
		return NULL;exit(1);
	}
}

// 26. ProcDeclaration������������������ľ����߼�
TreeNode* ProcDeclaration() {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = ProcDecK;
	if (NowToken->Lex != PROCEDURE) {
		printf("�﷨���󣺵�%d�У�ȱ��procedure-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��procedure-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex == ID) {
		t->name.push_back(NowToken->Sem); // ���������洢���ڵ��name������
		t->idnum++; 
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	}
	else {
		printf("�﷨���󣺵�%d�У�ȱ��program_ID-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��program_ID-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	if (NowToken->Lex != LPAREN) {
		printf("�﷨���󣺵�%d�У�ȱ��'('-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��'('-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	ParamList(t);
	if (NowToken->Lex != RPAREN) {
		printf("�﷨���󣺵�%d�У�ȱ��')'-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��')'-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex != SEMI) {
		printf("�﷨���󣺵�%d�У�ȱ��';'-ProcDeclaration\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У�ȱ��';'-ProcDeclaration\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[1] = ProcDecPart();
	t->child[2] = procBody();
	t->childnum += 2;
	t->sibling = ProcDec();
	return t;
}

// 27. ParamList��������������б�
void ParamList(TreeNode* t) {
	TreeNode* p = NULL; // ��ʼ�� p Ϊ NULL
	if (NowToken->Lex == RPAREN) {
		// �����ǰtoken��������)��˵�������б�Ϊ�գ�ֱ�ӷ���
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return;
	}
	else if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR || NowToken->Lex == RECORD || NowToken->Lex == ARRAY || NowToken->Lex == ID || NowToken->Lex == VAR) {
		// �����ǰtoken��INTEGER��CHAR��RECORD��ARRAY��ID��VAR��˵���в�������
		p = ParamDecList(); // ���� ParamDecList �������������������
		if (p != NULL) {
			t->child[0] = p; // �������ֵ�� t->child[0]
			t->childnum = 1; // �����ӽڵ�����
		}
	}
	else {
		// �����ǰtoken�����������ͣ�˵���﷨����
		printf("�﷨���󣺵�%d�У���ǰ���ʼȲ��ǲ������ͣ�INTEGER��CHAR��RECORD��ARRAY��ID��VAR����Ҳ���� ')'����ȡ��һ������.. -void ParamList(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʼȲ��ǲ������ͣ�INTEGER��CHAR��RECORD��ARRAY��ID��VAR����Ҳ���� ')'����ȡ��һ������.. -void ParamList(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++; 
		exit(1);
		NowToken = NowToken->next; 
		SynLine = NowToken->Lineshow;
	}
}

// 28. ParamDecList���������������������
TreeNode* ParamDecList(void) {
	TreeNode* t = Param(); // ���� Param ������������������
	TreeNode* p = ParamMore(); // ���� ParamMore �����������Ĳ�������
	if (p != NULL) {
		t->sibling = p; // ��� ParamMore ���ص�ָ����Ч�����丳ֵ�� t �� sibling ��Ա
	}
	return t;
}

// 29. ParamMore�������������Ĳ�������
TreeNode* ParamMore(void) {
	TreeNode* t = NULL; // ��ʼ�� t Ϊ NULL
	if (NowToken->Lex == RPAREN) {
		// �����ǰtoken��������)��˵����������������ֱ�ӷ���NULL
		return NULL;
	}
	else if (NowToken->Lex == SEMI) {
		// �����ǰtoken�Ƿֺ�;��˵�����и���Ĳ�������
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
		t = ParamDecList(); // ���� ParamDecList �������������������
		if (t == NULL) {
			// ��� ParamDecList ���� NULL��˵��������������ʧ��
			printf("�﷨���󣺵�%d�У�ȱ�ٲ����б��\";\" -TreeNode* ParamMore(void)\n", NowToken->Lineshow);
			fprintf(outfile, "�﷨���󣺵�%d�У�ȱ�ٲ����б��\";\" -TreeNode* ParamMore(void)\n", NowToken->Lineshow);
			SynErrorNum++; 
			exit(1);
		}
		return t;
	}
	else {
		// �����ǰtoken�Ȳ��Ƿֺ�;Ҳ����������)��˵���﷨����
		printf("�﷨���󣺵�%d�У���ǰ���ʼȲ��� ';'��Ҳ���� ')'����ȡ��һ������.. -TreeNode* ParamMore(void)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʼȲ��� ';'��Ҳ���� ')'����ȡ��һ������.. -TreeNode* ParamMore(void)\n", NowToken->Lineshow);
		SynErrorNum++; 
		exit(1);
		NowToken = NowToken->next; 
		SynLine = NowToken->Lineshow;
		return NULL; 
	}
}

// 30. Param��������������������
TreeNode* Param(void) {
	TreeNode* t = new TreeNode(); // �����µ��﷨���ڵ�
	InitTreeNode(t); // ��ʼ���ڵ�
	t->nodekind = NodeKind::DecK; // ���ýڵ�����Ϊ�����ڵ�

	if (NowToken->Lex == VAR) {
		// �����ǰtoken��VAR��˵���Ǳ��
		t->attr.ProcAttr.paramt = ParamType::varparamType; // ���ò�������Ϊ���
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
		// ���� TypeDef �� FormList �����������Ͷ���Ͳ����б�
		TypeDef(t);
		FormList(t);
	}
	else if (NowToken->Lex == INTEGER || NowToken->Lex == CHAR || NowToken->Lex == RECORD || NowToken->Lex == ARRAY || NowToken->Lex == ID) {
		// �����ǰtoken��INTEGER��CHAR��RECORD��ARRAY��ID��˵����ֵ��
		t->attr.ProcAttr.paramt = ParamType::valparamType; // ���ò�������Ϊֵ��
		// ���� TypeDef �� FormList �����������Ͷ���Ͳ����б�
		TypeDef(t);
		FormList(t);
	}
	else {
		// �����ǰtoken�����������ͣ�˵���﷨����
		printf("�﷨���󣺵�%d�У���ǰ���ʼȲ��Ǳ�Σ�VAR��,Ҳ����ֵ�Σ�INTEGER, CHAR, RECORD, ARRAY �� ID�� -TreeNode* Param(void)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʼȲ��Ǳ�Σ�VAR��,Ҳ����ֵ�Σ�INTEGER, CHAR, RECORD, ARRAY �� ID�� -TreeNode* Param(void)\n", NowToken->Lineshow);
		SynErrorNum++; // �����﷨�������
		exit(1);
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
	}
	return t;
}

// 31. FormList����������������б�
void FormList(TreeNode* t) {
	if (NowToken->Lex == ID) {
		// �����ǰtoken��ID��˵���ǲ�����
		t->name.push_back(NowToken->Sem); // ���������洢���ڵ��name������
		t->idnum++;
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
	}
	else {
		// �����ǰtoken����ID��˵���﷨����
		printf("�﷨���󣺵�%d�У�ӦΪ ID, ���ҵ��� %s -void FormList(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У�ӦΪ ID, ���ҵ��� %s -void FormList(TreeNode* t)\n", NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++; 
		exit(1);
	}
	// ���� FidMore �����������Ĳ�����
	FidMore(t);
}

// 32. FidMore�������������Ĳ�����
void FidMore(TreeNode* t) {
	if (NowToken->Lex == SEMI || NowToken->Lex == RPAREN) {
		// �����ǰtoken�Ƿֺ�;��������)��˵���������б������ֱ�ӷ���
		return;
	}
	else if (NowToken->Lex == COMMA) {
		// �����ǰtoken�Ƕ���,��˵�����и���Ĳ�����
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
		FormList(t); // ���� FormList �����������Ĳ�����
	}
	else {
		// �����ǰtoken�Ȳ��Ƿֺ�;��������)Ҳ���Ƕ���,��˵���﷨����
		printf("�﷨���󣺵�%d�У���ǰ���ʲ��� ','�� ')' �� ';' -void FidMore(TreeNode* t)\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У���ǰ���ʲ��� ','�� ')' �� ';' -void FidMore(TreeNode* t)\n", NowToken->Lineshow);
		SynErrorNum++; // �����﷨�������
		exit(1);
		NowToken = NowToken->next; // ��ȡ��һ��token
		SynLine = NowToken->Lineshow;
	}
}

//XYN------------------------------------------------------------------

//XYN------------------------------------------------------------------

//33�������������ֵķ����������
TreeNode* ProcDecPart(void) {
	TreeNode* t = DeclarePart();//�����������ֺ���
	if (t == NULL) {
		printf("�﷨���󣺵�%d�У� �������������ֵķ���ʧ��-ProcDecPart\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� �������������ֵķ���ʧ��-ProcDecPart\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	return t;
}

//34�����岿�ִ����������
TreeNode* procBody(void) {
	TreeNode* t = ProgramBody();// ���ó����岿�ֺ���
	if (t == NULL) {
		printf("�﷨���󣺵�%d�У� ȱ�ٳ�����-procBody\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�ٳ�����-procBody\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	return t;
}

//35�����򲿷ִ����������   
TreeNode* ProgramBody(void) {  //BEGIN  END
	TreeNode* t = new TreeNode(); // �����µ��﷨�������ڵ� 
	InitTreeNode(t);//��ʼ���ڵ�
	t->nodekind = StmLK;//���ýڵ�����Ϊ����־����
	if (NowToken->Lex == BEGIN) {
		NowToken = NowToken->next;// �ƶ�����һ������
		SynLine = NowToken->Lineshow;
		t->child[0] = StmList();  // ���� StmList() �����������
	}
	else {
		//error
		printf("�﷨���󣺵�%d�У� ȱ��BEGIN-ProgramBody\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��BEGIN-ProgramBody\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	if (NowToken->Lex != END) {
		printf("�﷨���󣺵�%d�У� ȱ��END-ProgramBody\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��END-ProgramBody\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//36������в��ִ����������   
TreeNode* StmList(void) {
	// �����ǰ������ END����ʾ������н��������� NULL
	if (NowToken->Lex == END) return NULL;
	TreeNode* t = Stm();//���ݹ鴦���������
	TreeNode* p = StmMore();//������䲿�ִ����������
	t->sibling = p;//�ֵܽڵ�
	return t;
}

//37������䲿�ִ����������
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
		printf("�﷨���󣺵�%d�У������������� - �����ֺ�(;)��END��ENDWH��FI��ELSE�����õ�'%s'-StmMore\n",
			NowToken->Lineshow, NowToken->Sem);
		fprintf(outfile, "�﷨���󣺵�%d�У������������� - �����ֺ�(;)��END��ENDWH��FI��ELSE�����õ�'%s'-StmMore\n",
			NowToken->Lineshow, NowToken->Sem);
		SynErrorNum++;
		exit(1);
		return NULL;
	}
}

//38���ݹ鴦���������
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
		temp_name = NowToken->Sem;// �����ʶ������
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		TreeNode* t = AssCall();
		return t;
	}
	else {
		printf("�﷨���󣺵�%d�У� ��俪ʼ����ȷ- Stm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ��俪ʼ����ȷ- Stm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

//39��ֵ���ͺ���������䲿�ֵĴ����������
TreeNode* AssCall(void) {
	if (NowToken->Lex == LPAREN) {
		return CallStmRest();
	}
	else if (NowToken->Lex == LMIDPAREN || NowToken->Lex == ASSIGN || NowToken->Lex == DOT) {
		return AssignmentRest();
	}
	else {
		printf("�﷨���󣺵�%d�У� ȱ��':='  �� '('-AssCall\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��':='  �� '('-AssCall\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

//40��ֵ��䴦���������  ??????
//���Ŀ�����Ե��﷨�����У�С�ڷ��ź͵��ڷ������﷨�ṹ�Ĺؼ���ɲ��֣���ô�ڹ����﷨��ʱ����Ҫ�����ǰ�����ȥ��
//���磬��������䣨�� if ��䣩�У�ͨ����ʹ�ñȽ���������� < �� =���������������ʽ��
//��α���� if x < 10 then ... Ϊ�������Ӧ���﷨���У�< ���Ż���Ϊһ����Ҫ�Ľڵ���ڣ����ڱ�ʾ�����жϵ��߼���
//��û�м�
TreeNode* AssignmentRest(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;//���ýڵ�����Ϊ�������
	t->kind.stmt = AssignK;//�����������Ϊ��ֵ���
	TreeNode* child = new TreeNode();
	InitTreeNode(child);
	child->nodekind = ExpK;
	child->kind.exp = VariK;
	child->name.push_back(temp_name);
	child->idnum++;
	variMore(child);
	t->child[0] = child;
	if (NowToken->Lex != ASSIGN) {
		printf("�﷨���󣺵�%d�У� ȱ��':='-assignmentrest\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��':='-assignmentrest\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[1] = Exp();
	return t;
}
//41������䴦���������  
TreeNode* ConditionalStm(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = IfK;
	if (NowToken->Lex != IF) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ�� IF  -ConditionalStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� IF  -ConditionalStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[0] = Exp();
	// ���ñ��ʽ������ Exp()������ֵ�����½ڵ��Ա child[0]����Ϊ IF �����������ʽ
	if (NowToken->Lex != THEN) {//����Ϊ��
		//error
		printf("�﷨���󣺵�%d�У� ȱ�� THEN -ConditionalStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� THEN -ConditionalStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[1] = StmList();//??????Stml
	// ������䴦���� StmL()������ֵ�����½ڵ��Ա child[1]����Ϊ����Ϊ��Ĵ������
	if (NowToken->Lex == ELSE) {
		NowToken = NowToken->next;
		t->child[2] = StmList();//����Ϊ��
	}
	if (NowToken->Lex != FI) {//FI �����ڽ������� IF ���
		//error
		printf("�﷨���󣺵�%d�У� ȱ�� FI -ConditionalStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� FI -ConditionalStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//42ѭ����䴦���������  
TreeNode* LoopStm(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = WhileK;
	if (NowToken->Lex != WHILE) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ�� WHILE -LoopStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� WHILE -LoopStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[0] = Exp();//while�������ʽ

	if (NowToken->Lex != DO) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ�� DO -LoopStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� DO -LoopStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[1] = StmList();  //��ͬ�����������������к���??????
	if (NowToken->Lex != ENDWH) {
		printf("�﷨���󣺵�%d�У� ȱ�� ENDWH -LoopStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� ENDWH -LoopStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//43�������Ĵ����������  
TreeNode* InputStm(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = ReadK;
	if (NowToken->Lex != READ) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ�� READ -InputStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� READ -InputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex != LPAREN) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ��  RPAREN -InputStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��  RPAREN -InputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex == ID) {
		(t->name).push_back(NowToken->Sem); // ����ʶ�����Ƹ�ֵ���﷨���ڵ�� name ��Ա
		t->idnum++; // һ���ڵ��б�ʶ���ĸ���
	}
	else {
		//error
		printf("�﷨���󣺵�%d�У� ȱ��ID -InputStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��ID -InputStm\n", NowToken->Lineshow);
		SynErrorNum++;
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex != RPAREN) {
		printf("�﷨���󣺵�%d�У� ȱ��LPAREN -InputStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��LPAREN -InputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;
	return t;
}

//44�����䴦���������  
TreeNode* OutputStm(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = WriteK;
	if (NowToken->Lex != WRITE) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ��WRITE -OutputStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��WRITE -OutputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	if (NowToken->Lex != LPAREN) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ��LPAREN -OutputStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��LPAREN -OutputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[0] = Exp();
	if (NowToken->Lex != RPAREN) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ��RPAREN  -OutputStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��RPAREN  -OutputStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//45���̷�����䴦��������� 
TreeNode* ReturnStm(void) {  //����(  )  //������ֵ��return?
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = ReturnK;
	if (NowToken->Lex != RETURN) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ��RETURN -ReturnStm\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��RETURN -ReturnStm\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//46���̵�����䴦���������  
TreeNode* CallStmRest(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = StmtK;
	t->kind.stmt = CallK;
	t->name.push_back(temp_name);
	t->idnum++;
	if (NowToken->Lex != LPAREN) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ��LPAREN -CallStmRest\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��LPAREN -CallStmRest\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	t->child[0] = ActParamList();

	if (NowToken->Lex != RPAREN) {
		//error
		printf("�﷨���󣺵�%d�У� ȱ��RPAREN -CallStmRest\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��RPAREN -CallStmRest\n", NowToken->Lineshow);
		SynErrorNum++;exit(1);
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	return t;
}

//47ʵ�β��ִ������  //�в������޲���
TreeNode* ActParamList(void) {  //token == "(" || type == "ID" || type == "INTC" || type == "CHARC"
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	//�����ǰ������ RPAREN����ʾû��ʵ�Σ�ֱ�ӷ��� NULL
	if (NowToken->Lex == RPAREN) {
		return NULL;
	}
	//�����ǰ������ ID �� INTC����ʾ��ʵ�Σ�����һ���µ��﷨���ڵ� t��������������Ϊ���ʽ���ͣ�ExpK����
	else if (NowToken->Lex == ID || NowToken->Lex == INTC || NowToken->Lex == CHARC || NowToken->Lex == LPAREN) {
		t = Exp();
		t->sibling = ActParamMore();
		return t;
	}
	else {
		//error?
		printf("�﷨���󣺵�%d�У� ȱ�� RPAREN or TypeName -ActParamList\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� RPAREN or TypeName -ActParamList\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

//48����ʵ�β��ִ����������   
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
		printf("�﷨���󣺵�%d�У� ȱ��  ')' or ',' -ActParamMore\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��  ')' or ',' -ActParamMore\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return NULL;
	}
}

//49���ʽ�ݹ鴦���������     LT/EQ
TreeNode* Exp(void) {
	TreeNode* t = Simple_exp();
	if (NowToken->Lex == LT || NowToken->Lex == EQ) {
		TreeNode* p = new TreeNode();
		InitTreeNode(p);
		p->nodekind = ExpK;// ���ýڵ�����Ϊ���ʽ����
		p->kind.exp = OpK;// ���ñ��ʽ����Ϊ���������
		p->attr.ExpAttr.op = NowToken->Lex; // ����ϵ�������ֵ���ڵ�� attr
		p->child[0] = t;
		t = p;
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		if (t != NULL) t->child[1] = Simple_exp();
		return t;
	}
	//����û��ifc�Ĵ��󣡣���������������������������������������������������������������������������������������������������������������������
	return t;
}

//50�򵥱��ʽ�ݹ鴦���������     PLUS/MINUS
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
//51��ݹ鴦���������     TIMES/OVER
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

//52���ӵݹ鴦���������    INIC/ID/LPAREN  //charc?????
TreeNode* factor(void) {
	if (NowToken->Lex == INTC) {
		TreeNode* t = new TreeNode();
		InitTreeNode(t);
		t->nodekind = ExpK;
		t->kind.exp = ConstK;
		t->attr.ExpAttr.val = atoi(NowToken->Sem);//�ַ���ת��������
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
		return t;
	}
	else if (NowToken->Lex == ID) {
		return variable();
	}
	else if (NowToken->Lex == CHARC) {
		//�ַ�����
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
			printf("�﷨���󣺵�%d�У� ȱ�� �� -factor\n", NowToken->Lineshow);
			fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� �� -factor\n", NowToken->Lineshow);
			SynErrorNum++;
			exit(1);
		}
		NowToken = NowToken->next;
		return t;
	}
	else {
		printf("�﷨���󣺵�%d�У����Խ���һ�����ʽ���ӣ�factor���������������޷�ʶ���token�� -factor\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У����Խ���һ�����ʽ���ӣ�factor���������������޷�ʶ���token�� -factor\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;
		return NULL;
	}
}

//53�����������      
TreeNode* variable(void) {
	TreeNode* t = new TreeNode();
	InitTreeNode(t);
	t->nodekind = ExpK;
	t->kind.exp = VariK;//�����µı��ʽ�ڵ�t
	if (NowToken->Lex == ID) {//ID!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		t->name.push_back(NowToken->Sem);
		t->idnum++;
	}
	NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	variMore(t);
	return t;
}

//54�������ಿ�ִ������    
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
			printf("�﷨���󣺵�%d�У� ȱ�� ] -varimore\n", NowToken->Lineshow);
			fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� ] -varimore\n", NowToken->Lineshow);
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
		printf("�﷨���󣺵�%d�У� ȱ�� [���� . -varimore\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� [���� . -varimore\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;    SynLine = NowToken->Lineshow;
	}
}

//55��������ִ������  
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
		printf("�﷨���󣺵�%d�У� ȱ�� ID -fieldvar\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ�� ID -fieldvar\n", NowToken->Lineshow);
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
			printf("�﷨���󣺵�%d�У� ȱ��] -fieldvarMore\n", NowToken->Lineshow);
			fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��] -fieldvarMore\n", NowToken->Lineshow);
			exit(1);
			
		}

		NowToken = NowToken->next;SynLine = NowToken->Lineshow;
		break;
	default:
		printf("�﷨���󣺵�%d�У� ���ʽ��������ȷ -fieldvarMore\n", NowToken->Lineshow);
		fprintf(outfile, "�﷨���󣺵�%d�У� ȱ��] -fieldvarMore\n", NowToken->Lineshow);
		SynErrorNum++;
		exit(1);
		NowToken = NowToken->next;	SynLine = NowToken->Lineshow;
		break;
	}
}