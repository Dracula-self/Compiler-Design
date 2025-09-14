#include<algorithm>
#include "Error_handler.h"
#include "total.h"
using namespace std;
void ErrorProcess(ErrorType errorType, int line, int num, char additionalInfo) {
    // ���ݴ����������������Ϣ
    switch (errorType) {
    case ErrorType::FILE_OPEN_FAILED:
        printf("�ʷ����� :�ļ���ʧ��.\n");
        fprintf(outfile,"�ʷ����� :�ļ���ʧ��.\n");
        break;
    case ErrorType::LEX_ERROR_INVALID_CHAR:// �޷�ʶ����ַ�
        printf("�ʷ����� %d: �� %d ----�޷�ʶ����ַ�: %c\n", num, line, additionalInfo);
        fprintf(outfile,"�ʷ����� %d: �� %d ----�޷�ʶ����ַ�: %c\n", num, line, additionalInfo);
        break;
    case ErrorType::LEX_ERROR_INVALID_ASSIGN:// ��ֵ���� ":=" ƴд����
        printf("�ʷ����� %d: �� %d ----��ֵ����ƴд����, ӦΪ ':='.\n", num, line);
        fprintf(outfile,"�ʷ����� %d: �� %d ----��ֵ����ƴд����, ӦΪ ':='.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL:// ���ַ�������󣨷���ĸ�����֣�
        printf("�ʷ����� %d: �� %d ----���ַ��������, ӦΪ������ĸ������.\n", num, line);
        fprintf(outfile,"�ʷ����� %d: �� %d ----���ַ��������, ӦΪ������ĸ������.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_UNEXPECTED_EOF:// ������ļ�������
        printf("�ʷ����� %d: �� %d ----������ļ�������.\n", num, line);
        fprintf(outfile,"�ʷ����� %d: �� %d ----������ļ�������.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_INVALID_RANGE:// �����±귶Χ����
        printf("�ʷ����� %d: �� %d ----�����±����ӦΪ '..'.\n", num, line);
        fprintf(outfile,"�ʷ����� %d: �� %d ----�����±����ӦΪ '..'.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_INVALID_COMMENT:// ע��δ��ȷ�ر�
        printf("�ʷ����� %d: �� %d ----ע��δ��ȷ�ر�.\n", num, line);
        fprintf(outfile,"�ʷ����� %d: �� %d ----ע��δ��ȷ�ر�.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_INVALID_NUMBER:
        printf("�ʷ����� %d: �� %d ----��ʶ�������ֿ�ͷ\n", num, line);
        fprintf(outfile,"�ʷ����� %d: �� %d ----��ʶ�������ֿ�ͷ\n", num, line);
        break;
    default://δ֪����
        printf("���� %d: �� %d ----δ֪����.\n", num, line);
        fprintf(outfile,"���� %d: �� %d ----δ֪����.\n", num, line);
        break;
    }
}