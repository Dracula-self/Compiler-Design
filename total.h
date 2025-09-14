#pragma once
#include<vector>
#include<string>
using namespace std;
//语法树节点孩子的最大个数
#define MAXCHILDREN 5
//标识符名称长度
#define IDLENGTH 10
extern FILE* infile;
extern FILE* outfile;
//词法分析阶段----------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{	//文件结束,错误
	ENDFILE, ERROR,
	//保留字,保留字和类型属于标识符范畴，标识符（ID）状态可识别出保留字，类型，和ID
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO, ENDWH,
	BEGIN, END, READ, WRITE, ARRAY, OF,
	RECORD, RETURN,
	//类型,整型和字符型
	INTEGER, CHAR,
	//多字符单词符号(标识符,整型常量,字符常量
	ID, INTC, CHARC,
	//特殊符号(操作符,标点符号
	ASSIGN, EQ, LT, GT,PLUS, MINUS,
	TIMES, OVER,
	LPAREN, RPAREN, DOT,
	COLON, SEMI, COMMA, LMIDPAREN, RMIDPAREN,
	UNDERANGE//数组下标
}LexType;
typedef LexType Endsign;//重命名为终极符号

//链表结点定义,把这个放到lextype前面会报错
struct TokenNode
{
	int Lineshow;//单词所在行数
	char Sem[100];//单词语义信息
	LexType Lex;
	TokenNode* pre;
	TokenNode* next;
};
//语法分析阶段------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum { ProK, PheadK, TypeK, VarK, ProcDecK, StmLK, DecK, StmtK, ExpK, nodeNull }NodeKind;
typedef enum { ArrayK, CharK, IntegerK, RecordK, IdK, decNull }  DecKind;
typedef enum { IfK, WhileK, AssignK, ReadK, WriteK, CallK, ReturnK, stmtNull } StmtKind;
typedef enum { OpK, ConstK, VariK, expKindNull } ExpKind;//表达式类型分为操作符类型：a+b 常整数类型：6 变量类型：a
typedef enum { IdV, ArrayMembV, FieldMembV, varNull } VarKind; //标识符变量 数组成员变量 域成员变量
typedef enum { Void, Integer, Boolean, expTypeNull } ExpType;//表达式整个节点的检查类型（为语义分析判别打基础
typedef enum { none, varparamType, valparamType, paramNull } ParamType;

//语法树
struct TreeNode
{
	struct TreeNode* child[MAXCHILDREN];
	int childnum;
	struct TreeNode* sibling;
	int lineno;
	NodeKind nodekind;             // 节点是声明还是语句还是表达式
	union
	{
		DecKind  dec;
		StmtKind stmt;
		ExpKind  exp;
	} kind;                        // 具体类型 dec/stmt/exp
	int idnum;                     // 一个节点中标识符的个数
	vector<string> name; // 标识符的名称
	struct SymbolTable* table[10];   // 符号表指针
	std::string type_name;         // 类型名称
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
//语法树可视化—-----------------------------------------------------------------------------------------------------------------------------------

//语义分析阶段------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum { intTy, charTy, arrayTy, recordTy, boolTy }TypeKind;
typedef enum { typeKind, varKind, procKind }IdKind;
typedef enum { Dir, InDir }AccessKind;
//域类型单元结构
struct FieldChain {
	char idname[IDLENGTH];//变量名
	struct TypeIR* unitType;//指向标识符的类型内部表示，共有基本5种(intTy, charTy, arrayTy, recordTy, boolTy)和自定义
	int offset;//在记录中的偏移
	struct FieldChain* next;
};


//中间代码生成阶段---------------------------------------------------------------------------------------------------------------------------------------------------------------
//中间代码的类别p181
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
//ARG结构
enum ARGform { ValueForm, LabelForm, Addrform };
//enum Access {dir,indir};
struct ARG {
	ARGform form;//ARG结构是数值类，地址类还是标号类
	union {
		int value;//记录常数值
		int label;//记录标号值
		struct
		{
			char name[10];
			int dataLevel;
			int dataOff;
			AccessKind access;//定义是dir还是indir
		}Addr;
	}Attr;
};
//中间代码的结构
typedef struct{
	Codekind codekind;
	ARG* arg1;
	ARG* arg2;
	ARG* arg3;
} midcode;
//中间代码的序列

//中间代码类型
struct codeFile {
	codeFile* former;
	midcode onecode;
	codeFile* next;
};

char* stringToChar(string& str);






