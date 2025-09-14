#pragma once
#include <stack>
#include "total.h" // 假设包含ARG和codeFile等结构的定义

// 最大变量定值表大小
#define MAX_VARTABLE_SIZE 200

// 循环信息表，记录一层循环的信息
struct looptable {
    codeFile* entr;   // 循环入口
    codeFile* exitt;  // 循环出口
    int varpos;       // 本层变量定值表开始位置
    int flagg;        // 循环外提标识，1表示可以外提，0表示不可外提
};

// 主循环优化函数，处理中间代码，进行循环不变式外提
codeFile* LoopOpti(codeFile* firstcode,FILE*outfile);
void printvartable(FILE* outfile);

// 循环入口处理函数
void whileEntry(codeFile* nowcode);

// 循环出口处理函数
void whileEnd(codeFile* nowcode);

// 过程调用处理函数，含有此语句的循环不能进行不变式外提
void callcall(codeFile* nowcode);

// 循环外提处理函数
void loopoutside(codeFile* entry);

// 在变量定值表中查找变量，从指定位置开始
// 返回找到的位置索引，未找到返回-1
int searchvartable(ARG* now, int begin);

// 从变量定值表中删除一个变量
void delvartable(ARG* now, int begin);

// 向变量定值表中添加一个变量
void addvartable(ARG* now);

// 外部变量声明
extern ARG* vartable[MAX_VARTABLE_SIZE]; // 变量定值表
extern int POS;                          // 当前位置
extern int tablenum;                     // 变量定值表一共有多少项
extern std::stack<looptable*> stackforloop; // 循环信息栈
