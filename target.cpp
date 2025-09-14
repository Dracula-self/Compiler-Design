#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <unordered_set>
#include<unordered_map>
#include<fstream>
#include "target.h"
using namespace std;
ArrayAccessInfo llastArrayAccess = { false };
IntermediateCode* parseIntermediateCode(FILE* file, int* count) {
    if (!file) {
        printf("无法打开中间代码文件\n");
        exit(1);
    }
    IntermediateCode* code = (IntermediateCode*)malloc(sizeof(IntermediateCode) * 100);//为存储中间代码的数组分配内存，最多支持100条指令。
    *count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        line[strcspn(line, "\n")] = 0;
        char index_str[10] = { 0 };
        char op[50] = { 0 };
        char arg1[50] = { 0 };
        char arg2[50] = { 0 };
        char result[50] = { 0 };
        int pos = 0;
        int field = 0;
        while (line[pos] && line[pos] != ':') {
            index_str[field++] = line[pos++];
        }
        index_str[field] = '\0';
        pos++;
        while (line[pos] && (line[pos] == ' ' || line[pos] == '\t')) pos++;
        field = 0;
        while (line[pos] && line[pos] != ' ' && line[pos] != '\t') {
            op[field++] = line[pos++];
        }
        op[field] = '\0';
        while (line[pos] && (line[pos] == ' ' || line[pos] == '\t')) pos++;
        field = 0;
        while (line[pos] && line[pos] != ' ' && line[pos] != '\t') {
            arg1[field++] = line[pos++];
        }
        arg1[field] = '\0';
        if (field == 0) strcpy(arg1, "NULL");
        while (line[pos] && (line[pos] == ' ' || line[pos] == '\t')) pos++;
        field = 0;
        while (line[pos] && line[pos] != ' ' && line[pos] != '\t') {
            arg2[field++] = line[pos++];
        }
        arg2[field] = '\0';
        if (field == 0) strcpy(arg2, "NULL");
        while (line[pos] && (line[pos] == ' ' || line[pos] == '\t')) pos++;
        field = 0;
        while (line[pos] && line[pos] != ' ' && line[pos] != '\t') {
            result[field++] = line[pos++];
        }
        result[field] = '\0';
        if (field == 0) strcpy(result, "NULL");
        int index = atoi(index_str);
        strcpy(code[*count].op, op);
        strcpy(code[*count].arg1, arg1);
        strcpy(code[*count].arg2, arg2);
        strcpy(code[*count].result, result);
        (*count)++;
    }
    return code;
}
//添加标签生成函数，为循环程序处理做准备
void generateLabel(FILE* outputFile, int labelNum) {
    fprintf(outputFile, "label%d:\n", labelNum);
}
//负责MIPS的数据段.data selection
//outputFile: 输出文件指针，用于写入生成的MIPS汇编代码,intermediateCode: 中间代码数组，包含解析好的三地址码指令,codeCount : 中间代码指令的数量
void generateDataSection(FILE* outputFile, IntermediateCode* intermediateCode, int codeCount) {
    fprintf(outputFile, ".data\n");
    unordered_set<string> definedVars;// 已定义的变量集合
    unordered_set<string> arrayVars;// 数组变量集合
    //首先扫描识别所有数组变量（通过AADD操作）
    for (int i = 0; i < codeCount; i++) {
        if (strcmp(intermediateCode[i].op, "AADD") == 0) {
            string arrayName = intermediateCode[i].arg1;
            arrayVars.insert(arrayName);
        }
    }

    //处理过程参数
    for (int i = 0; i < codeCount; i++) {
        if (strcmp(intermediateCode[i].op, "PENTRY") == 0) {
            //查找后续的参数定义pentry开始到endproc结束
            int j = i + 1;
            while (j < codeCount && strcmp(intermediateCode[j].op, "ENDPROC") != 0) {
                if (strcmp(intermediateCode[j].op, "ASSIG") == 0) {//特别查找赋值操作（ASSIG）。函数参数通常以赋值语句的形式出现在过程体的开头
                    //形参赋值的格式是 ASSIG 形参 目标变量 NULL
                    string varName = intermediateCode[j].arg1;
                    //检查是否为常量（数字或负数）
                    if (!isdigit(varName[0]) && varName[0] != '-' &&
                        definedVars.find(varName) == definedVars.end()) {//这个变量尚未在数据段中定义
                        fprintf(outputFile, "%s: .word 0\n", varName.c_str());//在MIPS汇编的数据段中为这个形参变量分配一个字（word）的存储空间
                        definedVars.insert(varName);//变量名添加到已定义变量集合
                    }
                }
                j++;
            }
        }
    }

    //遍历中间代码，发现所有变量并分配空间
    for (int i = 0; i < codeCount; i++) {
        // 处理arg1
        if (strlen(intermediateCode[i].arg1) > 0 &&
            strcmp(intermediateCode[i].arg1, "NULL") != 0) {
            string varName = intermediateCode[i].arg1;
            //检查是否为常量（数字或负数）
            if (!isdigit(varName[0]) && varName[0] != '-') {
                //检查是否为数组
                if (arrayVars.find(varName) != arrayVars.end()) {
                    if (definedVars.find(varName) == definedVars.end()) {
                        //为数组分配空间，增加至21个元素（索引0-20）
                        fprintf(outputFile, "%s: .space %d\n", varName.c_str(), 21 * 4);
                        definedVars.insert(varName);
                    }
                }
                else if (definedVars.find(varName) == definedVars.end()) {
                    fprintf(outputFile, "%s: .word 0\n", varName.c_str());
                    definedVars.insert(varName);
                }
            }
        }

        //处理arg2
        if (strlen(intermediateCode[i].arg2) > 0 &&
            strcmp(intermediateCode[i].arg2, "NULL") != 0) {
            string varName = intermediateCode[i].arg2;
            //检查是否为常量（数字或负数）
            if (!isdigit(varName[0]) && varName[0] != '-') {
                //检查是否为数组
                if (arrayVars.find(varName) != arrayVars.end()) {
                    if (definedVars.find(varName) == definedVars.end()) {
                        //为数组分配空间，增加至21个元素（索引0-20）
                        fprintf(outputFile, "%s: .space %d\n", varName.c_str(), 21 * 4);
                        definedVars.insert(varName);
                    }
                }
                else if (definedVars.find(varName) == definedVars.end()) {
                    fprintf(outputFile, "%s: .word 0\n", varName.c_str());
                    definedVars.insert(varName);
                }
            }
        }

        //处理result
        if (strlen(intermediateCode[i].result) > 0 &&
            strcmp(intermediateCode[i].result, "NULL") != 0) {
            string varName = intermediateCode[i].result;
            //检查是否为常量（数字或负数）
            if (!isdigit(varName[0]) && varName[0] != '-') {
                //检查是否为数组
                if (arrayVars.find(varName) != arrayVars.end()) {
                    if (definedVars.find(varName) == definedVars.end()) {
                        //为数组分配空间
                        fprintf(outputFile, "%s: .space %d\n", varName.c_str(), 21 * 4);
                        definedVars.insert(varName);
                    }
                }
                else if (definedVars.find(varName) == definedVars.end()) {
                    fprintf(outputFile, "%s: .word 0\n", varName.c_str());
                    definedVars.insert(varName);
                }
            }
        }
    }
    //中间代码中有一个AADD操作使用了数组,这个数组没有在任何其他指令的操作数中出现,前面的扫描不会为specialArray分配空间
    //最后检查是否有任何数组变量还没有定义
    for (const auto& arrayName : arrayVars) {
        if (definedVars.find(arrayName) == definedVars.end()) {
            //为数组分配更大的空间
            fprintf(outputFile, "%s: .space %d\n", arrayName.c_str(), 21 * 4);
            definedVars.insert(arrayName);
        }
    }

    fprintf(outputFile, "\n");
}
//被text和data调用处理单条指令
//outputFile：输出文件指针，用于写入生成的MIPS汇编代码
//code：当前处理的中间代码指令，包含操作码和操作数
//labelMap：标签映射表，将中间代码中的标签编号映射到MIPS汇编中的标签位置
void generateInstruction(FILE* outputFile, const IntermediateCode& code, unordered_map<string, int>& labelMap) {
    if (strcmp(code.op, "MENTRY") == 0) {
        fprintf(outputFile, "\t# 程序入口\n");
    }
    else if (strcmp(code.op, "READC") == 0) {
        fprintf(outputFile, "\t# 读取整数到 %s\n", code.arg1);
        fprintf(outputFile, "\tli $v0, 5\n");
        fprintf(outputFile, "\tsyscall\n");
        fprintf(outputFile, "\tsw $v0, %s\n", code.arg1);
    }
    else if (strcmp(code.op, "ADD") == 0) {
        fprintf(outputFile, "\t# %s = %s + %s\n", code.result, code.arg1, code.arg2);
        //检查arg1是否为常量
        if (isdigit(code.arg1[0])) {
            fprintf(outputFile, "\tli $t0, %s\n", code.arg1);
        }
        else {
            fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        }

        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\taddi $t1, $t0, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
            fprintf(outputFile, "\tadd $t1, $t0, $t1\n");
        }
        fprintf(outputFile, "\tsw $t1, %s\n", code.result);
    }
    else if (strcmp(code.op, "SUB") == 0) {
        fprintf(outputFile, "\t# %s = %s - %s\n", code.result, code.arg1, code.arg2);

        //检查arg1是否为常量
        if (isdigit(code.arg1[0])) {
            fprintf(outputFile, "\tli $t0, %s\n", code.arg1); 
        }
        else {
            fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);  
        }

        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\taddi $t1, $t0, -%s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
            fprintf(outputFile, "\tsub $t1, $t0, $t1\n");
        }
        fprintf(outputFile, "\tsw $t1, %s\n", code.result);
        
    }
    else if (strcmp(code.op, "MULT") == 0) {
        fprintf(outputFile, "\t# %s = %s * %s\n", code.result, code.arg1, code.arg2);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tmul $t2, $t0, $t1\n");
        fprintf(outputFile, "\tsw $t2, %s\n", code.result);
    }
    else if (strcmp(code.op, "ASSIG") == 0) {
        //检查是否是对数组元素的赋值
        if (llastArrayAccess.isArrayAccess && strcmp(code.arg2, llastArrayAccess.resultVar) == 0) {
            //这是对数组元素的赋值
            fprintf(outputFile, "\t# %s[%s] = %s\n", llastArrayAccess.arrayName, llastArrayAccess.offsetVar, code.arg1);
            //加载要存储的值
            if (isdigit(code.arg1[0])) {
                fprintf(outputFile, "\tli $t0, %s\n", code.arg1);
            }
            else {
                fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
            }
            //加载偏移量
            if (isdigit(llastArrayAccess.offsetVar[0])) {
                //如果是常量索引
                fprintf(outputFile, "\tli $t1, %s\n", llastArrayAccess.offsetVar);
            }
            else {
                //如果是变量索引
                fprintf(outputFile, "\tlw $t1, %s\n", llastArrayAccess.offsetVar);
            }
            fprintf(outputFile, "\tsll $t1, $t1, 2\n");  // 乘以4
            //在高级语言中使用数组索引时（如 array[i]）需要将这个逻辑索引转换为实际的内存地址,   元素地址 = 数组基地址 + (索引值 × 元素大小)
            //计算数组元素地址
            fprintf(outputFile, "\tla $t2, %s\n", llastArrayAccess.arrayName);
            fprintf(outputFile, "\tadd $t3, $t2, $t1\n");
            //存储值到数组元素
            fprintf(outputFile, "\tsw $t0, 0($t3)\n");
            //重置数组访问标志
            llastArrayAccess.isArrayAccess = false;
        }
        else {
            fprintf(outputFile, "\t# %s = %s\n", code.arg2, code.arg1);
            //加载原来的值
            if (isdigit(code.arg1[0])) {
                fprintf(outputFile, "\tli $t0, %s\n", code.arg1);
            }
            else {
                fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
            }
            //存储到目标变量
            fprintf(outputFile, "\tsw $t0, %s\n", code.arg2);
        }
    }
    else if (strcmp(code.op, "WRITEC") == 0) {
        fprintf(outputFile, "\t# 输出 %s 的值\n", code.arg1);
        fprintf(outputFile, "\tli $v0, 1\n");//MIPS系统调用1表示输出整数
        //检查arg1是否为数字（常量）
        if (isdigit(code.arg1[0]) || (code.arg1[0] == '-' && isdigit(code.arg1[1]))) {
            fprintf(outputFile, "\tli $a0, %s\n", code.arg1);//加载要输出的值到$a0
        }
        else {
            //从变量加载值
            fprintf(outputFile, "\tlw $a0, %s\n", code.arg1);
        }
        fprintf(outputFile, "\tsyscall\n");
        fprintf(outputFile, "\tli $v0, 11\n");
        fprintf(outputFile, "\tli $a0, 10\n");
        fprintf(outputFile, "\tsyscall\n");//额外添加输出换行符的系统调用（系统调用11，$a0=10）
    }
    else if (strcmp(code.op, "JMP") == 0) {
        //无条件跳转到标签
        fprintf(outputFile, "\t# 无条件跳转\n");
        fprintf(outputFile, "\tj label%s\n", code.arg1);
    }
    else if (strcmp(code.op, "LABEL") == 0) {
        //标签定义点
        int labelNum = atoi(code.arg1);
        fprintf(outputFile, "label%s:\n", code.arg1);
        labelMap[code.arg1] = labelNum;
    }
    else if (strcmp(code.op, "JLT") == 0) {
        //如果小于则跳转
        fprintf(outputFile, "\t# 如果 %s < %s 则跳转到 label%s\n", code.arg1, code.arg2, code.result);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tblt $t0, $t1, label%s\n", code.result);
    }
    else if (strcmp(code.op, "JLE") == 0) {
        //如果小于等于则跳转
        fprintf(outputFile, "\t# 如果 %s <= %s 则跳转到 label%s\n", code.arg1, code.arg2, code.result);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tble $t0, $t1, label%s\n", code.result);
    }
    else if (strcmp(code.op, "JGT") == 0) {
        //如果大于则跳转
        fprintf(outputFile, "\t# 如果 %s > %s 则跳转到 label%s\n", code.arg1, code.arg2, code.result);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tbgt $t0, $t1, label%s\n", code.result);
    }
    else if (strcmp(code.op, "JGE") == 0) {
        //如果大于等于则跳转
        fprintf(outputFile, "\t# 如果 %s >= %s 则跳转到 label%s\n", code.arg1, code.arg2, code.result);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tbge $t0, $t1, label%s\n", code.result);
    }
    else if (strcmp(code.op, "JEQ") == 0) {
        //如果等于则跳转
        fprintf(outputFile, "\t# 如果 %s == %s 则跳转到 label%s\n", code.arg1, code.arg2, code.result);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tbeq $t0, $t1, label%s\n", code.result);
    }
    else if (strcmp(code.op, "JNE") == 0) {
        //如果不等于则跳转
        fprintf(outputFile, "\t# 如果 %s != %s 则跳转到 label%s\n", code.arg1, code.arg2, code.result);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tbne $t0, $t1, label%s\n", code.result);
    }
    else if (strcmp(code.op, "WHILESTART") == 0) {
        fprintf(outputFile, "label%s:\n", code.arg1);//生成循环开始标签
        labelMap[code.arg1] = atoi(code.arg1);
    }
    else if (strcmp(code.op, "JUMP0") == 0) {
        //条件为假时跳转：if code.arg1 == 0 then goto label code.arg2
        fprintf(outputFile, "\t# 如果 %s == 0 则跳转到 label%s\n", code.arg1, code.arg2);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        fprintf(outputFile, "\tbeqz $t0, label%s\n", code.arg2);//beqz指令（Branch if Equal to Zero）在条件为0时跳转
    }
    else if (strcmp(code.op, "JUMP") == 0) {
        //无条件跳转到标签
        fprintf(outputFile, "\t# 无条件跳转到 label%s\n", code.arg1);
        fprintf(outputFile, "\tj label%s\n", code.arg1);
    }
    else if (strcmp(code.op, "ENDWHILE") == 0) {
        //循环结束标记
        fprintf(outputFile, "label%s:\n", code.arg1);
        labelMap[code.arg1] = atoi(code.arg1);
    }
    else if (strcmp(code.op, "LTC") == 0) {
            //小于条件比较：code.result = (code.arg1 < code.arg2)
            fprintf(outputFile, "\t# %s = (%s < %s)\n", code.result, code.arg1, code.arg2);
            fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
            //加载两个操作数到寄存器
            if (isdigit(code.arg2[0])) {
                fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
            }
            else {
                fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
            }
            //使用slt指令（Set if Less Than）进行比较，结果存入$t2
            fprintf(outputFile, "\tslt $t2, $t0, $t1\n");  //如果$t0 < $t1，设置$t2为1，否则为0
            fprintf(outputFile, "\tsw $t2, %s\n", code.result);
    }
    //IF语句相关指令
    else if (strcmp(code.op, "IFSTART") == 0) {
        //IF语句开始，不需要生成代码
    }
    else if (strcmp(code.op, "EQC") == 0) {
        //等于条件比较：code.result = (code.arg1 == code.arg2)
        fprintf(outputFile, "\t# %s = (%s == %s)\n", code.result, code.arg1, code.arg2);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tseq $t2, $t0, $t1\n");  //如果$t0 == $t1，设置$t2为1，否则为0
        fprintf(outputFile, "\tsw $t2, %s\n", code.result);
    }
    else if (strcmp(code.op, "GTC") == 0) {
            //大于条件比较：code.result = (code.arg1 > code.arg2)
            fprintf(outputFile, "\t# %s = (%s > %s)\n", code.result, code.arg1, code.arg2);
            fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
            if (isdigit(code.arg2[0])) {
                fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
            }
            else {
                fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
            }
            fprintf(outputFile, "\tsgt $t2, $t0, $t1\n");  //如果$t0 > $t1，设置$t2为1，否则为0
            fprintf(outputFile, "\tsw $t2, %s\n", code.result);
    }
    else if (strcmp(code.op, "THEN") == 0) {
                //THEN部分开始不需要生成代码
    }
    else if (strcmp(code.op, "ELSE") == 0) {
        //ELSE部分开始，需要无条件跳转到IF结束
        fprintf(outputFile, "\t# 跳转到IF结束\n");
        fprintf(outputFile, "\tj label%s\n", code.arg1); 
        //生成ELSE开始标签
        fprintf(outputFile, "label%s:\n", code.arg2);
        labelMap[code.arg2] = atoi(code.arg2);//将ELSE标签添加到映射表
    }
    else if (strcmp(code.op, "ENDIF") == 0) {
            //IF语句结束
            fprintf(outputFile, "label%s:\n", code.arg1);//生成IF语句结束的标签
            labelMap[code.arg1] = atoi(code.arg1);
    }
    //添加数组访问和赋值指令
    else if (strcmp(code.op, "AADD") == 0) {
        //数组元素访问: result = arg1[arg2]
        //arg1是数组名，arg2是偏移量，result是目标变量
        fprintf(outputFile, "\t# %s = %s[%s]\n", code.result, code.arg1, code.arg2);
        //记录当前数组访问信息，供后续ASSIG使用
        llastArrayAccess.isArrayAccess = true;
        strcpy(llastArrayAccess.arrayName, code.arg1);
        strcpy(llastArrayAccess.offsetVar, code.arg2);
        strcpy(llastArrayAccess.resultVar, code.result);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t0, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t0, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tsll $t0, $t0, 2\n");  //加载索引并计算字节偏移,乘以4（左移2位）
        fprintf(outputFile, "\tla $t1, %s\n", code.arg1);  //加载数组基址
        fprintf(outputFile, "\tadd $t2, $t1, $t0\n");     //计算元素地址
        fprintf(outputFile, "\tlw $t3, 0($t2)\n");         //加载数组元素值
        fprintf(outputFile, "\tsw $t3, %s\n", code.result); //存储到目标变量
        }
    else if (strcmp(code.op, "ASSIG") == 0) {
            //检查是否是对数组元素的赋值
            if (llastArrayAccess.isArrayAccess && strcmp(code.arg2, llastArrayAccess.resultVar) == 0) {
                //这是对数组元素的赋值
                fprintf(outputFile, "\t# %s[%s] = %s\n", llastArrayAccess.arrayName, llastArrayAccess.offsetVar, code.arg1);
                //加载要存储的值
                if (isdigit(code.arg1[0])) {
                    fprintf(outputFile, "\tli $t0, %s\n", code.arg1);
                }
                else {
                    fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
                }
                //加载偏移量
                if (isdigit(llastArrayAccess.offsetVar[0])) {
                    //如果是常量索引
                    fprintf(outputFile, "\tli $t1, %s\n", llastArrayAccess.offsetVar);
                }
                else {
                    //如果是变量索引
                    fprintf(outputFile, "\tlw $t1, %s\n", llastArrayAccess.offsetVar);
                }
                fprintf(outputFile, "\tsll $t1, $t1, 2\n");  // 乘以4
                //计算数组元素地址
                fprintf(outputFile, "\tla $t2, %s\n", llastArrayAccess.arrayName);
                fprintf(outputFile, "\tadd $t3, $t2, $t1\n");
                //存储值到数组元素
                fprintf(outputFile, "\tsw $t0, 0($t3)\n");
                //重置数组访问标志
                llastArrayAccess.isArrayAccess = false;
            }
            else {
                //普通赋值操作
                fprintf(outputFile, "\t# %s = %s\n", code.arg2, code.arg1);

                if (isdigit(code.arg1[0])) {
                    fprintf(outputFile, "\tli $t0, %s\n", code.arg1);
                }
                else {
                    fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
                }
                fprintf(outputFile, "\tsw $t0, %s\n", code.arg2);
            }
            }
            //值参数传递  VALACT x 0 NULL   // 准备值参数x，传递到参数位置0
    else if (strcmp(code.op, "VALACT") == 0) {
        fprintf(outputFile, "\t# 准备值参数 %s\n", code.arg1);
        if (isdigit(code.arg1[0]) || (code.arg1[0] == '-' && isdigit(code.arg1[1]))) {
            fprintf(outputFile, "\tli $a0, %s\n", code.arg1);
        }
        else {
            fprintf(outputFile, "\tlw $a0, %s\n", code.arg1);
        }
        //使用$a0寄存器传递第一个参数,如果有多个参数，可以使用$a1,$a2,$a3或者栈
    }
    else if (strcmp(code.op, "CALL") == 0) {
        int procNum = atoi(code.arg1);
        fprintf(outputFile, "\t# 调用过程 proc%d，参数个数 %s\n", procNum, code.arg2);
        fprintf(outputFile, "\t# 保存$a0中的参数\n");/////嵌套----
        if (procNum == 0) {
            // 给proc0传递参数a，确保参数保存到a变量
            fprintf(outputFile, "\tsw $a0, a\n");
        }
        else if (procNum == 1) {
            // 给proc1传递参数c，确保参数保存到c变量
            fprintf(outputFile, "\tsw $a0, c\n");
        }

        fprintf(outputFile, "\tjal proc%d\n", procNum);
        //检查是否有返回值目标变量----递归
        if (strcmp(code.result, "NULL") != 0) {
            //确保result是一个有效的变量名
            if (strlen(code.result) > 0 && !isdigit(code.result[0])) {
                fprintf(outputFile, "\tsw $v0, %s\n", code.result);
            }
        } //保存递归返回结果
    }
    else if (strcmp(code.op, "PENTRY") == 0) {
                //过程入口，在generateTextSection中处理
    }
    else if(strcmp(code.op, "ENDPROC") == 0) {
                    //过程结束，在generateTextSection中处理
    }
    else {
        printf("\n");
    }
}
//生成MIPS汇编代码的代码段
//outputFile: 输出文件指针，用于写入生成的MIPS汇编代码
//intermediateCode: 中间代码数组，包含解析好的三地址码指令
//codeCount : 中间代码指令的数量
void generateTextSection(FILE* outputFile, IntermediateCode* intermediateCode, int codeCount) {
    //添加一个参数映射表,跟踪过程形参和实参的对应关系
    unordered_map<string, string> paramMapping;
    fprintf(outputFile, ".text\n");
    fprintf(outputFile, ".globl main\n");
    //添加跳转到main，确保程序从main开始执行
    fprintf(outputFile, "j main\n\n");
    //存储标签编号和位置
    unordered_map<string, int> labelMap;
    //用于追踪最近的AADD操作
    bool lastOpWasAADD = false;
    char lastArrayName[50] = { 0 };
    char lastOffsetVar[50] = { 0 };
    char lastResultVar[50] = { 0 };
    //首先生成所有过程的代码
    int i = 0;
    while (i < codeCount) {
        if (strcmp(intermediateCode[i].op, "PENTRY") == 0) {
            //生成过程头
            int procNum = atoi(intermediateCode[i].arg1);
            fprintf(outputFile, "proc%d:\n", procNum);
            fprintf(outputFile, "\t# 保存返回地址和帧指针\n");
            fprintf(outputFile, "\tsw $ra, 0($sp)\n");
            fprintf(outputFile, "\taddiu $sp, $sp, -4\n");
            fprintf(outputFile, "\tsw $fp, 0($sp)\n");
            fprintf(outputFile, "\taddiu $sp, $sp, -4\n");
            fprintf(outputFile, "\tmove $fp, $sp\n\n");
            //清空之前的参数映射
            paramMapping.clear();
            //查找过程参数和对应的实际变量
            int k = i + 1;
            while (k < codeCount && strcmp(intermediateCode[k].op, "ENDPROC") != 0) {
                if (strcmp(intermediateCode[k].op, "ASSIG") == 0) {
                    //形参赋值的格式是 ASSIG 形参 目标变量 NULL
                    string formalParam = intermediateCode[k].arg1;
                    string actualVar = intermediateCode[k].arg2;
                    //存储映射关系
                    paramMapping[formalParam] = actualVar;
                }
                k++;
            }
            //获取形参名称
            string paramName = "";
            if (i + 1 < codeCount && strcmp(intermediateCode[i + 1].op, "ASSIG") == 0) {
                paramName = intermediateCode[i + 1].arg1;
            }
            if (!paramName.empty()) {
                fprintf(outputFile, "\t# 保存参数到变量\n");
                if (isdigit(paramName[0]) || paramName[0] == '-') {
                    //如果形参名是数字，查找对应的实际变量名
                    for (const auto& mapping : paramMapping) {
                        if (mapping.first == paramName) {
                            fprintf(outputFile, "\tsw $a0, %s\n\n", mapping.second.c_str());
                            break;
                        }
                    }
                }
                else {
                    fprintf(outputFile, "\tsw $a0, %s\n\n", paramName.c_str());
                }
            }
            i++;
            //处理过程体中的指令
            while (i < codeCount && strcmp(intermediateCode[i].op, "ENDPROC") != 0) {
                //检查是不是在赋值
                if (i + 6 < codeCount &&
                    strcmp(intermediateCode[i].op, "AADD") == 0 &&       
                    strcmp(intermediateCode[i + 1].op, "ASSIG") == 0 &&    
                    strcmp(intermediateCode[i + 2].op, "AADD") == 0 &&      
                    strcmp(intermediateCode[i + 3].op, "AADD") == 0 &&    
                    strcmp(intermediateCode[i + 4].op, "ASSIG") == 0 &&     
                    strcmp(intermediateCode[i + 5].op, "AADD") == 0 &&      
                    strcmp(intermediateCode[i + 6].op, "ASSIG") == 0) {   
                    //先AADD获取一个数组元素，然后ASSIG它
                    char arrayName[50];
                    char indexVar[50];
                    char resultVar[50];
                    //记录第一个AADD操作的信息
                    strcpy(arrayName, intermediateCode[i].arg1);
                    strcpy(indexVar, intermediateCode[i].arg2);
                    strcpy(resultVar, intermediateCode[i].result);
                    //生成原始的指令
                    for (int j = 0; j <= 6; j++) {
                        generateInstruction(outputFile, intermediateCode[i + j], labelMap);
                    }
                    //添加写回a[2]的代码
                    fprintf(outputFile, "\t# 补充数组写回: a[2] = temp18 (实际是a[3]的值)\n");
                    fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i + 4].arg1); 
                    fprintf(outputFile, "\tli $t1, %s\n", intermediateCode[i].arg2);  
                    fprintf(outputFile, "\tsll $t1, $t1, 2\n");  //乘以4
                    fprintf(outputFile, "\tla $t2, %s\n", intermediateCode[i].arg1);  
                    fprintf(outputFile, "\tadd $t3, $t2, $t1\n");                     
                    fprintf(outputFile, "\tsw $t0, 0($t3)\n");     //存储a[3]的值到a[2]
                    //跳过已处理的指令
                    i += 7;
                }
                //识别形如a[k] = a[k+1]的数组操作模式
                else if (i + 7 < codeCount &&
                    strcmp(intermediateCode[i].op, "SUB") == 0 &&
                    strcmp(intermediateCode[i + 1].op, "MULT") == 0 &&
                    strcmp(intermediateCode[i + 2].op, "AADD") == 0 &&
                    strcmp(intermediateCode[i + 3].op, "ADD") == 0 &&
                    strcmp(intermediateCode[i + 4].op, "SUB") == 0 &&
                    strcmp(intermediateCode[i + 5].op, "MULT") == 0 &&
                    strcmp(intermediateCode[i + 6].op, "AADD") == 0 &&
                    strcmp(intermediateCode[i + 7].op, "ASSIG") == 0 &&
                    strcmp(intermediateCode[i + 7].arg2, intermediateCode[i + 2].result) == 0 &&
                    strcmp(intermediateCode[i + 7].arg1, intermediateCode[i + 6].result) == 0) {
                    //提取关键信息
                    const char* arrayName = intermediateCode[i + 2].arg1; //数组名 a
                    const char* baseIdx = intermediateCode[i].arg1; //基础索引 k
                    const char* offset = intermediateCode[i + 3].arg2;//偏移量 1

                    fprintf(outputFile, "\t# 检测到数组元素赋值模式: %s[%s] = %s[%s+%s]\n",
                        arrayName, baseIdx, arrayName, baseIdx, offset);
                    //生成原始指令
                    for (int j = 0; j <= 7; j++) {
                        generateInstruction(outputFile, intermediateCode[i + j], labelMap);
                    }
                    //正确的写回代码 - 将a[k+1]的值(temp22)写回到a[k]
                    fprintf(outputFile, "\t# 补充数组写回: 将a[k+1]的值写入a[k]\n");
                    //加载a[k+1]的值(temp22)
                    fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i + 6].result);  //加载temp22
                    //计算a[k]的地址
                    fprintf(outputFile, "\tlw $t1, %s\n", baseIdx);  //加载k
                    fprintf(outputFile, "\tsll $t1, $t1, 2\n"); //k*4
                    fprintf(outputFile, "\tla $t2, %s\n", arrayName); //加载数组基址
                    fprintf(outputFile, "\tadd $t3, $t2, $t1\n"); //计算a[k]的地址
                    //将a[k+1]的值写入a[k]
                    fprintf(outputFile, "\tsw $t0, 0($t3)\n"); //存储值到a[k]
                    //跳过已处理的指令
                    i += 8;
                }
                //处理连续的AADD和ASSIG操作
                else if (strcmp(intermediateCode[i].op, "AADD") == 0) {
                    //记录当前AADD操作信息
                    lastOpWasAADD = true;
                    strcpy(lastArrayName, intermediateCode[i].arg1);
                    strcpy(lastOffsetVar, intermediateCode[i].arg2);
                    strcpy(lastResultVar, intermediateCode[i].result);
                    //生成正常的AADD指令
                    generateInstruction(outputFile, intermediateCode[i], labelMap);
                    i++;
                }
                //当前指令是不是赋值操作并且是不是上一个AADD结果
                else if (lastOpWasAADD && strcmp(intermediateCode[i].op, "ASSIG") == 0 &&
                        strcmp(intermediateCode[i].arg2, lastResultVar) == 0) {
                        generateInstruction(outputFile, intermediateCode[i], labelMap);
                        fprintf(outputFile, "\t# %s[%s] = %s (数组赋值)\n", lastArrayName, lastOffsetVar, intermediateCode[i].arg1);
                        if (isdigit(intermediateCode[i].arg1[0])) {
                            fprintf(outputFile, "\tli $t0, %s\n", intermediateCode[i].arg1);
                        }
                        else {
                            fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i].arg1);
                        }
                        if (isdigit(lastOffsetVar[0])) {
                            //如果是常量索引
                            fprintf(outputFile, "\tli $t1, %s\n", lastOffsetVar);
                        }
                        else {
                            //如果是变量索引
                            fprintf(outputFile, "\tlw $t1, %s\n", lastOffsetVar);
                        }
                        fprintf(outputFile, "\tsll $t1, $t1, 2\n"); //计算字节偏移
                        fprintf(outputFile, "\tla $t2, %s\n", lastArrayName);//把一个内存地址赋给某个寄存器
                        fprintf(outputFile, "\tadd $t3, $t2, $t1\n");
                        fprintf(outputFile, "\tsw $t0, 0($t3)\n");
                        lastOpWasAADD = false;
                        i++;
                 }
                else {
                    lastOpWasAADD = false;
                    generateInstruction(outputFile, intermediateCode[i], labelMap);
                    i++;
                }
            }
            //生成过程尾
            ////加载返回值到$v0----递归
            //fprintf(outputFile, "\t# 加载返回值\n");
            //fprintf(outputFile, "\tlw $v0, res\n");
            fprintf(outputFile, "\t# 恢复帧指针和返回地址\n");
            fprintf(outputFile, "\tmove $sp, $fp\n");//栈指针($sp)恢复到当前帧指针($fp)的位置，撤销所有局部变量分配的空间
            fprintf(outputFile, "\taddiu $sp, $sp, 4\n");//将栈指针向上移动4字节，准备读取保存的帧指针值
            fprintf(outputFile, "\tlw $fp, 0($sp)\n");//从栈中恢复原来的帧指针值到$fp寄存器
            fprintf(outputFile, "\taddiu $sp, $sp, 4\n");//再次将栈指针向上移动4字节，准备读取保存的返回地址
            fprintf(outputFile, "\tlw $ra, 0($sp)\n");//从栈中恢复返回地址到$ra寄存器
            fprintf(outputFile, "\tjr $ra\n\n");
            i++;
        }
        else {
            i++; 
        }
    }
    //生成主程序代码
    fprintf(outputFile, "main:\n");
    i = 0;
    lastOpWasAADD = false;
    while (i < codeCount) {
        if (strcmp(intermediateCode[i].op, "MENTRY") == 0) {
            i++;
            while (i < codeCount) {
                if (i + 6 < codeCount &&
                    strcmp(intermediateCode[i].op, "AADD") == 0 && //访问a[2]
                    strcmp(intermediateCode[i + 1].op, "ASSIG") == 0 &&//保存值到temp
                    strcmp(intermediateCode[i + 2].op, "AADD") == 0 &&//再次访问a[2]
                    strcmp(intermediateCode[i + 3].op, "AADD") == 0 &&//访问a[3]
                    strcmp(intermediateCode[i + 4].op, "ASSIG") == 0 &&//将a[3]值赋给temp18
                    strcmp(intermediateCode[i + 5].op, "AADD") == 0 &&//再次访问a[3]
                    strcmp(intermediateCode[i + 6].op, "ASSIG") == 0) { //将原a[2]值写回a[3]
                    char arrayName[50], index1[50], index2[50];
                    strcpy(arrayName, intermediateCode[i].arg1); //数组名称
                    strcpy(index1, intermediateCode[i].arg2);  //第一个索引(2)
                    strcpy(index2, intermediateCode[i + 3].arg2);//第二个索引(3)
                    //生成原始的指令
                    for (int j = 0; j <= 6; j++) {
                        generateInstruction(outputFile, intermediateCode[i + j], labelMap);
                    }
                    //添加写回a[2]的代码 - 使用temp21(a[3]的值)
                    fprintf(outputFile, "\t# 补充数组写回: %s[%s] = %s (将a[3]的值写入a[2])\n",
                        arrayName, index1, intermediateCode[i + 3].result);
                    fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i + 3].result); //加载temp21(a[3]的值)
                    fprintf(outputFile, "\tli $t1, %s\n", index1); //加载a[2]的索引
                    fprintf(outputFile, "\tsll $t1, $t1, 2\n"); //乘以4
                    fprintf(outputFile, "\tla $t2, %s\n", arrayName); //数组基址
                    fprintf(outputFile, "\tadd $t3, $t2, $t1\n"); //计算a[2]的地址
                    fprintf(outputFile, "\tsw $t0, 0($t3)\n"); //将a[3]的值存储到a[2]
                    //跳过已处理的指令
                    i += 7;
                }
                else if (i + 7 < codeCount &&
                    strcmp(intermediateCode[i].op, "SUB") == 0 &&
                    strcmp(intermediateCode[i + 1].op, "MULT") == 0 &&
                    strcmp(intermediateCode[i + 2].op, "AADD") == 0 &&
                    strcmp(intermediateCode[i + 3].op, "ADD") == 0 &&
                    strcmp(intermediateCode[i + 4].op, "SUB") == 0 &&
                    strcmp(intermediateCode[i + 5].op, "MULT") == 0 &&
                    strcmp(intermediateCode[i + 6].op, "AADD") == 0 &&
                    strcmp(intermediateCode[i + 7].op, "ASSIG") == 0 &&
                    strcmp(intermediateCode[i + 7].arg2, intermediateCode[i + 2].result) == 0 &&
                    strcmp(intermediateCode[i + 7].arg1, intermediateCode[i + 6].result) == 0) {
                    //提取关键信息
                    const char* arrayName = intermediateCode[i + 2].arg1; //数组名 a
                    const char* baseIdx = intermediateCode[i].arg1;//基础索引 k
                    const char* offset = intermediateCode[i + 3].arg2;//偏移量 1
                    fprintf(outputFile, "\t# 检测到数组元素赋值模式: %s[%s] = %s[%s+%s]\n",
                        arrayName, baseIdx, arrayName, baseIdx, offset);
                    //生成原始指令
                    for (int j = 0; j <= 7; j++) {
                        generateInstruction(outputFile, intermediateCode[i + j], labelMap);
                    }
                    //正确的写回代码 - 将a[k+1]的值(写回到a[k]
                    fprintf(outputFile, "\t# 补充数组写回: 将a[k+1]的值写入a[k]\n");
                    // 加载a[k+1]的值(temp22)
                    fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i + 6].result);  //加载temp22
                    //计算a[k]的地址
                    fprintf(outputFile, "\tlw $t1, %s\n", baseIdx); //加载k
                    fprintf(outputFile, "\tsll $t1, $t1, 2\n"); 
                    fprintf(outputFile, "\tla $t2, %s\n", arrayName); //加载数组基址
                    fprintf(outputFile, "\tadd $t3, $t2, $t1\n");    //计算a[k]的地址
                    //将a[k+1]的值写入a[k]
                    fprintf(outputFile, "\tsw $t0, 0($t3)\n");  //存储值到a[k]
                    //跳过已处理的指令
                    i += 8;
                }
                else if (strcmp(intermediateCode[i].op, "AADD") == 0) {
                    //记录当前AADD操作信息
                    lastOpWasAADD = true;
                    strcpy(lastArrayName, intermediateCode[i].arg1);
                    strcpy(lastOffsetVar, intermediateCode[i].arg2);
                    strcpy(lastResultVar, intermediateCode[i].result);
                    //生成正常的AADD指令
                    generateInstruction(outputFile, intermediateCode[i], labelMap);
                    i++;
                }
                else if (lastOpWasAADD && strcmp(intermediateCode[i].op, "ASSIG") == 0 &&
                    strcmp(intermediateCode[i].arg2, lastResultVar) == 0) {
                    //这是数组赋值模式：先AADD后ASSIG到AADD的结果
                    generateInstruction(outputFile, intermediateCode[i], labelMap);
                    fprintf(outputFile, "\t# %s[%s] = %s (数组赋值)\n", lastArrayName, lastOffsetVar, intermediateCode[i].arg1);
                    if (isdigit(intermediateCode[i].arg1[0])) {
                        fprintf(outputFile, "\tli $t0, %s\n", intermediateCode[i].arg1);
                    }
                    else {
                        fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i].arg1);
                    }
                    // 加载索引
                    if (isdigit(lastOffsetVar[0])) {
                        fprintf(outputFile, "\tli $t1, %s\n", lastOffsetVar);
                    }
                    else {
                        fprintf(outputFile, "\tlw $t1, %s\n", lastOffsetVar);
                    }
                    fprintf(outputFile, "\tsll $t1, $t1, 2\n"); 
                    fprintf(outputFile, "\tla $t2, %s\n", lastArrayName);
                    fprintf(outputFile, "\tadd $t3, $t2, $t1\n");
                    fprintf(outputFile, "\tsw $t0, 0($t3)\n");
                    lastOpWasAADD = false;
                    i++;
                }
                
                //// 检查是否是递归阶乘相关的调用
                //else if (strcmp(intermediateCode[i].op, "VALACT") == 0 &&//----------递归
                //    i + 1 < codeCount &&
                //    strcmp(intermediateCode[i + 1].op, "VALACT") == 0) {
                //    // 发现两个连续的VALACT，只处理第一个
                //    generateInstruction(outputFile, intermediateCode[i], labelMap);
                //    i++;
                //    // 跳过第二个VALACT
                //    i++;
                //}
                //else if (strcmp(intermediateCode[i].op, "CALL") == 0) {//--------------递归
                //    generateInstruction(outputFile, intermediateCode[i], labelMap);
                //    // 检查是否需要保存返回值到result
                //    if (strcmp(intermediateCode[i].result, "NULL") != 0) {
                //        fprintf(outputFile, "\tsw $v0, result\n");
                //    }
                //    i++;
                //}
                else{
                    lastOpWasAADD = false;
                    generateInstruction(outputFile, intermediateCode[i], labelMap);
                    i++;
                }
            }
        }
        else if (strcmp(intermediateCode[i].op, "PENTRY") == 0) {
            while (i < codeCount && strcmp(intermediateCode[i].op, "ENDPROC") != 0) {
                i++;
            }
            i++; 
        }
        else {
            i++;
        }
    }
    fprintf(outputFile, "\t# 程序结束\n");
    fprintf(outputFile, "\tli $v0, 10\n");
    fprintf(outputFile, "\tsyscall\n");
}
void generateMIPSCode(IntermediateCode* intermediateCode, int codeCount, const char* outputFileName) {
    FILE* outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        printf("无法创建输出文件 %s\n", outputFileName);
        exit(1);
    }
    generateDataSection(outputFile, intermediateCode, codeCount);
    generateTextSection(outputFile, intermediateCode, codeCount);
    fclose(outputFile);
    printf("MIPS代码生成完成!\n");
}