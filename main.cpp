#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <direct.h>
#include <fstream>
#include <cstddef>
#include "LexicalAnalysis.h"
#include "Error_handler.h"
#include "syntaxAnalysis.h"
#include "ll1.h"
#include "printTree.h"
#include "SemanticAnalysis.h"
#include "midcode.h"
#include "total.h"
#include "betterconst.h"
#include "target.h"
#include "betterloop.h"
#include "vs.h"
extern int errnum;
//FILE* outfile = fopen("test/ans.txt", "w");
//FILE* infile = fopen("test/1.txt", "w");
// 全局变量
FILE* infile = NULL;   // 代码文件读写指针
FILE* outfile = fopen("test/ans.txt", "w");  // token读写文件
char current_input_file[256] = "test/1.txt";  // 当前输入文件路径
char current_output_file[256] = "test/ans.txt";  // 当前输出文件路径

// 函数声明
bool reopen_files();
void close_files();
bool select_file_by_number();

// 预定义文件列表 - 可以手动更新这些列表
const char* lexical_files[] = {
    "非法字符和拼写错误.txt",
    "未闭合字符.txt",
    "单字符输入错误.txt",
    "标识符定义错误.txt"
};
const int lexical_files_count = sizeof(lexical_files) / sizeof(lexical_files[0]);

const char* syntax_files[] = {
    "while_endwhile.txt",
    "赋值号错误.txt",
    "括号不匹配.txt",
    "缺少程序头.txt",
    "数组声明相关错误.txt",
    "条件表达式缺少符号.txt",
};
const int syntax_files_count = sizeof(syntax_files) / sizeof(syntax_files[0]);

const char* semantic_files[] = {
    "标识符重复声明.txt",
    "赋值语句两端类型不匹配.txt",
    "没有声明的标识符.txt",
    "融合.txt",
    "数组下标上界小于下界.txt",
    "数组下标越界.txt"
};
const int semantic_files_count = sizeof(semantic_files) / sizeof(semantic_files[0]);

const char* target_files[] = {
    "IF语句测试.txt",
    "常量表达式优化测试.txt",
    "过程调用测试.txt",
    "基础.txt",
    "冒泡排序测试.txt",
    "嵌套+过程调用.txt",
    "数组测试.txt",
    "数组交换测试.txt",
    "循环.txt"
};
const int target_files_count = sizeof(target_files) / sizeof(target_files[0]);

const char* normal_files[] = {
    "1.txt",
    "test1.txt",
    "test2.txt",
    "loop.txt"
};
const int normal_files_count = sizeof(normal_files) / sizeof(normal_files[0]);

int main()
{
    int choice = 0;
    TokenNode* TokenListHead = NULL;
    TreeNode* root = NULL;
    codeFile* fircode = NULL;
    codeFile* opticode = NULL;
    IntermediateCode* intermediateCode = NULL;
    int codeCount = 0;

    // 初始化打开文件
    if (!reopen_files()) {
        printf("无法打开初始文件，程序退出\n");
        return 1;
    }

    printf("===================================================================\n");
    printf("                   SNL 编译器                                       \n");
    printf("===================================================================\n");

    while (true) {
        printf("\n请选择操作：\n");
        printf("1. 词法分析\n");
        printf("2. 语法分析\n");
        printf("3. 语义分析\n");
        printf("4. 中间代码生成\n");
        printf("5. 中间代码优化\n");
        printf("6. 目标代码生成\n");
        printf("7. 执行完整编译流程\n");
        printf("8. 选择测试文件 (当前: %s)\n", current_input_file);
        printf("0. 退出程序\n");
        printf("请输入选项 [0-8]: ");
        scanf("%d", &choice);
        if (choice == 0) {
            printf("程序已退出。\n");
            close_files();
            return 0;
        }

        switch (choice) {
        case 1: {
            printf("\n[1/6] 执行词法分析...\n");

            // 确保文件是重新打开的
            close_files();
            if (!reopen_files()) {
                printf("×无法打开输入文件！\n");
                continue;
            }

            TokenListHead = DFA();
            fprintf(outfile, "词法分析结果-------------------------------------------------------------------------\n");
            int error_message = LexicalOutput(TokenListHead, infile, outfile);
            if (!error_message) {
                printf("√词法分析完成！Token列表已保存到文件。\n");
            }
            else {
                printf("×词法分析出错！\n");
                FreeToken(TokenListHead);
                continue;
            }
            break;
        }
        case 2: {
            printf("\n[2/6] 执行语法分析...\n");
            if (TokenListHead == NULL) {
                printf("！请先执行词法分析！\n");
                continue;
            }

            // 重新打开输出文件用于追加
            if (outfile) fclose(outfile);
            //outfile = fopen(current_output_file, "a");
            outfile = fopen("test/ans.txt", "a");
            if (!outfile) {
                printf("×无法打开输出文件！\n");
                continue;
            }

            fprintf(outfile, "\n语法分析结果-------------------------------------------------------------------------\n");
            root = parse(TokenListHead);
            print_tree(root, 0, outfile);
            generateDOT(root, "test/tree.dot");
            printf("√语法分析完成！语法树已保存到文件。\n");
            break;
        }
        case 3: {
            printf("\n[3/6] 执行语义分析...\n");
            if (root == NULL) {
                printf("！请先执行语法分析！\n");
                continue;
            }

            // 重新打开输出文件用于追加
            if (outfile) fclose(outfile);
            outfile = fopen("test/ans.txt", "a");
            if (!outfile) {
                printf("×无法打开输出文件！\n");
                continue;
            }

            fprintf(outfile, "\n语义分析结果-------------------------------------------------------------------------\n");
            Analyze(root);
            PrintSymbTable(current_output_file);
            printf("√语义分析完成！符号表已保存到文件。\n");
            break;
        }
        case 4: {
            printf("\n[4/6] 生成中间代码...\n");
            if (root == NULL) {
                printf("！请先执行语法分析！\n");
                continue;
            }

            // 重新打开输出文件用于追加
            if (outfile) fclose(outfile);
            outfile = fopen("test/ans.txt", "a");
            if (!outfile) {
                printf("×无法打开输出文件！\n");
                continue;
            }

            fprintf(outfile, "\n中间代码-------------------------------------------------------------------------\n");
            fircode = GenMidCode(root);
            printmidcode(fircode, outfile);
            printf("√中间代码生成完成！已保存到文件。\n");
            break;
        }
        case 5: {
            printf("\n[5/6] 优化中间代码...\n");
            if (fircode == NULL) {
                printf("！请先生成中间代码！\n");
                continue;
            }

            // 重新打开输出文件用于追加
            if (outfile) fclose(outfile);
            outfile = fopen("test/ans.txt", "a");
            if (!outfile) {
                printf("×无法打开输出文件！\n");
                continue;
            }

            fprintf(outfile, "\n中间代码优化-常量表达式优化-------------------------------------------------------------------------\n");
            opticode = constoptimize(fircode);
            printmidcode(opticode, outfile);

            fprintf(outfile, "\n中间代码优化-循环不变式外提优化-------------------------------------------------------------------------\n");
            codeFile* loopopticode = LoopOpti(opticode, outfile);
            printmidcode(loopopticode, outfile);

            // 关闭当前文件并打开midcode.txt
            fclose(outfile);
            FILE* mids = fopen("test/midcode.txt", "w");
            if (mids) {
                printmidcode(loopopticode, mids);
                fclose(mids);
            }

            // 重新打开输出文件
           // outfile = fopen(current_output_file, "a");
            outfile = fopen("test/ans.txt", "a");


            printf("√中间代码优化完成！已保存到文件。\n");
            break;
        }
        case 6: {
            printf("\n[6/6] 生成目标代码...\n");
            if (opticode == NULL) {
                printf("！请先优化中间代码！\n");
                continue;
            }

            FILE* mids = fopen("test/midcode.txt", "r");
            if (!mids) {
                printf("×无法打开中间代码文件！\n");
                continue;
            }
            intermediateCode = parseIntermediateCode(mids, &codeCount);
            fclose(mids);
            generateMIPSCode(intermediateCode, codeCount, "test/mips.asm");
            free(intermediateCode);
            printf("√目标代码生成完成！MIPS汇编代码已保存到文件。\n");
            break;
        }
        case 7: {
            printf("\n开始执行完整编译流程...\n");

            // 确保文件是重新打开的
            close_files();
            if (!reopen_files()) {
                printf("×无法打开输入文件！\n");
                continue;
            }

            printf("\n[1/6] 执行词法分析...\n");
            TokenListHead = DFA();
            fprintf(outfile, "词法分析结果-------------------------------------------------------------------------\n");
            int error_message = LexicalOutput(TokenListHead, infile, outfile);
            if (!error_message) {
                printf("√词法分析完成！\n");
            }
            else {
                printf("×词法分析出错！\n");
                FreeToken(TokenListHead);
                continue;
            }

            // 关闭并重新打开输出文件用于追加
            fclose(outfile);
            outfile = fopen("test/ans.txt", "a");
            if (!outfile) {
                printf("×无法打开输出文件！\n");
                continue;
            }

            printf("\n[2/6] 执行语法分析...\n");
            fprintf(outfile, "\n语法分析结果-------------------------------------------------------------------------\n");
            root = parseLL1(TokenListHead);
            print_tree(root, 0, outfile);
            generateDOT(root, "test/tree.dot");
            printf("√语法分析完成！\n");

            printf("\n[3/6] 执行语义分析...\n");
            fprintf(outfile, "\n语义分析结果-------------------------------------------------------------------------\n");
            Analyze(root);
            PrintSymbTable(current_output_file);
            printf("√语义分析完成！\n");

            printf("\n[4/6] 生成中间代码...\n");
            fprintf(outfile, "\n中间代码-------------------------------------------------------------------------\n");
            fircode = GenMidCode(root);
            printmidcode(fircode, outfile);
            printf("√中间代码生成完成！\n");

            printf("\n[5/6] 优化中间代码...\n");
            fprintf(outfile, "\n中间代码优化-常量表达式优化-------------------------------------------------------------------------\n");
            opticode = constoptimize(fircode);
            printmidcode(opticode, outfile);

            fprintf(outfile, "\n中间代码优化-循环不变式外提优化-------------------------------------------------------------------------\n");
            codeFile* loopopticode = LoopOpti(opticode, outfile);
            printmidcode(loopopticode, outfile);

            // 关闭并打开midcode.txt
            fclose(outfile);
            FILE* mids = fopen("test/midcode.txt", "w");
            if (mids) {
                printmidcode(loopopticode, mids);
                fclose(mids);
            }

            // 重新打开输出文件
            outfile = fopen("test/ans.txt", "a");
            printf("√中间代码优化完成！\n");

            printf("\n[6/6] 生成目标代码...\n");
            mids = fopen("test/midcode.txt", "r");
            if (!mids) {
                printf("×无法打开中间代码文件！\n");
                continue;
            }
            intermediateCode = parseIntermediateCode(mids, &codeCount);
            fclose(mids);
            generateMIPSCode(intermediateCode, codeCount, "test/mips.asm");
            free(intermediateCode);
            printf("√目标代码生成完成！\n");

            printf("\n完整编译流程执行完毕！所有结果已保存到对应文件。\n");
            break;
        }
        case 8: {
            // 通过编号选择测试文件
            if (!select_file_by_number()) {
                printf("文件选择已取消，继续使用当前文件: %s\n", current_input_file);
            }
            break;
        }
        default:
            printf("无效选项，请重新输入！\n");
            break;
        }
    }

    return 0;
}

// 重新打开文件
bool reopen_files() {
    // 关闭现有文件
    //close_files();

    // 打开输入文件
    infile = fopen(current_input_file, "r");
    if (!infile) {
        printf("无法打开输入文件: %s\n", current_input_file);
        return false;
    }

    // 打开输出文件
    outfile = fopen("test/ans.txt", "a");
    if (!outfile) {
        printf("无法打开输出文件: %s\n", current_output_file);
        fclose(infile);
        infile = NULL;
        return false;
    }

    return true;
}

// 关闭文件
void close_files() {
    if (infile) {
        fclose(infile);
        infile = NULL;
    }

    if (outfile) {
        fclose(outfile);
        outfile = NULL;
    }
}

// 通过编号选择文件 (使用预定义列表)
bool select_file_by_number() {
    printf("\n请选择测试文件类别:\n");
    printf("1. 词法分析测试文件\n");
    printf("2. 语法分析测试文件\n");
    printf("3. 语义分析测试文件\n");
    printf("4. 目标代码测试文件\n");
    printf("5. 普通测试文件 (test目录)\n");
    printf("选择类别 [1-5]: ");

    int category;
    scanf("%d", &category);

    const char** files = NULL;
    int files_count = 0;
    char base_dir[100] = "test";

    switch (category) {
    case 1:
        files = lexical_files;
        files_count = lexical_files_count;
        strcpy(base_dir, "test\\词法分析");
        break;
    case 2:
        files = syntax_files;
        files_count = syntax_files_count;
        strcpy(base_dir, "test\\语法分析");
        break;
    case 3:
        files = semantic_files;
        files_count = semantic_files_count;
        strcpy(base_dir, "test\\语义分析");
        break;
    case 4:
        files = target_files;
        files_count = target_files_count;
        strcpy(base_dir, "test\\目标代码");
        break;
    case 5:
        files = normal_files;
        files_count = normal_files_count;
        strcpy(base_dir, "test");
        break;
    default:
        printf("无效选择，使用默认目录\n");
        return false;
    }

    if (files_count == 0) {
        printf("没有可用的测试文件\n");
        return false;
    }

    // 显示文件列表
    printf("\n%s 目录下的文件:\n", base_dir);
    for (int i = 0; i < files_count; i++) {
        printf("%d. %s\n", i + 1, files[i]);
    }

    // 用户选择文件
    printf("\n请选择文件 [1-%d] (0取消): ", files_count);
    int file_choice;
    scanf("%d", &file_choice);

    if (file_choice <= 0 || file_choice > files_count) {
        printf("无效选择或取消\n");
        return false;
    }

    // 备份当前文件路径
    char backup_path[256];
    strcpy(backup_path, current_input_file);

    // 更新文件路径
    sprintf(current_input_file, "%s\\%s", base_dir, files[file_choice - 1]);

    // 先关闭当前文件
    close_files();

    // 验证文件是否可以打开
    FILE* test = fopen(current_input_file, "r");
    if (test) {
        printf("已选择文件: %s\n", current_input_file);
        fclose(test);
        // 重新打开文件
        if (!reopen_files()) {
            printf("警告: 无法重新打开新选择的文件，恢复使用原文件\n");
            strcpy(current_input_file, backup_path);
            reopen_files();
            return false;
        }
        return true;
    }
    else {
        printf("错误: 文件不存在或无法访问: %s\n", current_input_file);
        printf("恢复使用原文件\n");
        strcpy(current_input_file, backup_path);
        reopen_files();
        return false;
    }
}




//int main()
//{
//    //词法分析&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//    TokenNode* TokenListHead = NULL; // token序列头
//    TokenListHead = DFA(); // 计算代码的tokenlist
//    fprintf(outfile, "词法分析结果-------------------------------------------------------------------------\n");
//    int error_message = LexicalOutput(TokenListHead,infile,outfile); // 将tokenlist输出到本地
//    if (!error_message)
//    {
//        printf("\n词法相关：输出token序列到本地成功！\n");
//        //PrintToken(TokenListHead);
//    }
//    else { FreeToken(TokenListHead);return 1; }
//    fclose(outfile);
//    //语法分析&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//    outfile = fopen("test/ans.txt", "a");  
//    fprintf(outfile, "\n语法分析结果-------------------------------------------------------------------------\n");
//    TreeNode* root = parse(TokenListHead);
//    //printTreeToFile(root, "test/yuyitree.txt");
//
//    //FILE* treefile = fopen("test/yuyitree.txt", "w");
//    if (outfile == NULL) {
//        perror("无法打开文件");
//        return 1;
//    }
//    print_tree(root, 0, outfile);
//    fclose(outfile);
//    //语义分析&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//    outfile = fopen("test/ans.txt", "a");
//    fprintf(outfile, "\n语义分析结果-------------------------------------------------------------------------\n");
//    Analyze(root);
//    fclose(outfile);
//    outfile = fopen("test/ans.txt", "a");
//    PrintSymbTable("test/ans.txt");
//    fclose(outfile);
//    //语法树可视化
//    generateDOT(root,"test/tree.dot");
//    //中间代码生成&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//    outfile = fopen("test/ans.txt", "a");
//    fprintf(outfile, "\n中间代码-------------------------------------------------------------------------\n");
//    codeFile* fircode = GenMidCode(root);
//    printmidcode(fircode,outfile);
//    //中间代码优化&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//    fprintf(outfile, "\n中间代码优化-常量表达式优化-------------------------------------------------------------------------\n");
//    codeFile* opticode = constoptimize(fircode);
//    printmidcode(opticode, outfile);
//    fprintf(outfile, "\n中间代码优化-循环不变式外提优化-------------------------------------------------------------------------\n");
//    codeFile* loopticode = LoopOpti(opticode,outfile);
//    printmidcode(loopticode, outfile);
//    fclose(outfile);
//    FILE* mids = fopen("test/midcode.txt", "w");
//    printmidcode(loopticode,mids);//再打印一遍到单独的文件，便于目标代码生成时候直接从文件读取
//    fclose(mids);
//
//    
//    //目标代码生成&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//    //从midcode里面读中间代码，目标代码在MIPS里面
//    mids = fopen("test/midcode.txt", "r"); // 打开中间代码文件
//    if (!mids) {
//        printf("无法打开中间代码文件\n");
//        return 1;
//    }
//    int codeCount;
//    IntermediateCode* intermediateCode = parseIntermediateCode(mids, &codeCount);
//    fclose(mids); //关闭文件
//    generateMIPSCode(intermediateCode, codeCount, "test/mips.asm");
//    free(intermediateCode);
//
//
//    return 0;
//}
