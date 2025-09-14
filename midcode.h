#pragma once
#include "total.h"

//�½�һ��ARG�ṹ
ARG* NewTemp(AccessKind now);
//�½���ֵ��ARG
ARG* ARGvalue(int now);
//����һ���µı��
int NewLabel();
//�½������ARG
ARG* ARGLabel(int now);
//�½���ַ��ARG
ARG* ARGAddr(const char* id, int level, int off, AccessKind acc);
//�м����Ĺ���
codeFile* GenCode(Codekind coki, ARG* arg1, ARG* arg2, ARG* arg3);
//��codekindת����string
string codekindToString(Codekind kind);
//��ӡARG
void printarg(ARG* now);
//��������м����
void printmidcode(codeFile* head,FILE*outfile);
//�м��������������
codeFile* GenMidCode(TreeNode* t);
//���ɹ�����ڡ����ڡ��������м����
void GenProcDec(TreeNode* now);
//������м�������ɺ���,ѭ����������м���봦��������ÿ�����
void GenBody(TreeNode* now);
//�������������͵�����Ӧ����䴦����
void GenStatement(TreeNode* now);
//��ֵ����м��������
void GenAssignS(TreeNode* a);
//����midcode����
ARG* GenVar(TreeNode* a);
//�������
ARG* GenArray(ARG* v1arg, TreeNode* t, int low, int size);
//���Ա����
ARG* GenField(ARG* v1arg, TreeNode* t, FieldChain* head);
//��lextype���������Ķ�Ӧ
Codekind midclass(LexType p);
//���ʽ(���������������ӱ��ʽ)
ARG* GenExpr(TreeNode* t);
//���̵��������м����(CALL��f������Result )
void GenCallS(TreeNode* t);
//������м��������read(v1)---(READ,v1)
void GenReadS(TreeNode* now);
void GenIfS(TreeNode* t);
//д����м����
void GenWriteS(TreeNode* t);
void GenWhileS(TreeNode* t);

extern FILE* mids;//�м���������
