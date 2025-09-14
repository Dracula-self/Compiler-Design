#include<algorithm>
#include "Error_handler.h"
#include "total.h"
using namespace std;
void ErrorProcess(ErrorType errorType, int line, int num, char additionalInfo) {
    // 根据错误类型输出错误信息
    switch (errorType) {
    case ErrorType::FILE_OPEN_FAILED:
        printf("词法错误 :文件打开失败.\n");
        fprintf(outfile,"词法错误 :文件打开失败.\n");
        break;
    case ErrorType::LEX_ERROR_INVALID_CHAR:// 无法识别的字符
        printf("词法错误 %d: 行 %d ----无法识别的字符: %c\n", num, line, additionalInfo);
        fprintf(outfile,"词法错误 %d: 行 %d ----无法识别的字符: %c\n", num, line, additionalInfo);
        break;
    case ErrorType::LEX_ERROR_INVALID_ASSIGN:// 赋值符号 ":=" 拼写错误
        printf("词法错误 %d: 行 %d ----赋值符号拼写错误, 应为 ':='.\n", num, line);
        fprintf(outfile,"词法错误 %d: 行 %d ----赋值符号拼写错误, 应为 ':='.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL:// 单字符输入错误（非字母或数字）
        printf("词法错误 %d: 行 %d ----单字符输入错误, 应为单个字母或数字.\n", num, line);
        fprintf(outfile,"词法错误 %d: 行 %d ----单字符输入错误, 应为单个字母或数字.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_UNEXPECTED_EOF:// 意外的文件结束符
        printf("词法错误 %d: 行 %d ----意外的文件结束符.\n", num, line);
        fprintf(outfile,"词法错误 %d: 行 %d ----意外的文件结束符.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_INVALID_RANGE:// 数组下标范围错误
        printf("词法错误 %d: 行 %d ----数组下标错误，应为 '..'.\n", num, line);
        fprintf(outfile,"词法错误 %d: 行 %d ----数组下标错误，应为 '..'.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_INVALID_COMMENT:// 注释未正确关闭
        printf("词法错误 %d: 行 %d ----注释未正确关闭.\n", num, line);
        fprintf(outfile,"词法错误 %d: 行 %d ----注释未正确关闭.\n", num, line);
        break;
    case ErrorType::LEX_ERROR_INVALID_NUMBER:
        printf("词法错误 %d: 行 %d ----标识符以数字开头\n", num, line);
        fprintf(outfile,"词法错误 %d: 行 %d ----标识符以数字开头\n", num, line);
        break;
    default://未知错误
        printf("错误 %d: 行 %d ----未知错误.\n", num, line);
        fprintf(outfile,"错误 %d: 行 %d ----未知错误.\n", num, line);
        break;
    }
}