#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include<iostream>
#include<cstring>
#include <cstdio>
#include "LexicalAnalysis.h"
#include "Error_handler.h"
using namespace std;
int errnum;

FILE* fp = fopen("E:/VScodes/25_complile/25_complile/test/1.txt", "r");// 代码文件读写指针
FILE* w_fp = fopen("E:/VScodes/25_complile/25_complile/test/token.txt", "w");// token读写文件

//全局变量
LexState statenow;
ErrorType ErrNow;//词法错误
int Line = 0;//token所在的行数

word1 reservedWords[21] = {
	{"program",PROGRAM},
	{"type",TYPE},
	{"var",VAR},
	{"procedure",PROCEDURE},
	{"begin",BEGIN},
	{"end",END},
	{"array",ARRAY},
	{"of",OF},
	{"record",RECORD},
	{"if",IF},
	{"then",THEN},
	{"else",ELSE},
	{"fi",FI},
	{"while",WHILE},
	{"do",DO},
	{"endwh",ENDWH},
	{"read",READ},
	{"write",WRITE},
	{"return",RETURN},
	{"integer",INTEGER},
	{"char",CHAR} };	//保留字

//ID分类函数
LexType classifySem(char* ptr) {
	for (int i = 0; i < 21; i++) {
		if (strcmp(reservedWords[i].Sem, ptr) == 0) {
			return reservedWords[i].tok;
		}
	}
	return ID;
}

word2 Words[42] = {
	{PROGRAM,"PROGRAM"},
	{TYPE,"TYPE"},
	{VAR,"VAR"},
	{ENDFILE,"ENDFILE"},
	{ERROR,"ERROR"},
	{ID,"ID"},
	{INTC,"INTC"},
	{CHARC,"CHARC"},
	{ASSIGN,"ASSIGN"},
	{PROCEDURE,"PROCEDURE"},
	{BEGIN,"BEGIN"},
	{END,"END"},
	{ARRAY,"ARRAY"},
	{EQ,"EQ"},
	{LT,"LT"},
	{PLUS,"PLUS"},
	{MINUS,"MINUS"},
	{TIMES,"TIMES"},
	{OVER,"OVER"},
	{OF,"OF"},
	{RECORD,"RECORD"},
	{IF,"IF"},
	{THEN,"THEN"},
	{ELSE,"ELSE"},
	{FI,"FI"},
	{LPAREN,"LPAREN"},
	{RPAREN,"RPAREN"},
	{DOT,"DOT"},
	{COLON,"COLON"},
	{SEMI,"SEMI"},
	{WHILE,"WHILE"},
	{DO,"DO"},
	{ENDWH,"ENDWH"},
	{READ,"READ"},
	{WRITE,"WRITE"},
	{COMMA,"COMMA"},
	{LMIDPAREN,"LMIDPAREN"},
	{RMIDPAREN,"RMIDPAREN"},
	{UNDERANGE,"UNDERANGE"},
	{RETURN,"RETURN"},
	{INTEGER,"INTEGER"},
	{CHAR,"CHAR"} };	//保留字

//打印输出词法信息
int PrintLex(LexType a) {
	for (int i = 0; i < 42; i++) {
		if (Words[i].tok == a) {
			//printf("%-15s\t", Words[i].Sem);
			fprintf(w_fp, "%-15s\t", Words[i].Sem);
			return 0;
		}
	}
}

char getNextChar() {
	char ch = fgetc(fp);
	return ch;
}
//回退
int ungetNextChar() {
	fseek(fp, -(long)sizeof(char), SEEK_CUR);
	return 0;
}


//返回当前是什么类型的字符
int ClassifyChar(char ch) {//
	if (isalpha(ch))
		return 1;//字母
	if (isdigit(ch))
		return 2;//数字
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=' || ch == '<' || ch == '>' || ch == ',')
		return 3;//单分界符
	if (ch == ':')
		return 4;//双分界符，接收为：
	if (ch == '{')
		return 5;//注释头符
	if (ch == '.')
		return 6;//数组下标
	if (ch == '\'')
		return 7;//字符状态
	//以下为测试部分
	if (ch == ' ' || ch == '\t')//字符为空白
		return 8;
	if (ch == '\n')
		return 9;
	if (ch == EOF)
		return 10;
	return 11;
}

//返回单词的词法类型
LexType ClassifyLex(char ch) {
	if (ch == '+') {
		return PLUS;
	}
	else if (ch == '-') {
		return MINUS;
	}
	else if (ch == '*') {
		return TIMES;
	}
	else if (ch == '/') {
		return OVER;
	}
	else if (ch == '(') {
		return LPAREN;
	}
	else if (ch == ')') {
		return RPAREN;
	}
	else if (ch == ';') {
		return SEMI;
	}
	else if (ch == '[') {
		return LMIDPAREN;
	}
	else if (ch == ']') {
		return RMIDPAREN;
	}
	else if (ch == '=') {
		return EQ;
	}
	else if (ch == '<') {
		return LT;
	}
	else if (ch == ',') {
		return COMMA;
	}
	else if (ch == EOF) {
		return ENDFILE;
	}

}

//链表结点初始化
int InitNode(TokenNode* ptr) {
	ptr->Lineshow = -1;
	ptr->Lex = ID;//初始化为标识符
	ptr->Sem[0] = '\0';
	ptr->next = ptr->pre = nullptr;
	return 0;
}

int LexicalOutput(TokenNode* head) {
	// 检查文件指针是否有效
	if (w_fp == NULL) {
		printf("\n词法错误：文件打开失败\n");
		return 0;
	}

	//// 检查词法分析是否有错误
	//if (ErrNow== ErrorType::FILE_READ_OVER) {
	//	printf("\n经检测，词法分析存在错误\n");
	//	fprintf(w_fp, "\n经检测，词法分析存在错误\n");
	//	fclose(w_fp);
	//	return 0;
	//}

	// 表头信息
	const char* header = "行数\t词法信息\t\t语义信息\t";
	printf("%s\n", header);
	fprintf(w_fp, "%s\n", header);

	// 遍历链表并输出每个Token的信息
	TokenNode* current = head;
	while (current != NULL) {
		// 输出行号
		printf("%d\t", current->Lineshow);
		fprintf(w_fp, "\n%d\t", current->Lineshow);

		// 输出词法信息（同时输出到控制台和文件）
		PrintLex(current->Lex);

		// 输出语义信息
		printf("%-15s\n", current->Sem);
		fprintf(w_fp, "%-15s", current->Sem);

		// 移动到下一个节点
		current = current->next;
	}

	printf("\n词法分析完成，结果已保存到文件\n");

	// 关闭文件
	fclose(w_fp);
	return 1;
}
void FreeToken(TokenNode* head)
{
	while (head->next != NULL) {
		head = head->next;
		free(head->pre);
	}
	free(head);
}
void PrintToken(TokenNode* head) {
	if (head == NULL) {
		printf("Empty list\n");
		return;
	}

	TokenNode* current = head;
	int nodeCount = 0;

	while (current != NULL) {
		printf("Token: %s\n", current->Sem);  // 使用%s打印字符串
		current = current->next;
		nodeCount++;
	}

	printf("Total tokens: %d\n", nodeCount);
}


//自动机部分_______-===========================================================

TokenNode* DFA() {
	// 使用rewind重置文件指针到文件开头
	if (fp != NULL) {
		rewind(fp);
	}
	else {
		// 如果文件指针为NULL，尝试打开文件
		fp = fopen("E:/VScodes/25_complile/25_complile/test/1.txt", "r");
		if (fp == NULL) {
			// 文件打开失败，调用错误处理函数
			ErrNow = ErrorType::FILE_OPEN_FAILED;
			errnum++;
			ErrorProcess(ErrNow, 0, errnum);
			return NULL;
		}
	}

	// 同样处理写入文件
	if (w_fp == NULL) {
		w_fp = fopen("E:/VScodes/25_complile/25_complile/test/token.txt", "w");
		if (w_fp == NULL) {
			// 处理写入文件打开失败
			printf("无法打开写入文件\n");
			return NULL;
		}
	}
	char prech;
	TokenNode* head = (TokenNode*)malloc(sizeof(TokenNode));
	TokenNode* nextt = (TokenNode*)malloc(sizeof(TokenNode));
	TokenNode* curr = head;
	InitNode(head);InitNode(nextt);//先初始化再分配内存
	nextt->next = (TokenNode*)malloc(sizeof(TokenNode));
	head->next = nextt;nextt->pre = head;

	int linenow = 0;//记录当前扫描到第几行
	int count = 0;//记录token的个数
	statenow = LexState::START;//设置初始状态
	ErrNow = ErrorType::LEX_ERROR_NONE;//记录当前错误类型
	char semnow[100];semnow[0] = '\0';//记录token的语义
	int semnum = -1;//sem下标
	LexType lexnow = ID;//token类型
	errnum = 0;//错误数量
	if (fp == nullptr) {
		//文件打开失败，调用错误处理函数
		ErrNow = ErrorType::FILE_OPEN_FAILED;
		errnum++;
		ErrorProcess(ErrNow, linenow, errnum);
		return NULL;
	}

	while (ErrNow == ErrorType::LEX_ERROR_NONE) {
		//只要没出错
		char ch = getNextChar();
		if (ch == '\n') {
			linenow++;
		}
		char ch2;
		switch (statenow) {
		case LexState::START:
			semnow[0] = '\0';semnum = -1;//清空记录
			//if (ch == '\n') { linenow++; }
			if (isalpha(ch)) {
				statenow = LexState::INID;//转入标识符状态
				semnow[++semnum] = ch;
			}
			else if (isdigit(ch)) {
				//转入数状态
				statenow = LexState::INNUM;
				semnow[++semnum] = ch;
			}
			else if (ch == '{') {
				//处理注释
				statenow = LexState::INCOMMENT;
				semnow[++semnum] = ch;
			}
			else if (ch == ':') {
				semnow[++semnum] = ch;
				//往后多读一个，看是不是=
				ch2 = getNextChar();
				if (ch2 == '=') {
					statenow = LexState::INASSIGN;
					semnow[++semnum] = ch;
				}
				else {
					//:的下一个不是=，报错
					ErrNow = ErrorType::LEX_ERROR_INVALID_ASSIGN;
					errnum++;
					ErrorProcess(ErrNow, linenow, errnum);//输出报错信息::的后面不是=*********************
					//?????????????????是否应该直接返回,如果直接返回就无法检测后面是否还有错误
					while (ch != EOF && ch != '=') { ch = getNextChar(); }
					if (ch == '=') {
						//那就可以跳过继续分析后面的
						statenow = LexState::INASSIGN;
						semnow[++semnum] = ch;
					}
					else {
						curr->Lex = ENDFILE;//当前token类型就是结束
						curr->Lineshow = linenow;
						strcpy(curr->Sem, "ENDFILE");
						curr->next = NULL;//????????????????????????????/
						fclose(fp);
						ErrNow = ErrorType::FILE_READ_OVER;
						printf("词法分析结束，共有%d个词法错误\n", errnum);
						return head;
					}

				}
			}
			else if (ch == '.') {
				//数组下标界限
				semnow[++semnum] = ch;
				ch2 = getNextChar();
				if (ch2 == '.') {//下一个也是.就是数组下表界限
					semnow[++semnum] = ch;
					statenow = LexState::INRANGE;
				}
				else {
					//否则就是程序结束标志,注意！！程序结束不等于文件分析完毕，不能return
					statenow = LexState::START;//开始下一段程序
					curr->Lex = DOT;
					curr->Lineshow = linenow;
					strcpy(curr->Sem, ".");
					curr = nextt;nextt = nextt->next;nextt->pre = curr;
					nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//申请空间
					statenow = LexState::START;
				}
			}
			else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=' || ch == '<' || ch == '>' || ch == ',') {
				if (ch == ';') {
					int i = 0;
				}//调试用
				semnow[++semnum] = ch;
				semnow[++semnum] = '\0';//***********************************
				statenow = LexState::DONE;
				prech = ch;
			}
			else if (ch == '\'') {
				//遇到单引号，字符串常量????????????????
				statenow = LexState::INCHAR;
			}
			else if (ch == ' ' || ch == '\t') {
				//空白或制表符，略过即可
				statenow = LexState::START;//回到初始
			}
			else if (ch == EOF) {
				//文件结束
				curr->Lex = ENDFILE;
				curr->Lineshow = linenow;
				curr->next = NULL;
				strcpy(curr->Sem, semnow);
				ErrNow = ErrorType::FILE_READ_OVER;
				printf("词法分析结束，共有%d个词法错误\n", errnum);
				fclose(fp);
				statenow = LexState::DONE;
				return head;
			}
			else {
				ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR;//当前错误：没有这种符号******************
				errnum++;
				ErrorProcess(ErrNow, linenow, errnum);
				statenow = LexState::START;
				ErrNow = ErrorType::LEX_ERROR_NONE;
			}
			break;

		case LexState::INID:
			while (isalnum(ch)) {
				//是字母或者数字
				semnow[++semnum] = ch;
				ch = getNextChar();
			}
			// 标志符结束
			semnow[++semnum] = '\0';
			curr->Lex = classifySem(semnow);  // 使用classifySem函数识别标识符或保留字
			curr->Lineshow = linenow;
			strcpy(curr->Sem, semnow);
			curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息

			statenow = LexState::START;  // 回归初始状态
			break;
		case INRANGE://数组下标状态

			curr->Lex = UNDERANGE; strcpy(curr->Sem, ".."); curr->Lineshow = linenow;
			curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
			statenow = LexState::START;  // 回归初始状态
			break;
		case LexState::INASSIGN:

			curr->Lex = ASSIGN;//
			curr->Lineshow = linenow;
			strcpy(curr->Sem, ":=");
			curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
			statenow = LexState::START;//回归初始状态
			break;
		case LexState::INNUM:

			while (isdigit(ch)) {
				ch = getNextChar();
				semnow[++semnum] = ch;
			}
			semnow[++semnum] = '\0';
			//遇到了非数字部分，表明数字token已经完结
			curr->Lex = INTC;//类型：数字
			curr->Lineshow = linenow;
			strcpy(curr->Sem, semnow);
			curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
			statenow = LexState::START;//回归初始状态
			break;
		case LexState::INCOMMENT:

			while (ch != '}') {
				ch = getNextChar();
			}
			statenow = LexState::START;//注释跳过即可
			break;
		case LexState::INCHAR://字符串常量

			ch2 = getNextChar();//因为遇到的时候没有再读
			//这里不能用while，因为单引号内部只能是单字符
			if (isalnum(ch2)) {
				statenow = LexState::DONE;
				semnow[++semnum] = ch2;
				semnow[++semnum] = '\0';
				curr->Lex = CHAR;
				curr->Lineshow = linenow;
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
			}
			else {
				statenow = START;
				errnum++;
				ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;
				ErrorProcess(ErrNow, linenow, errnum);//错误：字符常量出错
				//虽然这里出错，但是可以继续向下扫描,护士错误
				while (!isalnum(ch)) {
					ch = getNextChar();
				}
				statenow = LexState::DONE;
				curr->Lex = CHAR;
				curr->Lineshow = linenow;
				semnow[++semnum] = ch2;
				semnow[++semnum] = '\0';
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
			}
			break;

		case LexState::DONE:

			if (isalnum(prech)) {
				//ch = getNextChar();
				if (ch == '\'') {
					statenow = LexState::START;
					//字符结束了
				}
				else {
					errnum++;
					ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;//错误原因是单字符常量错误
					ErrorProcess(ErrNow, linenow, errnum);
					while (ch != EOF && ch != '\'') {
						ch = getNextChar();
					}
					if (ch == EOF) {
						curr->Lex = ENDFILE;
						curr->Lineshow = linenow;
						strcpy(curr->Sem, semnow);
						ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;//错误原因是单字符常量错误
						ErrorProcess(ErrNow, linenow, errnum);
						//errno++;
						ErrNow = ErrorType::FILE_READ_OVER;
						printf("词法分析结束，共有%d个词法错误\n", errnum);
						fclose(fp);
						return head;
					}
					else {
						//错误的常量结束了，开始下一阶段
						statenow = START;
						ErrNow = ErrorType::LEX_ERROR_NONE;
					}
				}
			}
			else {
				//当是单分界符号结束的DONE时
				semnow[++semnum] = '\0';
				curr->Lex = ClassifyLex(prech);//识别是什么类型的单分界符号
				curr->Lineshow = linenow;
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
				statenow = LexState::START;
			}
			break;
		default:
			//遇到未定义字符报错
			errnum++;
			ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR;//未定义字符的错误
			ErrorProcess(ErrNow, linenow, errnum, ch);
			statenow = LexState::START;
			break;
		}
	}
}
