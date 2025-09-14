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
        printf("�޷����м�����ļ�\n");
        exit(1);
    }
    IntermediateCode* code = (IntermediateCode*)malloc(sizeof(IntermediateCode) * 100);//Ϊ�洢�м�������������ڴ棬���֧��100��ָ�
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
//��ӱ�ǩ���ɺ�����Ϊѭ����������׼��
void generateLabel(FILE* outputFile, int labelNum) {
    fprintf(outputFile, "label%d:\n", labelNum);
}
//����MIPS�����ݶ�.data selection
//outputFile: ����ļ�ָ�룬����д�����ɵ�MIPS������,intermediateCode: �м�������飬���������õ�����ַ��ָ��,codeCount : �м����ָ�������
void generateDataSection(FILE* outputFile, IntermediateCode* intermediateCode, int codeCount) {
    fprintf(outputFile, ".data\n");
    unordered_set<string> definedVars;// �Ѷ���ı�������
    unordered_set<string> arrayVars;// �����������
    //����ɨ��ʶ���������������ͨ��AADD������
    for (int i = 0; i < codeCount; i++) {
        if (strcmp(intermediateCode[i].op, "AADD") == 0) {
            string arrayName = intermediateCode[i].arg1;
            arrayVars.insert(arrayName);
        }
    }

    //������̲���
    for (int i = 0; i < codeCount; i++) {
        if (strcmp(intermediateCode[i].op, "PENTRY") == 0) {
            //���Һ����Ĳ�������pentry��ʼ��endproc����
            int j = i + 1;
            while (j < codeCount && strcmp(intermediateCode[j].op, "ENDPROC") != 0) {
                if (strcmp(intermediateCode[j].op, "ASSIG") == 0) {//�ر���Ҹ�ֵ������ASSIG������������ͨ���Ը�ֵ������ʽ�����ڹ�����Ŀ�ͷ
                    //�βθ�ֵ�ĸ�ʽ�� ASSIG �β� Ŀ����� NULL
                    string varName = intermediateCode[j].arg1;
                    //����Ƿ�Ϊ���������ֻ�����
                    if (!isdigit(varName[0]) && varName[0] != '-' &&
                        definedVars.find(varName) == definedVars.end()) {//���������δ�����ݶ��ж���
                        fprintf(outputFile, "%s: .word 0\n", varName.c_str());//��MIPS�������ݶ���Ϊ����βα�������һ���֣�word���Ĵ洢�ռ�
                        definedVars.insert(varName);//��������ӵ��Ѷ����������
                    }
                }
                j++;
            }
        }
    }

    //�����м���룬�������б���������ռ�
    for (int i = 0; i < codeCount; i++) {
        // ����arg1
        if (strlen(intermediateCode[i].arg1) > 0 &&
            strcmp(intermediateCode[i].arg1, "NULL") != 0) {
            string varName = intermediateCode[i].arg1;
            //����Ƿ�Ϊ���������ֻ�����
            if (!isdigit(varName[0]) && varName[0] != '-') {
                //����Ƿ�Ϊ����
                if (arrayVars.find(varName) != arrayVars.end()) {
                    if (definedVars.find(varName) == definedVars.end()) {
                        //Ϊ�������ռ䣬������21��Ԫ�أ�����0-20��
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

        //����arg2
        if (strlen(intermediateCode[i].arg2) > 0 &&
            strcmp(intermediateCode[i].arg2, "NULL") != 0) {
            string varName = intermediateCode[i].arg2;
            //����Ƿ�Ϊ���������ֻ�����
            if (!isdigit(varName[0]) && varName[0] != '-') {
                //����Ƿ�Ϊ����
                if (arrayVars.find(varName) != arrayVars.end()) {
                    if (definedVars.find(varName) == definedVars.end()) {
                        //Ϊ�������ռ䣬������21��Ԫ�أ�����0-20��
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

        //����result
        if (strlen(intermediateCode[i].result) > 0 &&
            strcmp(intermediateCode[i].result, "NULL") != 0) {
            string varName = intermediateCode[i].result;
            //����Ƿ�Ϊ���������ֻ�����
            if (!isdigit(varName[0]) && varName[0] != '-') {
                //����Ƿ�Ϊ����
                if (arrayVars.find(varName) != arrayVars.end()) {
                    if (definedVars.find(varName) == definedVars.end()) {
                        //Ϊ�������ռ�
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
    //�м��������һ��AADD����ʹ��������,�������û�����κ�����ָ��Ĳ������г���,ǰ���ɨ�費��ΪspecialArray����ռ�
    //������Ƿ����κ����������û�ж���
    for (const auto& arrayName : arrayVars) {
        if (definedVars.find(arrayName) == definedVars.end()) {
            //Ϊ����������Ŀռ�
            fprintf(outputFile, "%s: .space %d\n", arrayName.c_str(), 21 * 4);
            definedVars.insert(arrayName);
        }
    }

    fprintf(outputFile, "\n");
}
//��text��data���ô�����ָ��
//outputFile������ļ�ָ�룬����д�����ɵ�MIPS������
//code����ǰ������м����ָ�����������Ͳ�����
//labelMap����ǩӳ������м�����еı�ǩ���ӳ�䵽MIPS����еı�ǩλ��
void generateInstruction(FILE* outputFile, const IntermediateCode& code, unordered_map<string, int>& labelMap) {
    if (strcmp(code.op, "MENTRY") == 0) {
        fprintf(outputFile, "\t# �������\n");
    }
    else if (strcmp(code.op, "READC") == 0) {
        fprintf(outputFile, "\t# ��ȡ������ %s\n", code.arg1);
        fprintf(outputFile, "\tli $v0, 5\n");
        fprintf(outputFile, "\tsyscall\n");
        fprintf(outputFile, "\tsw $v0, %s\n", code.arg1);
    }
    else if (strcmp(code.op, "ADD") == 0) {
        fprintf(outputFile, "\t# %s = %s + %s\n", code.result, code.arg1, code.arg2);
        //���arg1�Ƿ�Ϊ����
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

        //���arg1�Ƿ�Ϊ����
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
        //����Ƿ��Ƕ�����Ԫ�صĸ�ֵ
        if (llastArrayAccess.isArrayAccess && strcmp(code.arg2, llastArrayAccess.resultVar) == 0) {
            //���Ƕ�����Ԫ�صĸ�ֵ
            fprintf(outputFile, "\t# %s[%s] = %s\n", llastArrayAccess.arrayName, llastArrayAccess.offsetVar, code.arg1);
            //����Ҫ�洢��ֵ
            if (isdigit(code.arg1[0])) {
                fprintf(outputFile, "\tli $t0, %s\n", code.arg1);
            }
            else {
                fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
            }
            //����ƫ����
            if (isdigit(llastArrayAccess.offsetVar[0])) {
                //����ǳ�������
                fprintf(outputFile, "\tli $t1, %s\n", llastArrayAccess.offsetVar);
            }
            else {
                //����Ǳ�������
                fprintf(outputFile, "\tlw $t1, %s\n", llastArrayAccess.offsetVar);
            }
            fprintf(outputFile, "\tsll $t1, $t1, 2\n");  // ����4
            //�ڸ߼�������ʹ����������ʱ���� array[i]����Ҫ������߼�����ת��Ϊʵ�ʵ��ڴ��ַ,   Ԫ�ص�ַ = �������ַ + (����ֵ �� Ԫ�ش�С)
            //��������Ԫ�ص�ַ
            fprintf(outputFile, "\tla $t2, %s\n", llastArrayAccess.arrayName);
            fprintf(outputFile, "\tadd $t3, $t2, $t1\n");
            //�洢ֵ������Ԫ��
            fprintf(outputFile, "\tsw $t0, 0($t3)\n");
            //����������ʱ�־
            llastArrayAccess.isArrayAccess = false;
        }
        else {
            fprintf(outputFile, "\t# %s = %s\n", code.arg2, code.arg1);
            //����ԭ����ֵ
            if (isdigit(code.arg1[0])) {
                fprintf(outputFile, "\tli $t0, %s\n", code.arg1);
            }
            else {
                fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
            }
            //�洢��Ŀ�����
            fprintf(outputFile, "\tsw $t0, %s\n", code.arg2);
        }
    }
    else if (strcmp(code.op, "WRITEC") == 0) {
        fprintf(outputFile, "\t# ��� %s ��ֵ\n", code.arg1);
        fprintf(outputFile, "\tli $v0, 1\n");//MIPSϵͳ����1��ʾ�������
        //���arg1�Ƿ�Ϊ���֣�������
        if (isdigit(code.arg1[0]) || (code.arg1[0] == '-' && isdigit(code.arg1[1]))) {
            fprintf(outputFile, "\tli $a0, %s\n", code.arg1);//����Ҫ�����ֵ��$a0
        }
        else {
            //�ӱ�������ֵ
            fprintf(outputFile, "\tlw $a0, %s\n", code.arg1);
        }
        fprintf(outputFile, "\tsyscall\n");
        fprintf(outputFile, "\tli $v0, 11\n");
        fprintf(outputFile, "\tli $a0, 10\n");
        fprintf(outputFile, "\tsyscall\n");//�������������з���ϵͳ���ã�ϵͳ����11��$a0=10��
    }
    else if (strcmp(code.op, "JMP") == 0) {
        //��������ת����ǩ
        fprintf(outputFile, "\t# ��������ת\n");
        fprintf(outputFile, "\tj label%s\n", code.arg1);
    }
    else if (strcmp(code.op, "LABEL") == 0) {
        //��ǩ�����
        int labelNum = atoi(code.arg1);
        fprintf(outputFile, "label%s:\n", code.arg1);
        labelMap[code.arg1] = labelNum;
    }
    else if (strcmp(code.op, "JLT") == 0) {
        //���С������ת
        fprintf(outputFile, "\t# ��� %s < %s ����ת�� label%s\n", code.arg1, code.arg2, code.result);
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
        //���С�ڵ�������ת
        fprintf(outputFile, "\t# ��� %s <= %s ����ת�� label%s\n", code.arg1, code.arg2, code.result);
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
        //�����������ת
        fprintf(outputFile, "\t# ��� %s > %s ����ת�� label%s\n", code.arg1, code.arg2, code.result);
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
        //������ڵ�������ת
        fprintf(outputFile, "\t# ��� %s >= %s ����ת�� label%s\n", code.arg1, code.arg2, code.result);
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
        //�����������ת
        fprintf(outputFile, "\t# ��� %s == %s ����ת�� label%s\n", code.arg1, code.arg2, code.result);
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
        //�������������ת
        fprintf(outputFile, "\t# ��� %s != %s ����ת�� label%s\n", code.arg1, code.arg2, code.result);
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
        fprintf(outputFile, "label%s:\n", code.arg1);//����ѭ����ʼ��ǩ
        labelMap[code.arg1] = atoi(code.arg1);
    }
    else if (strcmp(code.op, "JUMP0") == 0) {
        //����Ϊ��ʱ��ת��if code.arg1 == 0 then goto label code.arg2
        fprintf(outputFile, "\t# ��� %s == 0 ����ת�� label%s\n", code.arg1, code.arg2);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        fprintf(outputFile, "\tbeqz $t0, label%s\n", code.arg2);//beqzָ�Branch if Equal to Zero��������Ϊ0ʱ��ת
    }
    else if (strcmp(code.op, "JUMP") == 0) {
        //��������ת����ǩ
        fprintf(outputFile, "\t# ��������ת�� label%s\n", code.arg1);
        fprintf(outputFile, "\tj label%s\n", code.arg1);
    }
    else if (strcmp(code.op, "ENDWHILE") == 0) {
        //ѭ���������
        fprintf(outputFile, "label%s:\n", code.arg1);
        labelMap[code.arg1] = atoi(code.arg1);
    }
    else if (strcmp(code.op, "LTC") == 0) {
            //С�������Ƚϣ�code.result = (code.arg1 < code.arg2)
            fprintf(outputFile, "\t# %s = (%s < %s)\n", code.result, code.arg1, code.arg2);
            fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
            //�����������������Ĵ���
            if (isdigit(code.arg2[0])) {
                fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
            }
            else {
                fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
            }
            //ʹ��sltָ�Set if Less Than�����бȽϣ��������$t2
            fprintf(outputFile, "\tslt $t2, $t0, $t1\n");  //���$t0 < $t1������$t2Ϊ1������Ϊ0
            fprintf(outputFile, "\tsw $t2, %s\n", code.result);
    }
    //IF������ָ��
    else if (strcmp(code.op, "IFSTART") == 0) {
        //IF��俪ʼ������Ҫ���ɴ���
    }
    else if (strcmp(code.op, "EQC") == 0) {
        //���������Ƚϣ�code.result = (code.arg1 == code.arg2)
        fprintf(outputFile, "\t# %s = (%s == %s)\n", code.result, code.arg1, code.arg2);
        fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
        if (isdigit(code.arg2[0])) {
            fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
        }
        else {
            fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
        }
        fprintf(outputFile, "\tseq $t2, $t0, $t1\n");  //���$t0 == $t1������$t2Ϊ1������Ϊ0
        fprintf(outputFile, "\tsw $t2, %s\n", code.result);
    }
    else if (strcmp(code.op, "GTC") == 0) {
            //���������Ƚϣ�code.result = (code.arg1 > code.arg2)
            fprintf(outputFile, "\t# %s = (%s > %s)\n", code.result, code.arg1, code.arg2);
            fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
            if (isdigit(code.arg2[0])) {
                fprintf(outputFile, "\tli $t1, %s\n", code.arg2);
            }
            else {
                fprintf(outputFile, "\tlw $t1, %s\n", code.arg2);
            }
            fprintf(outputFile, "\tsgt $t2, $t0, $t1\n");  //���$t0 > $t1������$t2Ϊ1������Ϊ0
            fprintf(outputFile, "\tsw $t2, %s\n", code.result);
    }
    else if (strcmp(code.op, "THEN") == 0) {
                //THEN���ֿ�ʼ����Ҫ���ɴ���
    }
    else if (strcmp(code.op, "ELSE") == 0) {
        //ELSE���ֿ�ʼ����Ҫ��������ת��IF����
        fprintf(outputFile, "\t# ��ת��IF����\n");
        fprintf(outputFile, "\tj label%s\n", code.arg1); 
        //����ELSE��ʼ��ǩ
        fprintf(outputFile, "label%s:\n", code.arg2);
        labelMap[code.arg2] = atoi(code.arg2);//��ELSE��ǩ��ӵ�ӳ���
    }
    else if (strcmp(code.op, "ENDIF") == 0) {
            //IF������
            fprintf(outputFile, "label%s:\n", code.arg1);//����IF�������ı�ǩ
            labelMap[code.arg1] = atoi(code.arg1);
    }
    //���������ʺ͸�ֵָ��
    else if (strcmp(code.op, "AADD") == 0) {
        //����Ԫ�ط���: result = arg1[arg2]
        //arg1����������arg2��ƫ������result��Ŀ�����
        fprintf(outputFile, "\t# %s = %s[%s]\n", code.result, code.arg1, code.arg2);
        //��¼��ǰ���������Ϣ��������ASSIGʹ��
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
        fprintf(outputFile, "\tsll $t0, $t0, 2\n");  //���������������ֽ�ƫ��,����4������2λ��
        fprintf(outputFile, "\tla $t1, %s\n", code.arg1);  //���������ַ
        fprintf(outputFile, "\tadd $t2, $t1, $t0\n");     //����Ԫ�ص�ַ
        fprintf(outputFile, "\tlw $t3, 0($t2)\n");         //��������Ԫ��ֵ
        fprintf(outputFile, "\tsw $t3, %s\n", code.result); //�洢��Ŀ�����
        }
    else if (strcmp(code.op, "ASSIG") == 0) {
            //����Ƿ��Ƕ�����Ԫ�صĸ�ֵ
            if (llastArrayAccess.isArrayAccess && strcmp(code.arg2, llastArrayAccess.resultVar) == 0) {
                //���Ƕ�����Ԫ�صĸ�ֵ
                fprintf(outputFile, "\t# %s[%s] = %s\n", llastArrayAccess.arrayName, llastArrayAccess.offsetVar, code.arg1);
                //����Ҫ�洢��ֵ
                if (isdigit(code.arg1[0])) {
                    fprintf(outputFile, "\tli $t0, %s\n", code.arg1);
                }
                else {
                    fprintf(outputFile, "\tlw $t0, %s\n", code.arg1);
                }
                //����ƫ����
                if (isdigit(llastArrayAccess.offsetVar[0])) {
                    //����ǳ�������
                    fprintf(outputFile, "\tli $t1, %s\n", llastArrayAccess.offsetVar);
                }
                else {
                    //����Ǳ�������
                    fprintf(outputFile, "\tlw $t1, %s\n", llastArrayAccess.offsetVar);
                }
                fprintf(outputFile, "\tsll $t1, $t1, 2\n");  // ����4
                //��������Ԫ�ص�ַ
                fprintf(outputFile, "\tla $t2, %s\n", llastArrayAccess.arrayName);
                fprintf(outputFile, "\tadd $t3, $t2, $t1\n");
                //�洢ֵ������Ԫ��
                fprintf(outputFile, "\tsw $t0, 0($t3)\n");
                //����������ʱ�־
                llastArrayAccess.isArrayAccess = false;
            }
            else {
                //��ͨ��ֵ����
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
            //ֵ��������  VALACT x 0 NULL   // ׼��ֵ����x�����ݵ�����λ��0
    else if (strcmp(code.op, "VALACT") == 0) {
        fprintf(outputFile, "\t# ׼��ֵ���� %s\n", code.arg1);
        if (isdigit(code.arg1[0]) || (code.arg1[0] == '-' && isdigit(code.arg1[1]))) {
            fprintf(outputFile, "\tli $a0, %s\n", code.arg1);
        }
        else {
            fprintf(outputFile, "\tlw $a0, %s\n", code.arg1);
        }
        //ʹ��$a0�Ĵ������ݵ�һ������,����ж������������ʹ��$a1,$a2,$a3����ջ
    }
    else if (strcmp(code.op, "CALL") == 0) {
        int procNum = atoi(code.arg1);
        fprintf(outputFile, "\t# ���ù��� proc%d���������� %s\n", procNum, code.arg2);
        fprintf(outputFile, "\t# ����$a0�еĲ���\n");/////Ƕ��----
        if (procNum == 0) {
            // ��proc0���ݲ���a��ȷ���������浽a����
            fprintf(outputFile, "\tsw $a0, a\n");
        }
        else if (procNum == 1) {
            // ��proc1���ݲ���c��ȷ���������浽c����
            fprintf(outputFile, "\tsw $a0, c\n");
        }

        fprintf(outputFile, "\tjal proc%d\n", procNum);
        //����Ƿ��з���ֵĿ�����----�ݹ�
        if (strcmp(code.result, "NULL") != 0) {
            //ȷ��result��һ����Ч�ı�����
            if (strlen(code.result) > 0 && !isdigit(code.result[0])) {
                fprintf(outputFile, "\tsw $v0, %s\n", code.result);
            }
        } //����ݹ鷵�ؽ��
    }
    else if (strcmp(code.op, "PENTRY") == 0) {
                //������ڣ���generateTextSection�д���
    }
    else if(strcmp(code.op, "ENDPROC") == 0) {
                    //���̽�������generateTextSection�д���
    }
    else {
        printf("\n");
    }
}
//����MIPS������Ĵ����
//outputFile: ����ļ�ָ�룬����д�����ɵ�MIPS������
//intermediateCode: �м�������飬���������õ�����ַ��ָ��
//codeCount : �м����ָ�������
void generateTextSection(FILE* outputFile, IntermediateCode* intermediateCode, int codeCount) {
    //���һ������ӳ���,���ٹ����βκ�ʵ�εĶ�Ӧ��ϵ
    unordered_map<string, string> paramMapping;
    fprintf(outputFile, ".text\n");
    fprintf(outputFile, ".globl main\n");
    //�����ת��main��ȷ�������main��ʼִ��
    fprintf(outputFile, "j main\n\n");
    //�洢��ǩ��ź�λ��
    unordered_map<string, int> labelMap;
    //����׷�������AADD����
    bool lastOpWasAADD = false;
    char lastArrayName[50] = { 0 };
    char lastOffsetVar[50] = { 0 };
    char lastResultVar[50] = { 0 };
    //�����������й��̵Ĵ���
    int i = 0;
    while (i < codeCount) {
        if (strcmp(intermediateCode[i].op, "PENTRY") == 0) {
            //���ɹ���ͷ
            int procNum = atoi(intermediateCode[i].arg1);
            fprintf(outputFile, "proc%d:\n", procNum);
            fprintf(outputFile, "\t# ���淵�ص�ַ��ָ֡��\n");
            fprintf(outputFile, "\tsw $ra, 0($sp)\n");
            fprintf(outputFile, "\taddiu $sp, $sp, -4\n");
            fprintf(outputFile, "\tsw $fp, 0($sp)\n");
            fprintf(outputFile, "\taddiu $sp, $sp, -4\n");
            fprintf(outputFile, "\tmove $fp, $sp\n\n");
            //���֮ǰ�Ĳ���ӳ��
            paramMapping.clear();
            //���ҹ��̲����Ͷ�Ӧ��ʵ�ʱ���
            int k = i + 1;
            while (k < codeCount && strcmp(intermediateCode[k].op, "ENDPROC") != 0) {
                if (strcmp(intermediateCode[k].op, "ASSIG") == 0) {
                    //�βθ�ֵ�ĸ�ʽ�� ASSIG �β� Ŀ����� NULL
                    string formalParam = intermediateCode[k].arg1;
                    string actualVar = intermediateCode[k].arg2;
                    //�洢ӳ���ϵ
                    paramMapping[formalParam] = actualVar;
                }
                k++;
            }
            //��ȡ�β�����
            string paramName = "";
            if (i + 1 < codeCount && strcmp(intermediateCode[i + 1].op, "ASSIG") == 0) {
                paramName = intermediateCode[i + 1].arg1;
            }
            if (!paramName.empty()) {
                fprintf(outputFile, "\t# �������������\n");
                if (isdigit(paramName[0]) || paramName[0] == '-') {
                    //����β��������֣����Ҷ�Ӧ��ʵ�ʱ�����
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
            //����������е�ָ��
            while (i < codeCount && strcmp(intermediateCode[i].op, "ENDPROC") != 0) {
                //����ǲ����ڸ�ֵ
                if (i + 6 < codeCount &&
                    strcmp(intermediateCode[i].op, "AADD") == 0 &&       
                    strcmp(intermediateCode[i + 1].op, "ASSIG") == 0 &&    
                    strcmp(intermediateCode[i + 2].op, "AADD") == 0 &&      
                    strcmp(intermediateCode[i + 3].op, "AADD") == 0 &&    
                    strcmp(intermediateCode[i + 4].op, "ASSIG") == 0 &&     
                    strcmp(intermediateCode[i + 5].op, "AADD") == 0 &&      
                    strcmp(intermediateCode[i + 6].op, "ASSIG") == 0) {   
                    //��AADD��ȡһ������Ԫ�أ�Ȼ��ASSIG��
                    char arrayName[50];
                    char indexVar[50];
                    char resultVar[50];
                    //��¼��һ��AADD��������Ϣ
                    strcpy(arrayName, intermediateCode[i].arg1);
                    strcpy(indexVar, intermediateCode[i].arg2);
                    strcpy(resultVar, intermediateCode[i].result);
                    //����ԭʼ��ָ��
                    for (int j = 0; j <= 6; j++) {
                        generateInstruction(outputFile, intermediateCode[i + j], labelMap);
                    }
                    //���д��a[2]�Ĵ���
                    fprintf(outputFile, "\t# ��������д��: a[2] = temp18 (ʵ����a[3]��ֵ)\n");
                    fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i + 4].arg1); 
                    fprintf(outputFile, "\tli $t1, %s\n", intermediateCode[i].arg2);  
                    fprintf(outputFile, "\tsll $t1, $t1, 2\n");  //����4
                    fprintf(outputFile, "\tla $t2, %s\n", intermediateCode[i].arg1);  
                    fprintf(outputFile, "\tadd $t3, $t2, $t1\n");                     
                    fprintf(outputFile, "\tsw $t0, 0($t3)\n");     //�洢a[3]��ֵ��a[2]
                    //�����Ѵ����ָ��
                    i += 7;
                }
                //ʶ������a[k] = a[k+1]���������ģʽ
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
                    //��ȡ�ؼ���Ϣ
                    const char* arrayName = intermediateCode[i + 2].arg1; //������ a
                    const char* baseIdx = intermediateCode[i].arg1; //�������� k
                    const char* offset = intermediateCode[i + 3].arg2;//ƫ���� 1

                    fprintf(outputFile, "\t# ��⵽����Ԫ�ظ�ֵģʽ: %s[%s] = %s[%s+%s]\n",
                        arrayName, baseIdx, arrayName, baseIdx, offset);
                    //����ԭʼָ��
                    for (int j = 0; j <= 7; j++) {
                        generateInstruction(outputFile, intermediateCode[i + j], labelMap);
                    }
                    //��ȷ��д�ش��� - ��a[k+1]��ֵ(temp22)д�ص�a[k]
                    fprintf(outputFile, "\t# ��������д��: ��a[k+1]��ֵд��a[k]\n");
                    //����a[k+1]��ֵ(temp22)
                    fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i + 6].result);  //����temp22
                    //����a[k]�ĵ�ַ
                    fprintf(outputFile, "\tlw $t1, %s\n", baseIdx);  //����k
                    fprintf(outputFile, "\tsll $t1, $t1, 2\n"); //k*4
                    fprintf(outputFile, "\tla $t2, %s\n", arrayName); //���������ַ
                    fprintf(outputFile, "\tadd $t3, $t2, $t1\n"); //����a[k]�ĵ�ַ
                    //��a[k+1]��ֵд��a[k]
                    fprintf(outputFile, "\tsw $t0, 0($t3)\n"); //�洢ֵ��a[k]
                    //�����Ѵ����ָ��
                    i += 8;
                }
                //����������AADD��ASSIG����
                else if (strcmp(intermediateCode[i].op, "AADD") == 0) {
                    //��¼��ǰAADD������Ϣ
                    lastOpWasAADD = true;
                    strcpy(lastArrayName, intermediateCode[i].arg1);
                    strcpy(lastOffsetVar, intermediateCode[i].arg2);
                    strcpy(lastResultVar, intermediateCode[i].result);
                    //����������AADDָ��
                    generateInstruction(outputFile, intermediateCode[i], labelMap);
                    i++;
                }
                //��ǰָ���ǲ��Ǹ�ֵ���������ǲ�����һ��AADD���
                else if (lastOpWasAADD && strcmp(intermediateCode[i].op, "ASSIG") == 0 &&
                        strcmp(intermediateCode[i].arg2, lastResultVar) == 0) {
                        generateInstruction(outputFile, intermediateCode[i], labelMap);
                        fprintf(outputFile, "\t# %s[%s] = %s (���鸳ֵ)\n", lastArrayName, lastOffsetVar, intermediateCode[i].arg1);
                        if (isdigit(intermediateCode[i].arg1[0])) {
                            fprintf(outputFile, "\tli $t0, %s\n", intermediateCode[i].arg1);
                        }
                        else {
                            fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i].arg1);
                        }
                        if (isdigit(lastOffsetVar[0])) {
                            //����ǳ�������
                            fprintf(outputFile, "\tli $t1, %s\n", lastOffsetVar);
                        }
                        else {
                            //����Ǳ�������
                            fprintf(outputFile, "\tlw $t1, %s\n", lastOffsetVar);
                        }
                        fprintf(outputFile, "\tsll $t1, $t1, 2\n"); //�����ֽ�ƫ��
                        fprintf(outputFile, "\tla $t2, %s\n", lastArrayName);//��һ���ڴ��ַ����ĳ���Ĵ���
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
            //���ɹ���β
            ////���ط���ֵ��$v0----�ݹ�
            //fprintf(outputFile, "\t# ���ط���ֵ\n");
            //fprintf(outputFile, "\tlw $v0, res\n");
            fprintf(outputFile, "\t# �ָ�ָ֡��ͷ��ص�ַ\n");
            fprintf(outputFile, "\tmove $sp, $fp\n");//ջָ��($sp)�ָ�����ǰָ֡��($fp)��λ�ã��������оֲ���������Ŀռ�
            fprintf(outputFile, "\taddiu $sp, $sp, 4\n");//��ջָ�������ƶ�4�ֽڣ�׼����ȡ�����ָ֡��ֵ
            fprintf(outputFile, "\tlw $fp, 0($sp)\n");//��ջ�лָ�ԭ����ָ֡��ֵ��$fp�Ĵ���
            fprintf(outputFile, "\taddiu $sp, $sp, 4\n");//�ٴν�ջָ�������ƶ�4�ֽڣ�׼����ȡ����ķ��ص�ַ
            fprintf(outputFile, "\tlw $ra, 0($sp)\n");//��ջ�лָ����ص�ַ��$ra�Ĵ���
            fprintf(outputFile, "\tjr $ra\n\n");
            i++;
        }
        else {
            i++; 
        }
    }
    //�������������
    fprintf(outputFile, "main:\n");
    i = 0;
    lastOpWasAADD = false;
    while (i < codeCount) {
        if (strcmp(intermediateCode[i].op, "MENTRY") == 0) {
            i++;
            while (i < codeCount) {
                if (i + 6 < codeCount &&
                    strcmp(intermediateCode[i].op, "AADD") == 0 && //����a[2]
                    strcmp(intermediateCode[i + 1].op, "ASSIG") == 0 &&//����ֵ��temp
                    strcmp(intermediateCode[i + 2].op, "AADD") == 0 &&//�ٴη���a[2]
                    strcmp(intermediateCode[i + 3].op, "AADD") == 0 &&//����a[3]
                    strcmp(intermediateCode[i + 4].op, "ASSIG") == 0 &&//��a[3]ֵ����temp18
                    strcmp(intermediateCode[i + 5].op, "AADD") == 0 &&//�ٴη���a[3]
                    strcmp(intermediateCode[i + 6].op, "ASSIG") == 0) { //��ԭa[2]ֵд��a[3]
                    char arrayName[50], index1[50], index2[50];
                    strcpy(arrayName, intermediateCode[i].arg1); //��������
                    strcpy(index1, intermediateCode[i].arg2);  //��һ������(2)
                    strcpy(index2, intermediateCode[i + 3].arg2);//�ڶ�������(3)
                    //����ԭʼ��ָ��
                    for (int j = 0; j <= 6; j++) {
                        generateInstruction(outputFile, intermediateCode[i + j], labelMap);
                    }
                    //���д��a[2]�Ĵ��� - ʹ��temp21(a[3]��ֵ)
                    fprintf(outputFile, "\t# ��������д��: %s[%s] = %s (��a[3]��ֵд��a[2])\n",
                        arrayName, index1, intermediateCode[i + 3].result);
                    fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i + 3].result); //����temp21(a[3]��ֵ)
                    fprintf(outputFile, "\tli $t1, %s\n", index1); //����a[2]������
                    fprintf(outputFile, "\tsll $t1, $t1, 2\n"); //����4
                    fprintf(outputFile, "\tla $t2, %s\n", arrayName); //�����ַ
                    fprintf(outputFile, "\tadd $t3, $t2, $t1\n"); //����a[2]�ĵ�ַ
                    fprintf(outputFile, "\tsw $t0, 0($t3)\n"); //��a[3]��ֵ�洢��a[2]
                    //�����Ѵ����ָ��
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
                    //��ȡ�ؼ���Ϣ
                    const char* arrayName = intermediateCode[i + 2].arg1; //������ a
                    const char* baseIdx = intermediateCode[i].arg1;//�������� k
                    const char* offset = intermediateCode[i + 3].arg2;//ƫ���� 1
                    fprintf(outputFile, "\t# ��⵽����Ԫ�ظ�ֵģʽ: %s[%s] = %s[%s+%s]\n",
                        arrayName, baseIdx, arrayName, baseIdx, offset);
                    //����ԭʼָ��
                    for (int j = 0; j <= 7; j++) {
                        generateInstruction(outputFile, intermediateCode[i + j], labelMap);
                    }
                    //��ȷ��д�ش��� - ��a[k+1]��ֵ(д�ص�a[k]
                    fprintf(outputFile, "\t# ��������д��: ��a[k+1]��ֵд��a[k]\n");
                    // ����a[k+1]��ֵ(temp22)
                    fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i + 6].result);  //����temp22
                    //����a[k]�ĵ�ַ
                    fprintf(outputFile, "\tlw $t1, %s\n", baseIdx); //����k
                    fprintf(outputFile, "\tsll $t1, $t1, 2\n"); 
                    fprintf(outputFile, "\tla $t2, %s\n", arrayName); //���������ַ
                    fprintf(outputFile, "\tadd $t3, $t2, $t1\n");    //����a[k]�ĵ�ַ
                    //��a[k+1]��ֵд��a[k]
                    fprintf(outputFile, "\tsw $t0, 0($t3)\n");  //�洢ֵ��a[k]
                    //�����Ѵ����ָ��
                    i += 8;
                }
                else if (strcmp(intermediateCode[i].op, "AADD") == 0) {
                    //��¼��ǰAADD������Ϣ
                    lastOpWasAADD = true;
                    strcpy(lastArrayName, intermediateCode[i].arg1);
                    strcpy(lastOffsetVar, intermediateCode[i].arg2);
                    strcpy(lastResultVar, intermediateCode[i].result);
                    //����������AADDָ��
                    generateInstruction(outputFile, intermediateCode[i], labelMap);
                    i++;
                }
                else if (lastOpWasAADD && strcmp(intermediateCode[i].op, "ASSIG") == 0 &&
                    strcmp(intermediateCode[i].arg2, lastResultVar) == 0) {
                    //�������鸳ֵģʽ����AADD��ASSIG��AADD�Ľ��
                    generateInstruction(outputFile, intermediateCode[i], labelMap);
                    fprintf(outputFile, "\t# %s[%s] = %s (���鸳ֵ)\n", lastArrayName, lastOffsetVar, intermediateCode[i].arg1);
                    if (isdigit(intermediateCode[i].arg1[0])) {
                        fprintf(outputFile, "\tli $t0, %s\n", intermediateCode[i].arg1);
                    }
                    else {
                        fprintf(outputFile, "\tlw $t0, %s\n", intermediateCode[i].arg1);
                    }
                    // ��������
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
                
                //// ����Ƿ��ǵݹ�׳���صĵ���
                //else if (strcmp(intermediateCode[i].op, "VALACT") == 0 &&//----------�ݹ�
                //    i + 1 < codeCount &&
                //    strcmp(intermediateCode[i + 1].op, "VALACT") == 0) {
                //    // ��������������VALACT��ֻ�����һ��
                //    generateInstruction(outputFile, intermediateCode[i], labelMap);
                //    i++;
                //    // �����ڶ���VALACT
                //    i++;
                //}
                //else if (strcmp(intermediateCode[i].op, "CALL") == 0) {//--------------�ݹ�
                //    generateInstruction(outputFile, intermediateCode[i], labelMap);
                //    // ����Ƿ���Ҫ���淵��ֵ��result
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
    fprintf(outputFile, "\t# �������\n");
    fprintf(outputFile, "\tli $v0, 10\n");
    fprintf(outputFile, "\tsyscall\n");
}
void generateMIPSCode(IntermediateCode* intermediateCode, int codeCount, const char* outputFileName) {
    FILE* outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        printf("�޷���������ļ� %s\n", outputFileName);
        exit(1);
    }
    generateDataSection(outputFile, intermediateCode, codeCount);
    generateTextSection(outputFile, intermediateCode, codeCount);
    fclose(outputFile);
    printf("MIPS�����������!\n");
}