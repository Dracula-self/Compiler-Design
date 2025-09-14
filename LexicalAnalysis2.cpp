#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include<iostream>
#include<cstring>
#include <cstdio>
#include "LexicalAnalysis.h"
#include "Error_handler.h"
using namespace std;
int errnum;

FILE* fp = fopen("E:/VScodes/25_complile/25_complile/test/1.txt", "r");// �����ļ���дָ��
FILE* w_fp = fopen("E:/VScodes/25_complile/25_complile/test/token.txt", "w");// token��д�ļ�

//ȫ�ֱ���
LexState statenow;
ErrorType ErrNow;//�ʷ�����
int Line = 0;//token���ڵ�����

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
	{"char",CHAR} };	//������

//ID���ຯ��
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
	{CHAR,"CHAR"} };	//������

//��ӡ����ʷ���Ϣ
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
//����
int ungetNextChar() {
	fseek(fp, -(long)sizeof(char), SEEK_CUR);
	return 0;
}


//���ص�ǰ��ʲô���͵��ַ�
int ClassifyChar(char ch) {//
	if (isalpha(ch))
		return 1;//��ĸ
	if (isdigit(ch))
		return 2;//����
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=' || ch == '<' || ch == '>' || ch == ',')
		return 3;//���ֽ��
	if (ch == ':')
		return 4;//˫�ֽ��������Ϊ��
	if (ch == '{')
		return 5;//ע��ͷ��
	if (ch == '.')
		return 6;//�����±�
	if (ch == '\'')
		return 7;//�ַ�״̬
	//����Ϊ���Բ���
	if (ch == ' ' || ch == '\t')//�ַ�Ϊ�հ�
		return 8;
	if (ch == '\n')
		return 9;
	if (ch == EOF)
		return 10;
	return 11;
}

//���ص��ʵĴʷ�����
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

//�������ʼ��
int InitNode(TokenNode* ptr) {
	ptr->Lineshow = -1;
	ptr->Lex = ID;//��ʼ��Ϊ��ʶ��
	ptr->Sem[0] = '\0';
	ptr->next = ptr->pre = nullptr;
	return 0;
}

int LexicalOutput(TokenNode* head) {
	// ����ļ�ָ���Ƿ���Ч
	if (w_fp == NULL) {
		printf("\n�ʷ������ļ���ʧ��\n");
		return 0;
	}

	//// ���ʷ������Ƿ��д���
	//if (ErrNow== ErrorType::FILE_READ_OVER) {
	//	printf("\n����⣬�ʷ��������ڴ���\n");
	//	fprintf(w_fp, "\n����⣬�ʷ��������ڴ���\n");
	//	fclose(w_fp);
	//	return 0;
	//}

	// ��ͷ��Ϣ
	const char* header = "����\t�ʷ���Ϣ\t\t������Ϣ\t";
	printf("%s\n", header);
	fprintf(w_fp, "%s\n", header);

	// �����������ÿ��Token����Ϣ
	TokenNode* current = head;
	while (current != NULL) {
		// ����к�
		printf("%d\t", current->Lineshow);
		fprintf(w_fp, "\n%d\t", current->Lineshow);

		// ����ʷ���Ϣ��ͬʱ���������̨���ļ���
		PrintLex(current->Lex);

		// ���������Ϣ
		printf("%-15s\n", current->Sem);
		fprintf(w_fp, "%-15s", current->Sem);

		// �ƶ�����һ���ڵ�
		current = current->next;
	}

	printf("\n�ʷ�������ɣ�����ѱ��浽�ļ�\n");

	// �ر��ļ�
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
		printf("Token: %s\n", current->Sem);  // ʹ��%s��ӡ�ַ���
		current = current->next;
		nodeCount++;
	}

	printf("Total tokens: %d\n", nodeCount);
}


//�Զ�������_______-===========================================================

TokenNode* DFA() {
	// ʹ��rewind�����ļ�ָ�뵽�ļ���ͷ
	if (fp != NULL) {
		rewind(fp);
	}
	else {
		// ����ļ�ָ��ΪNULL�����Դ��ļ�
		fp = fopen("E:/VScodes/25_complile/25_complile/test/1.txt", "r");
		if (fp == NULL) {
			// �ļ���ʧ�ܣ����ô�������
			ErrNow = ErrorType::FILE_OPEN_FAILED;
			errnum++;
			ErrorProcess(ErrNow, 0, errnum);
			return NULL;
		}
	}

	// ͬ������д���ļ�
	if (w_fp == NULL) {
		w_fp = fopen("E:/VScodes/25_complile/25_complile/test/token.txt", "w");
		if (w_fp == NULL) {
			// ����д���ļ���ʧ��
			printf("�޷���д���ļ�\n");
			return NULL;
		}
	}
	char prech;
	TokenNode* head = (TokenNode*)malloc(sizeof(TokenNode));
	TokenNode* nextt = (TokenNode*)malloc(sizeof(TokenNode));
	TokenNode* curr = head;
	InitNode(head);InitNode(nextt);//�ȳ�ʼ���ٷ����ڴ�
	nextt->next = (TokenNode*)malloc(sizeof(TokenNode));
	head->next = nextt;nextt->pre = head;

	int linenow = 0;//��¼��ǰɨ�赽�ڼ���
	int count = 0;//��¼token�ĸ���
	statenow = LexState::START;//���ó�ʼ״̬
	ErrNow = ErrorType::LEX_ERROR_NONE;//��¼��ǰ��������
	char semnow[100];semnow[0] = '\0';//��¼token������
	int semnum = -1;//sem�±�
	LexType lexnow = ID;//token����
	errnum = 0;//��������
	if (fp == nullptr) {
		//�ļ���ʧ�ܣ����ô�������
		ErrNow = ErrorType::FILE_OPEN_FAILED;
		errnum++;
		ErrorProcess(ErrNow, linenow, errnum);
		return NULL;
	}

	while (ErrNow == ErrorType::LEX_ERROR_NONE) {
		//ֻҪû����
		char ch = getNextChar();
		if (ch == '\n') {
			linenow++;
		}
		char ch2;
		switch (statenow) {
		case LexState::START:
			semnow[0] = '\0';semnum = -1;//��ռ�¼
			//if (ch == '\n') { linenow++; }
			if (isalpha(ch)) {
				statenow = LexState::INID;//ת���ʶ��״̬
				semnow[++semnum] = ch;
			}
			else if (isdigit(ch)) {
				//ת����״̬
				statenow = LexState::INNUM;
				semnow[++semnum] = ch;
			}
			else if (ch == '{') {
				//����ע��
				statenow = LexState::INCOMMENT;
				semnow[++semnum] = ch;
			}
			else if (ch == ':') {
				semnow[++semnum] = ch;
				//������һ�������ǲ���=
				ch2 = getNextChar();
				if (ch2 == '=') {
					statenow = LexState::INASSIGN;
					semnow[++semnum] = ch;
				}
				else {
					//:����һ������=������
					ErrNow = ErrorType::LEX_ERROR_INVALID_ASSIGN;
					errnum++;
					ErrorProcess(ErrNow, linenow, errnum);//���������Ϣ::�ĺ��治��=*********************
					//?????????????????�Ƿ�Ӧ��ֱ�ӷ���,���ֱ�ӷ��ؾ��޷��������Ƿ��д���
					while (ch != EOF && ch != '=') { ch = getNextChar(); }
					if (ch == '=') {
						//�ǾͿ��������������������
						statenow = LexState::INASSIGN;
						semnow[++semnum] = ch;
					}
					else {
						curr->Lex = ENDFILE;//��ǰtoken���;��ǽ���
						curr->Lineshow = linenow;
						strcpy(curr->Sem, "ENDFILE");
						curr->next = NULL;//????????????????????????????/
						fclose(fp);
						ErrNow = ErrorType::FILE_READ_OVER;
						printf("�ʷ���������������%d���ʷ�����\n", errnum);
						return head;
					}

				}
			}
			else if (ch == '.') {
				//�����±����
				semnow[++semnum] = ch;
				ch2 = getNextChar();
				if (ch2 == '.') {//��һ��Ҳ��.���������±����
					semnow[++semnum] = ch;
					statenow = LexState::INRANGE;
				}
				else {
					//������ǳ��������־,ע�⣡����������������ļ�������ϣ�����return
					statenow = LexState::START;//��ʼ��һ�γ���
					curr->Lex = DOT;
					curr->Lineshow = linenow;
					strcpy(curr->Sem, ".");
					curr = nextt;nextt = nextt->next;nextt->pre = curr;
					nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����ռ�
					statenow = LexState::START;
				}
			}
			else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=' || ch == '<' || ch == '>' || ch == ',') {
				if (ch == ';') {
					int i = 0;
				}//������
				semnow[++semnum] = ch;
				semnow[++semnum] = '\0';//***********************************
				statenow = LexState::DONE;
				prech = ch;
			}
			else if (ch == '\'') {
				//���������ţ��ַ�������????????????????
				statenow = LexState::INCHAR;
			}
			else if (ch == ' ' || ch == '\t') {
				//�հ׻��Ʊ�����Թ�����
				statenow = LexState::START;//�ص���ʼ
			}
			else if (ch == EOF) {
				//�ļ�����
				curr->Lex = ENDFILE;
				curr->Lineshow = linenow;
				curr->next = NULL;
				strcpy(curr->Sem, semnow);
				ErrNow = ErrorType::FILE_READ_OVER;
				printf("�ʷ���������������%d���ʷ�����\n", errnum);
				fclose(fp);
				statenow = LexState::DONE;
				return head;
			}
			else {
				ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR;//��ǰ����û�����ַ���******************
				errnum++;
				ErrorProcess(ErrNow, linenow, errnum);
				statenow = LexState::START;
				ErrNow = ErrorType::LEX_ERROR_NONE;
			}
			break;

		case LexState::INID:
			while (isalnum(ch)) {
				//����ĸ��������
				semnow[++semnum] = ch;
				ch = getNextChar();
			}
			// ��־������
			semnow[++semnum] = '\0';
			curr->Lex = classifySem(semnow);  // ʹ��classifySem����ʶ���ʶ��������
			curr->Lineshow = linenow;
			strcpy(curr->Sem, semnow);
			curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ

			statenow = LexState::START;  // �ع��ʼ״̬
			break;
		case INRANGE://�����±�״̬

			curr->Lex = UNDERANGE; strcpy(curr->Sem, ".."); curr->Lineshow = linenow;
			curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
			statenow = LexState::START;  // �ع��ʼ״̬
			break;
		case LexState::INASSIGN:

			curr->Lex = ASSIGN;//
			curr->Lineshow = linenow;
			strcpy(curr->Sem, ":=");
			curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
			statenow = LexState::START;//�ع��ʼ״̬
			break;
		case LexState::INNUM:

			while (isdigit(ch)) {
				ch = getNextChar();
				semnow[++semnum] = ch;
			}
			semnow[++semnum] = '\0';
			//�����˷����ֲ��֣���������token�Ѿ����
			curr->Lex = INTC;//���ͣ�����
			curr->Lineshow = linenow;
			strcpy(curr->Sem, semnow);
			curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
			statenow = LexState::START;//�ع��ʼ״̬
			break;
		case LexState::INCOMMENT:

			while (ch != '}') {
				ch = getNextChar();
			}
			statenow = LexState::START;//ע����������
			break;
		case LexState::INCHAR://�ַ�������

			ch2 = getNextChar();//��Ϊ������ʱ��û���ٶ�
			//���ﲻ����while����Ϊ�������ڲ�ֻ���ǵ��ַ�
			if (isalnum(ch2)) {
				statenow = LexState::DONE;
				semnow[++semnum] = ch2;
				semnow[++semnum] = '\0';
				curr->Lex = CHAR;
				curr->Lineshow = linenow;
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
			}
			else {
				statenow = START;
				errnum++;
				ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;
				ErrorProcess(ErrNow, linenow, errnum);//�����ַ���������
				//��Ȼ����������ǿ��Լ�������ɨ��,��ʿ����
				while (!isalnum(ch)) {
					ch = getNextChar();
				}
				statenow = LexState::DONE;
				curr->Lex = CHAR;
				curr->Lineshow = linenow;
				semnow[++semnum] = ch2;
				semnow[++semnum] = '\0';
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
			}
			break;

		case LexState::DONE:

			if (isalnum(prech)) {
				//ch = getNextChar();
				if (ch == '\'') {
					statenow = LexState::START;
					//�ַ�������
				}
				else {
					errnum++;
					ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;//����ԭ���ǵ��ַ���������
					ErrorProcess(ErrNow, linenow, errnum);
					while (ch != EOF && ch != '\'') {
						ch = getNextChar();
					}
					if (ch == EOF) {
						curr->Lex = ENDFILE;
						curr->Lineshow = linenow;
						strcpy(curr->Sem, semnow);
						ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;//����ԭ���ǵ��ַ���������
						ErrorProcess(ErrNow, linenow, errnum);
						//errno++;
						ErrNow = ErrorType::FILE_READ_OVER;
						printf("�ʷ���������������%d���ʷ�����\n", errnum);
						fclose(fp);
						return head;
					}
					else {
						//����ĳ��������ˣ���ʼ��һ�׶�
						statenow = START;
						ErrNow = ErrorType::LEX_ERROR_NONE;
					}
				}
			}
			else {
				//���ǵ��ֽ���Ž�����DONEʱ
				semnow[++semnum] = '\0';
				curr->Lex = ClassifyLex(prech);//ʶ����ʲô���͵ĵ��ֽ����
				curr->Lineshow = linenow;
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
				statenow = LexState::START;
			}
			break;
		default:
			//����δ�����ַ�����
			errnum++;
			ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR;//δ�����ַ��Ĵ���
			ErrorProcess(ErrNow, linenow, errnum, ch);
			statenow = LexState::START;
			break;
		}
	}
}
