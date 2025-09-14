#define _CRT_SECURE_NO_WARNINGS
#include<fstream>
#include<iostream>
#include "LexicalAnalysis.h"
#include "Error_handler.h"
using namespace std;
int errnum;
//ȫ�ֱ���
LexState statenow;
ErrorType ErrNow;//�ʷ�����
int Line=0;//token���ڵ�����
int tokennum = 0;
//ID���ຯ��
LexType classifySem(char* ptr) {
	for (int i = 0; i < 21; i++) {
		if (strcmp(reservedWords[i].Sem, ptr) == 0) {
			return reservedWords[i].tok;
		}
	}
	return ID;
}

//��ӡ����ʷ���Ϣ
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
//����
int ungetNextChar() {
	fseek(infile, -(long)sizeof(char), SEEK_CUR);
	return 0;
}

//���ص�ǰ��ʲô���͵��ַ�
int ClassifyChar(char ch) {//
	if (isalpha(ch))
		return 1;//��ĸ
	if (isdigit(ch))
		return 2;//����
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=' || ch == '<' ||ch=='>'|| ch == ',')
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
	{"char",CHAR} };	//������

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

//�������ʼ��
int InitNode(TokenNode* ptr) {
	ptr->Lineshow = -1;
	ptr->Lex = ID;//��ʼ��Ϊ��ʶ��
	ptr->Sem[0] = '\0';
	ptr->next = ptr->pre = nullptr;
	return 0;
}

int LexicalOutput(TokenNode* head,FILE*infile,FILE*outfile) {
	if (errnum) { return 1; }
	if (outfile == NULL) {
		printf("\n�ʷ������ļ���ʧ��\n");
		return 1;
	}
	const char* header = "���\t����\t�ʷ���Ϣ\t\t������Ϣ\t";
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
	printf("\n�ʷ�������ɣ�����%d��token������ѱ��浽�ļ�\n", tokenCount);
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
		printf("Token: %s\n", current->Sem);  // ʹ��%s��ӡ�ַ���
		current = current->next;
		nodeCount++;
	}

	printf("Total tokens: %d\n", nodeCount);
}


//�Զ�������_______-===========================================================

TokenNode* DFA() {
	// ʹ��rewind�����ļ�ָ�뵽�ļ���ͷ
	if (infile != NULL) {
		rewind(infile);
	}
	else {
		// ����ļ�ָ��ΪNULL�����Դ��ļ�
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
			printf("�޷���д���ļ�\n");
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

	int linenow = 1;//��¼��ǰɨ�赽�ڼ���
	int count = 0;//��¼token�ĸ���
	statenow = LexState::START;//���ó�ʼ״̬
	ErrNow = ErrorType::LEX_ERROR_NONE;//��¼��ǰ��������
	char semnow[100];semnow[0] = '\0';//��¼token������
	int semnum = -1;//sem�±�
	LexType lexnow = ID;//token����
	errnum = 0;//��������
	if (infile == nullptr) {
		//�ļ���ʧ�ܣ����ô�������
		ErrNow = ErrorType::FILE_OPEN_FAILED;
		errnum++;
		ErrorProcess(ErrNow,linenow,errnum,'\0');
		return NULL;
	}
	char ch = getNextChar();
	//while (ErrNow == ErrorType::LEX_ERROR_NONE) {
	while(1){
		//ֻҪû����
		char ch2;
		switch (statenow) {
		case LexState::START:
			semnow[0] = '\0';semnum = -1;//��ռ�¼
			if (ch == '\n') { 
				linenow++;
				ch = getNextChar();
			}
			else if (isalpha(ch)) {
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
				ch = getNextChar();
				if (ch == '=') {
					statenow = LexState::INASSIGN;
					semnow[++semnum] = ch;
					break;
				}
				else {
					//:����һ������=������
					ErrNow = ErrorType::LEX_ERROR_INVALID_ASSIGN;
					errnum++;
					ErrorProcess(ErrNow, linenow,errnum,'\0');
					fclose(infile);
					return head;
				}
			}
			else if (ch == '.') {
				//�����±����
				semnow[++semnum] = ch;
				ch = getNextChar();
				if (ch == '.') {//��һ��Ҳ��.���������±����
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
				//�հ׻��Ʊ�����Թ�����
				ch = getNextChar();
				
			}
			else if (ch == EOF) {
				//�ļ�����
				curr->Lex = ENDFILE;
				curr->Lineshow = linenow;
				curr->next = NULL;
				strcpy(curr->Sem, semnow);
				ErrNow = ErrorType::FILE_READ_OVER;
				printf("�ʷ���������������%d���ʷ�����\n", errnum);
				fclose(infile);
				statenow = LexState::DONE;
				return head;
			}
			else{
				ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR;//��ǰ����û�����ַ���******************
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
			InitNode(nextt->next);
			statenow = LexState::START;  // �ع��ʼ״̬
			break;
		case INRANGE://�����±�״̬
			
			curr->Lex = UNDERANGE; strcpy(curr->Sem, ".."); curr->Lineshow = linenow;
			curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
			InitNode(nextt->next);
			ch = getNextChar();
			statenow = LexState::START;  // �ع��ʼ״̬
			break;
		case LexState::INASSIGN:
			curr->Lex = ASSIGN;//
			curr->Lineshow = linenow;
			strcpy(curr->Sem, ":=");
			curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
			nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
			InitNode(nextt->next);
			ch = getNextChar();
			statenow = LexState::START;//�ع��ʼ״̬
			break;
		case LexState::INNUM:
			ch = getNextChar();
			while (isdigit(ch)) {
				semnow[++semnum] = ch;
				ch = getNextChar();
			}
			//������ֺ��Ƿ������ĸ������ǣ�����
			if (isalpha(ch)) {
				// ����ĸҲ���룬�Ա��ڴ�����Ϣ����ʾ�����Ĵ���token
				semnow[++semnum] = ch;
				// ������ȡ��������ĸ������
				ch = getNextChar();
				while (isalnum(ch)) {
					semnow[++semnum] = ch;
					ch = getNextChar();
				}
				semnow[++semnum] = '\0';
				//�������
				ErrNow = ErrorType::LEX_ERROR_INVALID_NUMBER;
				errnum++;
				ErrorProcess(ErrNow, linenow, errnum, '\0');
			}
			else {
				//������������token
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
			statenow = LexState::START;//ע����������
			break;
		case LexState::INCHAR://�ַ�������
			ch = getNextChar();//��Ϊ������ʱ��û���ٶ�
			//���ﲻ����while����Ϊ�������ڲ�ֻ���ǵ��ַ�
			if (isalnum(ch)) {
				statenow = LexState::DONE;
				semnow[++semnum] = ch;
				semnow[++semnum] = '\0';
				curr->Lex = CHARC;
				curr->Lineshow = linenow;
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
				InitNode(nextt->next);
			}
			else {
				statenow = START;
				errnum++;
				ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;
				ErrorProcess(ErrNow,linenow,errnum,'\0');//�����ַ���������
				

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
					//�ַ�������
				}
				else {
					errnum++;
					ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;//����ԭ���ǵ��ַ���������
					ErrorProcess(ErrNow,linenow,errnum,'\0');
					while (ch != EOF && ch != '\'') {
						ch = getNextChar();
					}
					if (ch == EOF) {
						curr->Lex = ENDFILE;
						curr->Lineshow = linenow;
						strcpy(curr->Sem, semnow);
						ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR_LITERAL;//����ԭ���ǵ��ַ���������
						ErrorProcess(ErrNow,linenow,errnum,'\0');
						errnum++;
						ErrNow = ErrorType::FILE_READ_OVER;
						printf("�ʷ���������������%d���ʷ�����\n", errnum);
						fclose(infile);
						return head;
					}
					else {
						//����ĳ��������ˣ���ʼ��һ�׶�
						statenow = START;
						ErrNow = ErrorType::LEX_ERROR_NONE;
						ch = getNextChar();
					}
				}
			}
			else {
				//���ǵ��ֽ���Ž�����DONEʱ
				semnow[++semnum] = '\0';
				curr->Lex = charTOlex(ch);//ʶ����ʲô���͵ĵ��ֽ����
				curr->Lineshow = linenow;
				strcpy(curr->Sem, semnow);
				curr = nextt; nextt = nextt->next;//��current��ָ����Ϣ��������
				nextt->pre = curr; nextt->next = (TokenNode*)malloc(sizeof(TokenNode));//����next��ָ����Ϣ
				InitNode(nextt->next);
				statenow = LexState::START;
				ch = getNextChar();
			}
			break;
		default:
			//����δ�����ַ�����
			errnum++;
			ErrNow = ErrorType::LEX_ERROR_INVALID_CHAR;//δ�����ַ��Ĵ���
			ErrorProcess(ErrNow,linenow,errnum,ch);
			break;
		}
	}
	return head;
}
