#pragma once
#include "total.h"
using namespace std;
//������
struct ParamTable {
	//int entry;//ָ���βα�ʶ���ڷ��ű��е�λ��
	SymbolTable* entry;
	struct ParamTable* next;
};

struct TypeIR {
	unsigned int serial;//�����Ա��е�λ�����
	int size;//������ռ�ռ��С
	TypeKind kind;
	union {
		struct {
			struct TypeIR* indexTy;
			struct TypeIR* elemTy;
			int low;//��¼�����Ͻ�
			int up;//��¼�����½�
		}ArrayAttr;
		FieldChain* body;
	}More;
};
struct AttributeIR {
	struct TypeIR* idtype;//ָ���ʶ���������ڲ���ʾ
	IdKind kind;//��ʶ��������
	union {
		struct {
			AccessKind access;
			int level;
			int off;//�������������������/���̵�ƫ����
		}VarAttr;//������ʶ��������
		struct {
			int level;//��ǰ���̵Ĳ���
			struct ParamTable* param;//������
			int code;//����Ŀ������ַ��Ŀ���������ʱ����д��
			int size;//��ǰ��������Ҫ�ռ��С
			int prAcReSi;//����ʱ���̻��¼��С
			int procEntry;//������ڵ�ַ
			int procEntryNum;//���̵���ڱ�������м��������
			int spOff;//SP��display���ƫ����
		}ProcAttr;//��������ʶ��������
	}More;//��ʶ���Ĳ�ͬ�����в�ͬ����
};
// ���ű�����
struct SymbolTable {
	char idname[IDLENGTH];//��ʶ��������
	AttributeIR attrIR;
	struct SymbolTable* next;
};

//���ű�������
void CreateTable(void);//1.����һ�����ű�
void DestroyTable(void);//2.����һ�����ű�
bool Enter(char* Id, AttributeIR* AttribP, SymbolTable** Entry, TreeNode* t);//3.�ǼǱ�ʶ�������Ե����ű�
bool FindEntry(char* id, bool flag, SymbolTable** Entry);//4.���ű��в��ұ�ʶ��
bool FindField(char* Id, FieldChain* head, FieldChain** Entry);//5.������в�������
void PrintSymbTable(const string& path);//6.��ӡ���ű�
bool SearchOneTable(char* id, int currentlevel, SymbolTable** Entry);//7.�ڵ������ű��в��ұ�ʶ��

// �����������������
void Analyze(TreeNode* currentP);//1.�������������
void initialize(void);//2.��ʼ�����������ڲ���ʾ����
TypeIR* TypeProcess(TreeNode* t, DecKind deckind);//3.���ͷ�������
TypeIR* nameType(TreeNode* t);//4.�Զ��������ڲ��ṹ��������
TypeIR* arrayType(TreeNode* t);//5.���������ڲ���ʾ������
TypeIR* recordType(TreeNode* t);//6.�����¼���͵��ڲ���ʾ����
void TypeDecPart(TreeNode* t);//7.�����������ַ���������
void VarDecList(TreeNode* t);//8.�����������ַ���������
void ProcDecPart(TreeNode* t);//9.�����������ַ���������
SymbolTable* HeadProcess(TreeNode* t);//10.��������ͷ��������
ParamTable* ParaDecList(TreeNode* t);//11.�βη���������
void Body(TreeNode* t);//12.ִ���岿�ַ���������
void statement(TreeNode* t);//13.������з���������
TypeIR* Expr(TreeNode* t, AccessKind* Ekind);//14.���ʽ����������
TypeIR* arrayVar(TreeNode* t);//15.��������ķ���������
TypeIR* recordVar(TreeNode* t);//16.��¼������������ķ���������
void assignstatement(TreeNode* t);//17.��ֵ����������
void callstatement(TreeNode* t);//18.���̵���������������
void ifstatement(TreeNode* t);//19.����������������
void whilestatement(TreeNode* t);//20.ѭ��������������
void readstatement(TreeNode* t);//21.��������������
void returnstatement(TreeNode* t);//22.����������������
void writestatement(TreeNode* t);//23.д������������
void LoopProgram(TreeNode* t);//�����﷨����������Ӧ�ĺ��������﷨�����