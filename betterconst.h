#pragma once
#include "total.h"
//������ֵ�����ڳ������ʽ�Ż�
struct consttable {
	ARG* var;//��ʾ����
	int constvalue;
	struct consttable* pre;
	struct consttable* next;
};
//�洢�������ȫ������
extern codeFile* BLOCK[200];
//ȫ�ֳ�����ֵ��
extern consttable* constTable;
//�����黮�ֺ���
int DivBaseBlock(codeFile* firstCode);
//�������ʽ�Ż�������
codeFile* constoptimize(codeFile* firstcode);
//���ض�����������Ż�
void OptiBlock(int i);
//���������ͱȽϲ���
int ArithC(codeFile* code);
//�Բ�������ֵ�滻
void Substiarg(codeFile* code, int i);
//�ڳ�����ֵ���в��ұ���
int findconst(ARG* arg, consttable** entry);
//��ӱ������䳣��ֵ��������ֵ��
void addconsttable(ARG* arg, int res);
//�ӳ�����ֵ����ɾ������
void delconsttable(ARG* arg);
//�����ǰ������ֵ��
void printConsttable(int i);