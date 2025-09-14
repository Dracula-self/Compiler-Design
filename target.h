#pragma once
#include <string>
#include<unordered_map>
#include<iostream>

//中间代码结构体定义
typedef struct {
    char op[50];      // 操作码
    char arg1[50];    // 参数1
    char arg2[50];    // 参数2
    char result[50];  // 结果
} IntermediateCode;
//用于跟踪数组访问操作的全局变量
typedef struct {
    bool isArrayAccess;// 是否为数组访问
    char arrayName[50];// 数组名
    char offsetVar[50];// 偏移量变量或常量
    char resultVar[50];// 结果变量
} ArrayAccessInfo;
extern ArrayAccessInfo llastArrayAccess;
//从文件解析中间代码的函数
IntermediateCode* parseIntermediateCode(FILE* file, int* count);

//生成数据段
void generateDataSection(FILE* outputFile, IntermediateCode* intermediateCode, int codeCount);

//生成代码段
void generateTextSection(FILE* outputFile, IntermediateCode* intermediateCode, int codeCount);

//根据单条中间代码生成MIPS指令

void generateInstruction(FILE* outputFile, const IntermediateCode& code, std::unordered_map<std::string, int>& labelMap);

//生成MIPS目标代码的主函数
void generateMIPSCode(IntermediateCode* intermediateCode, int codeCount, const char* outputFileName);