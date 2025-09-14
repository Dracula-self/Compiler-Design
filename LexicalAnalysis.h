#pragma once
#include "total.h"
//一些全局变量
constexpr int ReserveNum = 43; //保留字数量
constexpr int maxlen = 20; // 一个字符的最大长度
extern int errnum;
//保留字表，便于查阅
struct word1
{
	char Sem[100];   //语义信息
	LexType tok; //词法信息
};	//保留字

//词法信息表,含有定义的所有词法类型
struct word2
{
	LexType tok; //词法信息
	char Sem[100]; //词法信息
};	//保留字


// 然后声明这两个数组（注意使用 extern）
extern word1 reservedWords[21];
extern word2 Words[42];


//以上为总.h,下面是ci.h
//类型声明
//定义自动机状态
enum LexState {
	START,//入口状态
	INID,//标志符状态
	INNUM,//数字状态
	DONE,//完成状态
	INASSIGN,//赋值状态
	INCOMMENT,//注释状态
	INRANGE,//数组下标界限状态
	INCHAR,//字符标志状态
	ERRORSTATE,
};
//ID的分类函数
LexType classifySem(char* ptr);
//打印输出词法信息
int PrintLex(LexType a);
//从源文件中读取一个字符
char getNextChar();
//将文件现有指针向前移动一个字符大小
int ungetNextChar();
//返回当前是什么类型的字符
int ClassifyChar(char ch);
//返回单词的词法类型
LexType charTOlex(char ch);
//链表结点初始化
int InitNode(TokenNode* ptr);

//同时输出到控制台和文件
int LexicalOutput(TokenNode* head, FILE* infile, FILE* outfile);
//释放链表
void FreeToken(TokenNode* head);
TokenNode* DFA();
void PrintToken(TokenNode* head);

