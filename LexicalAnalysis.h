#pragma once
#include "total.h"
//һЩȫ�ֱ���
constexpr int ReserveNum = 43; //����������
constexpr int maxlen = 20; // һ���ַ�����󳤶�
extern int errnum;
//�����ֱ����ڲ���
struct word1
{
	char Sem[100];   //������Ϣ
	LexType tok; //�ʷ���Ϣ
};	//������

//�ʷ���Ϣ��,���ж�������дʷ�����
struct word2
{
	LexType tok; //�ʷ���Ϣ
	char Sem[100]; //�ʷ���Ϣ
};	//������


// Ȼ���������������飨ע��ʹ�� extern��
extern word1 reservedWords[21];
extern word2 Words[42];


//����Ϊ��.h,������ci.h
//��������
//�����Զ���״̬
enum LexState {
	START,//���״̬
	INID,//��־��״̬
	INNUM,//����״̬
	DONE,//���״̬
	INASSIGN,//��ֵ״̬
	INCOMMENT,//ע��״̬
	INRANGE,//�����±����״̬
	INCHAR,//�ַ���־״̬
	ERRORSTATE,
};
//ID�ķ��ຯ��
LexType classifySem(char* ptr);
//��ӡ����ʷ���Ϣ
int PrintLex(LexType a);
//��Դ�ļ��ж�ȡһ���ַ�
char getNextChar();
//���ļ�����ָ����ǰ�ƶ�һ���ַ���С
int ungetNextChar();
//���ص�ǰ��ʲô���͵��ַ�
int ClassifyChar(char ch);
//���ص��ʵĴʷ�����
LexType charTOlex(char ch);
//�������ʼ��
int InitNode(TokenNode* ptr);

//ͬʱ���������̨���ļ�
int LexicalOutput(TokenNode* head, FILE* infile, FILE* outfile);
//�ͷ�����
void FreeToken(TokenNode* head);
TokenNode* DFA();
void PrintToken(TokenNode* head);

