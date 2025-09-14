#pragma once
#include <string>
#include<unordered_map>
#include<iostream>

//�м����ṹ�嶨��
typedef struct {
    char op[50];      // ������
    char arg1[50];    // ����1
    char arg2[50];    // ����2
    char result[50];  // ���
} IntermediateCode;
//���ڸ���������ʲ�����ȫ�ֱ���
typedef struct {
    bool isArrayAccess;// �Ƿ�Ϊ�������
    char arrayName[50];// ������
    char offsetVar[50];// ƫ������������
    char resultVar[50];// �������
} ArrayAccessInfo;
extern ArrayAccessInfo llastArrayAccess;
//���ļ������м����ĺ���
IntermediateCode* parseIntermediateCode(FILE* file, int* count);

//�������ݶ�
void generateDataSection(FILE* outputFile, IntermediateCode* intermediateCode, int codeCount);

//���ɴ����
void generateTextSection(FILE* outputFile, IntermediateCode* intermediateCode, int codeCount);

//���ݵ����м��������MIPSָ��

void generateInstruction(FILE* outputFile, const IntermediateCode& code, std::unordered_map<std::string, int>& labelMap);

//����MIPSĿ������������
void generateMIPSCode(IntermediateCode* intermediateCode, int codeCount, const char* outputFileName);