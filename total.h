#pragma once
#include<vector>
#include<string>
using namespace std;
//�﷨���ڵ㺢�ӵ�������
#define MAXCHILDREN 5
//��ʶ�����Ƴ���
#define IDLENGTH 10
extern FILE* infile;
extern FILE* outfile;
//�ʷ������׶�----------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{	//�ļ�����,����
	ENDFILE, ERROR,
	//������,�����ֺ��������ڱ�ʶ�����룬��ʶ����ID��״̬��ʶ��������֣����ͣ���ID
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO, ENDWH,
	BEGIN, END, READ, WRITE, ARRAY, OF,
	RECORD, RETURN,
	//����,���ͺ��ַ���
	INTEGER, CHAR,
	//���ַ����ʷ���(��ʶ��,���ͳ���,�ַ�����
	ID, INTC, CHARC,
	//�������(������,������
	ASSIGN, EQ, LT, GT,PLUS, MINUS,
	TIMES, OVER,
	LPAREN, RPAREN, DOT,
	COLON, SEMI, COMMA, LMIDPAREN, RMIDPAREN,
	UNDERANGE//�����±�
}LexType;
typedef LexType Endsign;//������Ϊ�ռ�����

//�����㶨��,������ŵ�lextypeǰ��ᱨ��
struct TokenNode
{
	int Lineshow;//������������
	char Sem[100];//����������Ϣ
	LexType Lex;
	TokenNode* pre;
	TokenNode* next;
};
//�﷨�����׶�------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum { ProK, PheadK, TypeK, VarK, ProcDecK, StmLK, DecK, StmtK, ExpK, nodeNull }NodeKind;
typedef enum { ArrayK, CharK, IntegerK, RecordK, IdK, decNull }  DecKind;
typedef enum { IfK, WhileK, AssignK, ReadK, WriteK, CallK, ReturnK, stmtNull } StmtKind;
typedef enum { OpK, ConstK, VariK, expKindNull } ExpKind;//���ʽ���ͷ�Ϊ���������ͣ�a+b ���������ͣ�6 �������ͣ�a
typedef enum { IdV, ArrayMembV, FieldMembV, varNull } VarKind; //��ʶ������ �����Ա���� ���Ա����
typedef enum { Void, Integer, Boolean, expTypeNull } ExpType;//���ʽ�����ڵ�ļ�����ͣ�Ϊ��������б�����
typedef enum { none, varparamType, valparamType, paramNull } ParamType;

//�﷨��
struct TreeNode
{
	struct TreeNode* child[MAXCHILDREN];
	int childnum;
	struct TreeNode* sibling;
	int lineno;
	NodeKind nodekind;             // �ڵ�������������仹�Ǳ��ʽ
	union
	{
		DecKind  dec;
		StmtKind stmt;
		ExpKind  exp;
	} kind;                        // �������� dec/stmt/exp
	int idnum;                     // һ���ڵ��б�ʶ���ĸ���
	vector<string> name; // ��ʶ��������
	struct SymbolTable* table[10];   // ���ű�ָ��
	std::string type_name;         // ��������
	struct
	{
		struct
		{
			int low;
			int up;
			DecKind childtype;
		}ArrayAttr;
		struct
		{
			ParamType paramt;
		}ProcAttr;
		struct
		{
			LexType op;
			int val;
			VarKind varkind;
			ExpType type;
			bool ischar;
		}ExpAttr;
	} attr;
};
void InitTreeNode(struct TreeNode* t);
//�﷨�����ӻ���-----------------------------------------------------------------------------------------------------------------------------------

//��������׶�------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum { intTy, charTy, arrayTy, recordTy, boolTy }TypeKind;
typedef enum { typeKind, varKind, procKind }IdKind;
typedef enum { Dir, InDir }AccessKind;
//�����͵�Ԫ�ṹ
struct FieldChain {
	char idname[IDLENGTH];//������
	struct TypeIR* unitType;//ָ���ʶ���������ڲ���ʾ�����л���5��(intTy, charTy, arrayTy, recordTy, boolTy)���Զ���
	int offset;//�ڼ�¼�е�ƫ��
	struct FieldChain* next;
};


//�м�������ɽ׶�---------------------------------------------------------------------------------------------------------------------------------------------------------------
//�м��������p181
enum Codekind {
	ADD, SUB, MULT, DIV,
	EQC, LTC,
	READC, WRITEC, RETURNC, ASSIG, AADD, LABEL, JUMP, JUMP0, CALL, VARACT, VALACT,
	PENTRY, ENDPROC, MENTRY, ENDWHILE, WHILESTART
};
const string codekindStrings[] = {
	"ADD", "SUB", "MULT", "DIV",
	"EQC", "LTC",
	"READC", "WRITEC", "RETURNC", "ASSIG", "AADD", "LABEL", "JUMP", "JUMP0", "CALL", "VARACT","VALACT",
	"PENTRY", "ENDPROC", "MENTRY", "ENDWHILE","WHILESTART"
};
//ARG�ṹ
enum ARGform { ValueForm, LabelForm, Addrform };
//enum Access {dir,indir};
struct ARG {
	ARGform form;//ARG�ṹ����ֵ�࣬��ַ�໹�Ǳ����
	union {
		int value;//��¼����ֵ
		int label;//��¼���ֵ
		struct
		{
			char name[10];
			int dataLevel;
			int dataOff;
			AccessKind access;//������dir����indir
		}Addr;
	}Attr;
};
//�м����Ľṹ
typedef struct{
	Codekind codekind;
	ARG* arg1;
	ARG* arg2;
	ARG* arg3;
} midcode;
//�м���������

//�м��������
struct codeFile {
	codeFile* former;
	midcode onecode;
	codeFile* next;
};

char* stringToChar(string& str);






