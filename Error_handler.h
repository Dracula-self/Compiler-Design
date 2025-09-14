#define _CRT_SECURE_NO_WARNINGS
#pragma once
// ��������ö��
enum ErrorType {
	FILE_OPEN_FAILED,//�ļ���ʧ��
	FILE_READ_OVER,//����ļ���ȡ����
	LEX_ERROR_NONE = 0,          // �޴���
	LEX_ERROR_INVALID_CHAR,      // �޷�ʶ����ַ�
	LEX_ERROR_INVALID_ASSIGN,    // ��ֵ���� ":=" ƴд����
	LEX_ERROR_INVALID_CHAR_LITERAL, // ���ַ�������󣨷���ĸ�����֣�
	LEX_ERROR_UNEXPECTED_EOF,    // ������ļ�������
	LEX_ERROR_INVALID_RANGE,     // �����±귶Χ����
	LEX_ERROR_INVALID_COMMENT,   // ע��δ��ȷ�ر�
	LEX_ERROR_MAX,                // �����������ֵ�����ڱ߽��飩
	LEX_ERROR_INVALID_NUMBER//��ʶ�������ֿ�ͷ
};

// ��������ʵ��
void ErrorProcess(ErrorType errorType, int line, int num, char additionalInfo);

