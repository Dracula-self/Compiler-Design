#define _CRT_SECURE_NO_WARNINGS
#include<fstream>
#include<iostream>
#include "LexicalAnalysis.h"
#include "Error_handler.h"
using namespace std;
int errnum;
//全局变量
LexState statenow;
ErrorType ErrNow;//词法错误
int Line=0;//token所在的行数
int tokennum = 0;
//ID分类函数
LexType classifySem(char* ptr) {
	for (int i = 0; i < 21; i++) {
		if (strcmp(reservedWords[i].Sem, ptr) == 0) {
			return reservedWords[i].tok;
		}
	}
	return ID;
}

//打印输出词法信息
int PrintLex(LexType a) {
	for (int i = 0; i < 42; i++) {
		if (Words[i].tok == a) {
			printf("%-15s\t", Words[i].Sem);
			fprintf(outfile, "%-15s\t", Words[i].Sem);
			return 0;
		}
	}
}

char getNextChar() {
	char ch = fgetc(infile);
	return ch;
}
//回退
int ungetNextChar() {
	fseek(infile, -(long)sizeof(char), SEEK_CUR);
	return 0;
}

//返回当前是什么类型的字符
int ClassifyChar(char ch) {//
	if (isalpha(ch))
		return 1;//字母
	if (isdigit(ch))
		return 2;//数字
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=' || ch == '<' ||ch=='>'|| ch == ',')
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
LexType charTOlex(char ch) {
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
	else if (ch == '>') {
		return GT;
	}
	else if (ch == ',') {
		return COMMA;
	}
	else if (ch == EOF) {
		return ENDFILE;
	}

}

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

//链表结点初始化
int InitNode(TokenNode* ptr) {
	ptr->Lineshow = -1;
	ptr->Lex = ID;//初始化为标识符
	ptr->Sem[0] = '\0';
	ptr->next = ptr->pre = nullptr;
	return 0;
}

int LexicalOutput(TokenNode* head,FILE*infile,FILE*outfile) {
	if (errnum) { return 1; }
	if (outfile == NULL) {
		printf("\n词法错误：文件打开失败\n");
		return 1;
	}
	const char* header = "序号\t行数\t词法信息\t\t语义信息\t";
	printf("%s\n", header);
	fprintf(outfile, "%s\n", header);
	TokenNode* current = head;
	int tokenCount = 1;  
	while (current != NULL && current->Lineshow != -1) {
		printf("%-4d\t", tokenCount);
		fprintf(outfile, "\n%-4d\t", tokenCount);
		printf("%d\t", current->Lineshow);
		fprintf(outfile, "%d\t", current->Lineshow);
		PrintLex(current->Lex);
		printf("%-15s\n", current->Sem);
		fprintf(outfile, "%-15s", current->Sem);
		current = current->next;
		tokenCount++;
	}
	printf("\n词法分析完成，共计%d个token，结果已保存到文件\n", tokenCount);
	fclose(outfile);
	return 0;
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
	int nodeCount = 1;

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
	if (infile != NULL) {
		rewind(infile);
	}
	else {
		// 如果文件指针为NULL，尝试打开文件
		infile = fopen("test/yuyitest.txt", "r");
		if (infile == NULL) {
			ErrNow = ErrorType::FILE_OPEN_FAILED;
			errnum++;
			ErrorProcess(ErrNow, 0, errnum,'\0');
			return NULL;
		}
	}
	if (outfile == NULL) {
		outfile = fopen("test/yuyitoken.txt", "w");
		if (outfile == NULL) {
			printf("无法打开写入文件\n");
			return NULL;
		}
	}
	char prech;
	TokenNode* head = (TokenNode*)malloc(sizeof(TokenNode));
	TokenNode* nextt = (TokenNode*)malloc(sizeof(TokenNode));
	TokenNode* curr = head;
	InitNode(head);InitNode(nextt);
	nextt->next = (TokenNode*)malloc(sizeof(TokenNode));
	InitNode(nextt->next);
	head->next = nextt;nextt->pre = head;

	int linenow = 1;//记录当前扫描到第几行
	int count = 0;//记录token的个数
	statenow = LexState::START;//设置初始状态
	ErrNow = ErrorType::LEX_ERROR_NONE;//记录当前错误类型
	char semnow[100];semnow[0] = '\0';//记录token的语义
	int semnum = -1;//sem下标
	LexType lexnow = ID;//token类型
	errnum = 0;//错误数量
	if (infile == nullptr) {
		//文件打开失败，调用错误处理函数
		ErrNow = ErrorType::FILE_OPEN_FAILED;
		errnum++;
		ErrorProcess(ErrNow,linenow,errnum,'\0');
		return NULL;
	}
	char ch = getNextChar();
	//while (ErrNow == ErrorType::LEX_ERROR_NONE) {
	while(1){
		//只要没出错
		char ch2;
		switch (statenow) {
		case LexState::START:
			semnow[0] = '\0';semnum = -1;//清空记录
			if (ch == '\n') { 
				linenow++;
				ch = getNextChar();
			}
			else if (isalpha(ch)) {
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
				ch = getNextChar();
				if (ch == '=') {
					statenow = LexState::INASSIGN;
					semnow[++semnum] = ch;
					break;
				}
				else {
					//:的下一个不是=，报错
					ErrNow = ErrorType::LEX_ERROR_INVALID_ASSIGN;
					errnum++;
					ErrorProcess(ErrNow, linenow,errnum,'\0');
					fclose(infile);
					return head;
				}
			}
			else if (ch == '.') {
				//数组下标界限
				semnow[++semnum] = ch;
				ch = getNextChar();
				if (ch == '.') {//下一个也是.就是数组下表界限
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
					InitNode(nextt->next);
					statenow = LexState::START;
				}
			}
			else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=' || ch == '<' || ch == '>' || ch == ',') {
				semnow[++semnum] = ch;
				statenow = LexState::DONE;
			}
			else if (ch == '\'') {
				statenow = LexState::INCHAR;
			}
			else if (ch == ' ' || ch == '\t') {
				//空白或制表符，略过即可
				ch = getNextChar();
				
			}
			else if (ch == EOF) {
				//文件结束
				curr->Lex = ENDFILE;
				curr->Lineshow = linenow;
				curr->next = NULL;
				strcpy(curr->Sem, semnow);
				ErrNow = ErrorType::FILE_READ_OVER;
				printf("词法分析结束，共有%d个词法错误\n", errnum);
				fclose(infile);
				statenow = LexState::DONE;
				return head;
			}
			else{
				ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR;//当前错误：没有这种符号******************
				errnum++;
				ErrorProcess(ErrNow,linenow,errnum,ch);
				ch=getNextChar();
				statenow = LexState::START;
				ErrNow = ErrorType::LEX_ERROR_NONE;
			}
			
			break;

		case LexState::INID:
			ch = getNextChar();
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
			InitNode(nextt->next);
			statenow = LexState::START;  // 回归初始状态
			break;
		case INRANGE://数组下标状态
			
			curr->Lex = UNDERANGE; strcpy(curr->Sem, ".."); curr->Lineshow = linenow;
			curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
			InitNode(nextt->next);
			ch = getNextChar();
			statenow = LexState::START;  // 回归初始状态
			break;
		case LexState::INASSIGN:
			curr->Lex = ASSIGN;//
			curr->Lineshow = linenow;
			strcpy(curr->Sem, ":=");
			curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
			InitNode(nextt->next);
			ch = getNextChar();
			statenow = LexState::START;//回归初始状态
			break;
		case LexState::INNUM:
			ch = getNextChar();
			while (isdigit(ch)) {
				semnow[++semnum] = ch;
				ch = getNextChar();
			}
			//检查数字后是否紧跟字母，如果是，报错
			if (isalpha(ch)) {
				// 将字母也读入，以便在错误信息中显示完整的错误token
				semnow[++semnum] = ch;
				// 继续读取后续的字母和数字
				ch = getNextChar();
				while (isalnum(ch)) {
					semnow[++semnum] = ch;
					ch = getNextChar();
				}
				semnow[++semnum] = '\0';
				//报告错误
				ErrNow = ErrorType::LEX_ERROR_INVALID_NUMBER;
				errnum++;
				ErrorProcess(ErrNow, linenow, errnum, '\0');
			}
			else {
				//正常处理数字token
				semnow[++semnum] = '\0';
				curr->Lex = INTC;
				curr->Lineshow = linenow;
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));
				InitNode(nextt->next);
			}
			statenow = LexState::START;
			break;
		case LexState::INCOMMENT:
			ch = getNextChar();
			while (ch != '}') {
				ch = getNextChar();
			}
			ch = getNextChar();
			statenow = LexState::START;//注释跳过即可
			break;
		case LexState::INCHAR://字符串常量
			ch = getNextChar();//因为遇到的时候没有再读
			//这里不能用while，因为单引号内部只能是单字符
			if (isalnum(ch)) {
				statenow = LexState::DONE;
				semnow[++semnum] = ch;
				semnow[++semnum] = '\0';
				curr->Lex = CHARC;
				curr->Lineshow = linenow;
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
				InitNode(nextt->next);
			}
			else {
				statenow = START;
				errnum++;
				ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;
				ErrorProcess(ErrNow,linenow,errnum,'\0');//错误：字符常量出错
				

				fclose(infile);
				return head;
			}
			break;
		
		case LexState::DONE:
			if (isalnum(ch)) {
				ch = getNextChar();
				if (ch == '\'') {
					statenow = LexState::START;
					ch = getNextChar();
					//字符结束了
				}
				else {
					errnum++;
					ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;//错误原因是单字符常量错误
					ErrorProcess(ErrNow,linenow,errnum,'\0');
					while (ch != EOF && ch != '\'') {
						ch = getNextChar();
					}
					if (ch == EOF) {
						curr->Lex = ENDFILE;
						curr->Lineshow = linenow;
						strcpy(curr->Sem, semnow);
						ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;//错误原因是单字符常量错误
						ErrorProcess(ErrNow,linenow,errnum,'\0');
						errnum++;
						ErrNow = ErrorType::FILE_READ_OVER;
						printf("词法分析结束，共有%d个词法错误\n", errnum);
						fclose(infile);
						return head;
					}
					else {
						//错误的常量结束了，开始下一阶段
						statenow = START;
						ErrNow = ErrorType::LEX_ERROR_NONE;
						ch = getNextChar();
					}
				}
			}
			else {
				//当是单分界符号结束的DONE时
				semnow[++semnum] = '\0';
				curr->Lex = charTOlex(ch);//识别是什么类型的单分界符号
				curr->Lineshow = linenow;
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//新current的指针信息是完整的
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//完善next的指针信息
				InitNode(nextt->next);
				statenow = LexState::START;
				ch = getNextChar();
			}
			break;
		default:
			//遇到未定义字符报错
			errnum++;
			ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR;//未定义字符的错误
			ErrorProcess(ErrNow,linenow,errnum,ch);
			break;
		}
	}
	return head;
}
