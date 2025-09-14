#pragma once
#include <stack>
#include "total.h" // �������ARG��codeFile�Ƚṹ�Ķ���

// ��������ֵ���С
#define MAX_VARTABLE_SIZE 200

// ѭ����Ϣ����¼һ��ѭ������Ϣ
struct looptable {
    codeFile* entr;   // ѭ�����
    codeFile* exitt;  // ѭ������
    int varpos;       // ���������ֵ��ʼλ��
    int flagg;        // ѭ�������ʶ��1��ʾ�������ᣬ0��ʾ��������
};

// ��ѭ���Ż������������м���룬����ѭ������ʽ����
codeFile* LoopOpti(codeFile* firstcode,FILE*outfile);
void printvartable(FILE* outfile);

// ѭ����ڴ�����
void whileEntry(codeFile* nowcode);

// ѭ�����ڴ�����
void whileEnd(codeFile* nowcode);

// ���̵��ô����������д�����ѭ�����ܽ��в���ʽ����
void callcall(codeFile* nowcode);

// ѭ�����ᴦ����
void loopoutside(codeFile* entry);

// �ڱ�����ֵ���в��ұ�������ָ��λ�ÿ�ʼ
// �����ҵ���λ��������δ�ҵ�����-1
int searchvartable(ARG* now, int begin);

// �ӱ�����ֵ����ɾ��һ������
void delvartable(ARG* now, int begin);

// �������ֵ�������һ������
void addvartable(ARG* now);

// �ⲿ��������
extern ARG* vartable[MAX_VARTABLE_SIZE]; // ������ֵ��
extern int POS;                          // ��ǰλ��
extern int tablenum;                     // ������ֵ��һ���ж�����
extern std::stack<looptable*> stackforloop; // ѭ����Ϣջ
