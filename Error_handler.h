#define _CRT_SECURE_NO_WARNINGS
#pragma once
// 错误类型枚举
enum ErrorType {
	FILE_OPEN_FAILED,//文件打开失败
	FILE_READ_OVER,//标记文件读取结束
	LEX_ERROR_NONE = 0,          // 无错误
	LEX_ERROR_INVALID_CHAR,      // 无法识别的字符
	LEX_ERROR_INVALID_ASSIGN,    // 赋值符号 ":=" 拼写错误
	LEX_ERROR_INVALID_CHAR_LITERAL, // 单字符输入错误（非字母或数字）
	LEX_ERROR_UNEXPECTED_EOF,    // 意外的文件结束符
	LEX_ERROR_INVALID_RANGE,     // 数组下标范围错误
	LEX_ERROR_INVALID_COMMENT,   // 注释未正确关闭
	LEX_ERROR_MAX,                // 错误类型最大值（用于边界检查）
	LEX_ERROR_INVALID_NUMBER//标识符以数字开头
};

// 错误处理函数实现
void ErrorProcess(ErrorType errorType, int line, int num, char additionalInfo);

