#define _CRT_SECURE_NO_WARNINGS
#include "total.h"
#include "ll1.h"
#include<stack>
#include<tuple>
#include<cstring>
#define Program ll1types::Program
using namespace std;
// 导入所有需要的枚举成员到全局命名空间

using ll1types::ProgramHead;
using ll1types::ProgramName;
using ll1types::DeclarePart;
using ll1types::TypeDec;
using ll1types::TypeDeclaration;
using ll1types::TypeDecList;
using ll1types::TypeDecMore;
using ll1types::TypeId;
using ll1types::TypeDef;
using ll1types::BaseType;
using ll1types::StructureType;
using ll1types::ArrayType;
using ll1types::Low;
using ll1types::Top;
using ll1types::RecType;
using ll1types::FieldDecList;
using ll1types::FieldDecMore;
using ll1types::IdList;
using ll1types::IdMore;
using ll1types::VarDec;
using ll1types::VarDeclaration;
using ll1types::VarDecList;
using ll1types::VarDecMore;
using ll1types::VarIdList;
using ll1types::VarIdMore;
using ll1types::ProcDec;
using ll1types::ProcDeclaration;
using ll1types::ProcDecMore;
using ll1types::ProcName;
using ll1types::ParamList;
using ll1types::ParamDecList;
using ll1types::ParamMore;
using ll1types::Param;
using ll1types::FormList;
using ll1types::FidMore;
using ll1types::ProcDecPart;
using ll1types::ProcBody;
using ll1types::ProgramBody;
using ll1types::StmList;
using ll1types::StmMore;
using ll1types::Stm;
using ll1types::AssCall;
using ll1types::AssignmentRest;
using ll1types::ConditionalStm;
using ll1types::StmL;
using ll1types::LoopStm;
using ll1types::InputStm;
using ll1types::InVar;
using ll1types::OutputStm;
using ll1types::ReturnStm;
using ll1types::CallStmRest;
using ll1types::ActParamList;
using ll1types::ActParamMore;
using ll1types::RelExp;
using ll1types::OtherRelE;
using ll1types::Exp;
using ll1types::OtherTerm;
using ll1types::Term;
using ll1types::OtherFactor;
using ll1types::Factor;
using ll1types::Variable;
using ll1types::VariMore;
using ll1types::FieldVar;
using ll1types::FieldVarMore;
using ll1types::CmpOp;
using ll1types::AddOp;
using ll1types::MultOp;
//using ll1types::INTEGER;
//using ll1types::CHAR;
//using ll1types::ARRAY;
//using ll1types::RECORD;
//using ll1types::ID;
//using ll1types::INTC;
//using ll1types::SEMI;
//using ll1types::COMMA;
//using ll1types::ASSIGN;
//using ll1types::DOT;
//using ll1types::LPAREN;
//using ll1types::RPAREN;
//using ll1types::LMIDPAREN;
//using ll1types::RMIDPAREN;
//using ll1types::EQ;
//using ll1types::LT;
//using ll1types::PLUS;
//using ll1types::MINUS;
//using ll1types::TIMES;
//using ll1types::OVER;
//using ll1types::THEN;
//using ll1types::DO;
//using ll1types::BEGIN;
//using ll1types::END;
//using ll1types::ENDWH;
//using ll1types::ELSE;
//using ll1types::FI;
//using ll1types::RETURN;
//using ll1types::READ;
//using ll1types::WRITE;
//using ll1types::VAR;
//using ll1types::PROCEDURE;
//using ll1types::COLON;
//using ll1types::UNDERANGE;
//using ll1types::OF;
//using ll1types::ENDFILE;

//LL1分析表
int LTable[110][110];
//行数
int LL1Line = 0;
//LL1符号栈,第一个int为1是终极，0非终极
stack<tuple<int, int, string>>signStack;
//语法树栈，存放指向儿子或者兄弟节点的指针的地址
stack<TreeNode**>TreeStack;
//操作符栈
stack<TreeNode*>OpsignStack;
//操作数栈
stack<TreeNode*>OpnumStack;
//当前处理的token
TokenNode* LL1NowToken;
//语法树当前节点
TreeNode* currentP;
//记录返回类型信息的成员地址
DecKind* reTypeAdd;
//保存函数声明节点指针，恢复函数声明节点
TreeNode* saveFuncP;


void createtable() {
	for (int i = 0;i < 110;i++) {
		for (int j = 0;j < 110;j++) {
			LTable[i][j] = 0;
		}
	}
	LTable[Program][PROGRAM] = 1;
	LTable[ProgramHead][PROGRAM] = 2;
	LTable[ProgramName][ID] = 3;
	LTable[DeclarePart][TYPE] = 4;
	LTable[DeclarePart][VAR] = 4;
	LTable[DeclarePart][PROCEDURE] = 4;
	LTable[DeclarePart][BEGIN] = 4;
	LTable[TypeDec][VAR] = 5;
	LTable[TypeDec][PROCEDURE] = 5;
	LTable[TypeDec][BEGIN] = 5;
	LTable[TypeDec][TYPE] = 6;
	LTable[TypeDeclaration][TYPE] = 7;
	LTable[TypeDecList][ID] = 8;
	LTable[TypeDecMore][VAR] = 9;
	LTable[TypeDecMore][PROCEDURE] = 9;
	LTable[TypeDecMore][BEGIN] = 9;
	LTable[TypeDecMore][ID] = 10;
	LTable[TypeId][ID] = 11;
	LTable[TypeDef][INTEGER] = 12;
	LTable[TypeDef][CHAR] = 12;
	LTable[TypeDef][ARRAY] = 13;
	LTable[TypeDef][RECORD] = 13;
	LTable[TypeDef][ID] = 14;
	LTable[BaseType][INTEGER] = 15;
	LTable[BaseType][CHAR] = 16;
	LTable[StructureType][ARRAY] = 17;
	LTable[StructureType][RECORD] = 18;
	LTable[ArrayType][ARRAY] = 19;
	LTable[Low][INTC] = 20;
	LTable[Top][INTC] = 21;
	LTable[RecType][RECORD] = 22;
	LTable[FieldDecList][INTEGER] = 23;
	LTable[FieldDecList][CHAR] = 23;
	LTable[FieldDecList][ARRAY] = 24;
	LTable[FieldDecMore][END] = 25;
	LTable[FieldDecMore][INTEGER] = 26;
	LTable[FieldDecMore][CHAR] = 26;
	LTable[FieldDecMore][ARRAY] = 26;
	LTable[IdList][ID] = 27;
	LTable[IdMore][SEMI] = 28;
	LTable[IdMore][COMMA] = 29;
	LTable[VarDec][PROCEDURE] = 30;
	LTable[VarDec][BEGIN] = 30;
	LTable[VarDec][VAR] = 31;
	LTable[VarDeclaration][VAR] = 32;
	LTable[VarDecList][INTEGER] = 33;
	LTable[VarDecList][CHAR] = 33;
	LTable[VarDecList][ARRAY] = 33;
	LTable[VarDecList][RECORD] = 33;
	LTable[VarDecList][ID] = 33;
	LTable[VarDecMore][PROCEDURE] = 34;
	LTable[VarDecMore][BEGIN] = 34;
	LTable[VarDecMore][INTEGER] = 35;
	LTable[VarDecMore][CHAR] = 35;
	LTable[VarDecMore][ARRAY] = 35;
	LTable[VarDecMore][RECORD] = 35;
	LTable[VarDecMore][ID] = 35;
	LTable[VarIdList][ID] = 36;
	LTable[VarIdMore][SEMI] = 37;
	LTable[VarIdMore][COMMA] = 38;
	LTable[ProcDec][BEGIN] = 39;
	LTable[ProcDec][PROCEDURE] = 40;
	LTable[ProcDeclaration][PROCEDURE] = 41;
	LTable[ProcDecMore][BEGIN] = 42;
	LTable[ProcDecMore][PROCEDURE] = 43;
	LTable[ProcName][ID] = 44;
	LTable[ParamList][RPAREN] = 45;
	LTable[ParamList][INTEGER] = 46;
	LTable[ParamList][CHAR] = 46;
	LTable[ParamList][ARRAY] = 46;
	LTable[ParamList][RECORD] = 46;
	LTable[ParamList][ID] = 46;
	LTable[ParamList][VAR] = 46;
	LTable[ParamDecList][INTEGER] = 47;
	LTable[ParamDecList][CHAR] = 47;
	LTable[ParamDecList][ARRAY] = 47;
	LTable[ParamDecList][RECORD] = 47;
	LTable[ParamDecList][ID] = 47;
	LTable[ParamDecList][VAR] = 47;
	LTable[ParamMore][RPAREN] = 48;
	LTable[ParamMore][SEMI] = 49;
	LTable[Param][INTEGER] = 50;
	LTable[Param][CHAR] = 50;
	LTable[Param][ARRAY] = 50;
	LTable[Param][RECORD] = 50;
	LTable[Param][ID] = 50;
	LTable[Param][VAR] = 51;
	LTable[FormList][ID] = 52;
	LTable[FidMore][SEMI] = 53;
	LTable[FidMore][RPAREN] = 53;
	LTable[FidMore][COMMA] = 54;
	LTable[ProcDecPart][TYPE] = 55;
	LTable[ProcDecPart][VAR] = 55;
	LTable[ProcDecPart][PROCEDURE] = 55;
	LTable[ProcDecPart][BEGIN] = 55;
	LTable[ProcBody][BEGIN] = 56;
	LTable[ProgramBody][BEGIN] = 57;
	LTable[StmList][ID] = 58;
	LTable[StmList][IF] = 58;
	LTable[StmList][WHILE] = 58;
	LTable[StmList][RETURN] = 58;
	LTable[StmList][READ] = 58;
	LTable[StmList][WRITE] = 58;
	LTable[StmMore][END] = 59;
	LTable[StmMore][ENDWH] = 59;
	LTable[StmMore][ELSE] = 59;
	LTable[StmMore][FI] = 59;
	LTable[StmMore][SEMI] = 60;
	LTable[Stm][IF] = 61;
	LTable[Stm][WHILE] = 62;
	LTable[Stm][READ] = 63;
	LTable[Stm][WRITE] = 64;
	LTable[Stm][RETURN] = 65;
	LTable[Stm][ID] = 66;
	LTable[AssCall][ASSIGN] = 67;
	LTable[AssCall][LMIDPAREN] = 67;
	LTable[AssCall][DOT] = 67;
	LTable[AssCall][LPAREN] = 68;
	LTable[AssignmentRest][ASSIGN] = 69;
	LTable[AssignmentRest][LMIDPAREN] = 69;
	LTable[AssignmentRest][DOT] = 69;
	LTable[ConditionalStm][IF] = 70;
	LTable[LoopStm][WHILE] = 71;
	LTable[InputStm][READ] = 72;
	LTable[InVar][ID] = 73;
	LTable[OutputStm][WRITE] = 74;
	LTable[ReturnStm][RETURN] = 75;
	LTable[CallStmRest][LPAREN] = 76;
	LTable[ActParamList][RPAREN] = 77;
	LTable[ActParamList][ID] = 78;
	LTable[ActParamList][INTC] = 78;
	LTable[ActParamList][LPAREN] = 78;
	LTable[ActParamMore][RPAREN] = 79;
	LTable[ActParamMore][COMMA] = 80;
	LTable[RelExp][LPAREN] = 81;
	LTable[RelExp][INTC] = 81;
	LTable[RelExp][ID] = 81;
	LTable[OtherRelE][LT] = 82;
	LTable[OtherRelE][EQ] = 82;
	LTable[Exp][LPAREN] = 83;
	LTable[Exp][INTC] = 83;
	LTable[Exp][ID] = 83;
	LTable[OtherTerm][LT] = 84;
	LTable[OtherTerm][EQ] = 84;
	LTable[OtherTerm][THEN] = 84;
	LTable[OtherTerm][DO] = 84;
	LTable[OtherTerm][RPAREN] = 84;
	LTable[OtherTerm][END] = 84;
	LTable[OtherTerm][SEMI] = 84;
	LTable[OtherTerm][COMMA] = 84;
	LTable[OtherTerm][ENDWH] = 84;
	LTable[OtherTerm][ELSE] = 84;
	LTable[OtherTerm][FI] = 84;
	LTable[OtherTerm][RMIDPAREN] = 84;
	LTable[OtherTerm][PLUS] = 85;
	LTable[OtherTerm][MINUS] = 85;
	LTable[Term][LPAREN] = 86;
	LTable[Term][INTC] = 86;
	LTable[Term][ID] = 86;
	LTable[OtherFactor][PLUS] = 87;
	LTable[OtherFactor][MINUS] = 87;
	LTable[OtherFactor][LT] = 87;
	LTable[OtherFactor][EQ] = 87;
	LTable[OtherFactor][THEN] = 87;
	LTable[OtherFactor][ELSE] = 87;
	LTable[OtherFactor][FI] = 87;
	LTable[OtherFactor][DO] = 87;
	LTable[OtherFactor][ENDWH] = 87;
	LTable[OtherFactor][RPAREN] = 87;
	LTable[OtherFactor][END] = 87;
	LTable[OtherFactor][SEMI] = 87;
	LTable[OtherFactor][COMMA] = 87;
	LTable[OtherFactor][RMIDPAREN] = 87;
	LTable[OtherFactor][TIMES] = 88;
	LTable[OtherFactor][OVER] = 88;
	LTable[Factor][LPAREN] = 89;
	LTable[Factor][INTC] = 90;
	LTable[Factor][ID] = 91;
	LTable[Variable][ID] = 92;
	LTable[VariMore][ASSIGN] = 93;
	LTable[VariMore][TIMES] = 93;
	LTable[VariMore][OVER] = 93;
	LTable[VariMore][PLUS] = 93;
	LTable[VariMore][MINUS] = 93;
	LTable[VariMore][LT] = 93;
	LTable[VariMore][EQ] = 93;
	LTable[VariMore][THEN] = 93;
	LTable[VariMore][ELSE] = 93;
	LTable[VariMore][FI] = 93;
	LTable[VariMore][DO] = 93;
	LTable[VariMore][ENDWH] = 93;
	LTable[VariMore][RPAREN] = 93;
	LTable[VariMore][END] = 93;
	LTable[VariMore][SEMI] = 93;
	LTable[VariMore][COMMA] = 93;
	LTable[VariMore][RMIDPAREN] = 93;
	LTable[VariMore][LMIDPAREN] = 94;
	LTable[VariMore][DOT] = 95;
	LTable[FieldVar][ID] = 96;
	LTable[FieldVarMore][ASSIGN] = 97;
	LTable[FieldVarMore][TIMES] = 97;
	LTable[FieldVarMore][OVER] = 97;
	LTable[FieldVarMore][PLUS] = 97;
	LTable[FieldVarMore][MINUS] = 97;
	LTable[FieldVarMore][LT] = 97;
	LTable[FieldVarMore][EQ] = 97;
	LTable[FieldVarMore][THEN] = 97;
	LTable[FieldVarMore][ELSE] = 97;
	LTable[FieldVarMore][FI] = 97;
	LTable[FieldVarMore][DO] = 97;
	LTable[FieldVarMore][ENDWH] = 97;
	LTable[FieldVarMore][RPAREN] = 97;
	LTable[FieldVarMore][END] = 97;
	LTable[FieldVarMore][SEMI] = 97;
	LTable[FieldVarMore][LMIDPAREN] = 98;
	LTable[CmpOp][LT] = 99;
	LTable[CmpOp][EQ] = 100;
	LTable[AddOp][PLUS] = 101;
	LTable[AddOp][MINUS] = 102;
	LTable[MultOp][TIMES] = 103;
	LTable[MultOp][OVER] = 104;
}
//选择产生式函数
void selectPredict(int num) {
	switch (num) {
	case 1: process1(); break;
	case 2: process2(); break;
	case 3: process3(); break;
	case 4: process4(); break;
	case 5: process5(); break;
	case 6: process6(); break;
	case 7: process7(); break;
	case 8: process8(); break;
	case 9: process9(); break;
	case 10: process10(); break;
	case 11: process11(); break;
	case 12: process12(); break;
	case 13: process13(); break;
	case 14: process14(); break;
	case 15: process15(); break;
	case 16: process16(); break;
	case 17: process17(); break;
	case 18: process18(); break;
	case 19: process19(); break;
	case 20: process20(); break;
	case 21: process21(); break;
	case 22: process22(); break;
	case 23: process23(); break;
	case 24: process24(); break;
	case 25: process25(); break;
	case 26: process26(); break;
	case 27: process27(); break;
	case 28: process28(); break;
	case 29: process29(); break;
	case 30: process30(); break;
	case 31: process31(); break;
	case 32: process32(); break;
	case 33: process33(); break;
	case 34: process34(); break;
	case 35: process35(); break;
	case 36: process36(); break;
	case 37: process37(); break;
	case 38: process38(); break;
	case 39: process39(); break;
	case 40: process40(); break;
	case 41: process41(); break;
	case 42: process42(); break;
	case 43: process43(); break;
	case 44: process44(); break;
	case 45: process45(); break;
	case 46: process46(); break;
	case 47: process47(); break;
	case 48: process48(); break;
	case 49: process49(); break;
	case 50: process50(); break;
	case 51: process51(); break;
	case 52: process52(); break;
	case 53: process53(); break;
	case 54: process54(); break;
	case 55: process55(); break;
	case 56: process56(); break;
	case 57: process57(); break;
	case 58: process58(); break;
	case 59: process59(); break;
	case 60: process60(); break;
	case 61: process61(); break;
	case 62: process62(); break;
	case 63: process63(); break;
	case 64: process64(); break;
	case 65: process65(); break;
	case 66: process66(); break;
	case 67: process67(); break;
	case 68: process68(); break;
	case 69: process69(); break;
	case 70: process70(); break;
	case 71: process71(); break;
	case 72: process72(); break;
	case 73: process73(); break;
	case 74: process74(); break;
	case 75: process75(); break;
	case 76: process76(); break;
	case 77: process77(); break;
	case 78: process78(); break;
	case 79: process79(); break;
	case 80: process80(); break;
	case 81: process81(); break;
	case 82: process82(); break;
	case 83: process83(); break;
	case 84: process84(); break;
	case 85: process85(); break;
	case 86: process86(); break;
	case 87: process87(); break;
	case 88: process88(); break;
	case 89: process89(); break;
	case 90: process90(); break;
	case 91: process91(); break;
	case 92: process92(); break;
	case 93: process93(); break;
	case 94: process94(); break;
	case 95: process95(); break;
	case 96: process96(); break;
	case 97: process97(); break;
	case 98: process98(); break;
	case 99: process99(); break;
	case 100: process100(); break;
	case 101: process101(); break;
	case 102: process102(); break;
	case 103: process103(); break;
	case 104: process104(); break;
	case 0:
	default: {
		printf("语法错误：第%d行，%s不是期望的token\n",LL1NowToken->Lineshow,LL1NowToken->Sem);
	}
	}
}

void LL1InitTreeNode(struct TreeNode* t) {
	if (t == NULL) return;
	for (int i = 0; i < MAXCHILDREN; i++) {
		t->child[i] = NULL;
	}
	t->childnum = 0;
	t->sibling = NULL;
	t->lineno = LL1Line;
	t->nodekind = DecK;  // 默认是声明节点
	t->kind.dec = decNull;  // 未定义类型
	t->name.clear();
	t->idnum = 0;
	// 初始化符号表指针
	for (int i = 0; i < 10; i++) {
		t->table[i] = NULL;
	}
	t->type_name = "";
	t->attr.ArrayAttr.low = 0;
	t->attr.ArrayAttr.up = 0;
	t->attr.ExpAttr.ischar = false;//假设是数字
	t->attr.ArrayAttr.childtype = decNull;
	t->attr.ProcAttr.paramt = paramNull;
}

//LL1语法分析主函数
//采用表驱动
TreeNode* parseLL1(TokenNode* head) {
	createtable();
	LL1NowToken = head;
	//开始符号program压栈
	signStack.push({ 0,Program,"Program" });
	//创建语法根节点Prok
	TreeNode* root = new TreeNode();
	LL1InitTreeNode(root);
	root->nodekind = ProK;
	//三个儿子指针压进语法树
	TreeStack.push(&(root->child[2]));
	TreeStack.push(&(root->child[1]));
	TreeStack.push(&(root->child[0]));
	LL1Line = LL1NowToken->Lineshow;
	while (!signStack.empty()) {
		if (LL1NowToken->Lineshow == 8) {
			int ii = 1;
		}
		auto now = signStack.top();
		if (get<0>(now) == 1) {
			//栈顶是终极符，匹配当前token,成功谈栈
			LexType p = (LexType)get<1>(now);
			if ((Endsign)get<1>(now) == LL1NowToken->Lex) {//需要用到强制转换
				signStack.pop();
				LL1NowToken = LL1NowToken->next;
				LL1Line = LL1NowToken->Lineshow;
			}
			else {
				printf("语法错误：第%d行，%s不是期望的单词", LL1Line, LL1NowToken->Sem);
				exit(0);
			}
		}
		else {
			//现在不是非终极符号
			auto stacktop = (ll1types::noEndsign)get<1>(now);//找到非终极符号是谁
			//到LL1里面去找，按照产生式右部规则逆序入栈
			int num = LTable[stacktop][LL1NowToken->Lex];
			signStack.pop();
			selectPredict(num);

		}
	}
	if (LL1NowToken->Lex != ENDFILE) {
		printf("语法错误：第%d行，文件提前结束", LL1Line);
	}
	return root;
}
//返回操作符号的优先级
int Priosity(LexType op) {
	switch (op) {
	case END:
		return 0;break;
	case LPAREN:
		return 1;break;
	case LT:
		return 2;break;
	case EQ:
		return 2;break;
	case PLUS:
		return 3;break;
	case MINUS:
		return 3;break;
	case TIMES:
		return 4;break;
	case OVER:
		return 4;break;
	default:
		printf("语法错误：第%d行，没有这种运算符", LL1Line);
		return -1;
	}
}
void process1() {
	signStack.push({ 1, DOT, "DOT" });
	signStack.push({ 0, ProgramBody, "ProgramBody" });
	signStack.push({ 0, DeclarePart, "DeclarePart" });
	signStack.push({ 0, ProgramHead, "ProgramHead" });
}
void process2() {
	signStack.push({ 0, ProgramName, "ProgramName" });
	signStack.push({ 1, PROGRAM, "PROGRAM" });
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = PheadK;//创建程序头类型语法树节点
	TreeNode** t = TreeStack.top();
	TreeStack.pop();
	(*t) = currentP;
}
void process3() {
	signStack.push({ 1,ID,"ID" });
	//当前标识符语义信息写入程序头结点
	currentP->name.push_back(LL1NowToken->Sem);
	currentP->idnum++;
}
void process4() {
	signStack.push({ 0, ProcDec, "ProcDec" });
	signStack.push({ 0, VarDec, "VarDec" });
	signStack.push({ 0, TypeDec, "TypeDec" });
}
void process5() {

}
void process6() {
	signStack.push({ 0,TypeDeclaration,"TypeDeclaration" });
}
//TypeDeclaration>::=TYPE TypeDecList
void process7() {
	signStack.push({ 0,TypeDecList,"TypeDecList" });
	signStack.push({ 1,TYPE,"TYPE" });
	//类型声明标识节点，弹语法树栈得到指针地址
	TreeNode** now = TreeStack.top();
	TreeStack.pop();
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = TypeK;//创建类型声明标志
	(*now) = currentP;
	TreeStack.push(&(currentP->sibling));//兄弟节点和儿子节点压栈
	TreeStack.push(&(currentP->child[0]));
}
void process8() {
	signStack.push({ 0,TypeDecMore,"TypeDecMore" });
	signStack.push({ 1,SEMI,"SEMI" });
	signStack.push({ 0,TypeDef,"TypeDef" });
	signStack.push({ 1,EQ,"EQ" });
	signStack.push({ 0,TypeId,"TypeId" });
	TreeNode** t = TreeStack.top();
	TreeStack.pop();
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = DecK;//声明节点
	(*t) = currentP;
	TreeStack.push(&(currentP->sibling));
}
void process9() {
	TreeStack.pop();
}
void process10() {
	signStack.push({ 0,TypeDecList,"TypeDecList" });

}
void process11() {
	signStack.push({ 1,ID, "ID" });
	currentP->name.push_back(LL1NowToken->Sem);
	currentP->idnum++;
}
void process12() {
	signStack.push({ 0,BaseType,"BaseType" });
	reTypeAdd = &(currentP->kind.dec);//节点上填写标识符类型信息部分的地址

}
void process13() {
	signStack.push({ 0,StructureType,"StructureType" });

}
//类型部分是类型标识符
void process14() {
	signStack.push({ 1,ID, "ID" });
	currentP->kind.dec = IdK;
	currentP->type_name = LL1NowToken->Sem;
}
void process15() {
	signStack.push({ 1,INTEGER,"INTEGER" });
	(*reTypeAdd) = IntegerK;//整数类型
}
void process16() {
	signStack.push({ 1,CHAR,"CHAR" });
	(*reTypeAdd) = CharK;

}
void process17() {
	signStack.push({ 0,ArrayType,"ArrayType" });

}
void process18() {
	signStack.push({ 0,RecType,"RecType" });
}
void process19() {
	signStack.push({ 0, BaseType, "BaseType" });
	signStack.push({ 1, OF, "OF" });
	signStack.push({ 1, RMIDPAREN, "RMIDPAREN" });
	signStack.push({ 0, Top, "Top" });
	signStack.push({ 1, UNDERANGE, "UNDERANGE" });
	signStack.push({ 0, Low, "Low" });
	signStack.push({ 1, LMIDPAREN, "LMIDPAREN" });
	signStack.push({ 1, ARRAY, "ARRAY" });
	(*currentP).kind.dec = ArrayK;
	//**************************************************************????????????????????
	reTypeAdd = &(currentP->attr.ArrayAttr.childtype);
}
void process20() {
	signStack.push({ 1, INTC, "INTC" });
	currentP->attr.ArrayAttr.low = atoi(LL1NowToken->Sem);
}
void process21() {
	signStack.push({ 1, INTC, "INTC" });
	currentP->attr.ArrayAttr.up = atoi(LL1NowToken->Sem);

}
void process22() {
	signStack.push({ 1, END, "END" });
	signStack.push({ 0, FieldDecList, "FieldDecList" });
	signStack.push({ 1, RECORD, "RECORD" });
	currentP->kind.dec = RecordK;
	saveFuncP = currentP;
	TreeStack.push(&(currentP->child[0]));
}
void process23() {
	signStack.push({ 0, FieldDecMore, "FieldDecMore" });
	signStack.push({ 1, SEMI, "SEMI" });
	signStack.push({ 0, IdList, "IDList" });
	signStack.push({ 0, BaseType, "BaseType" });
	TreeNode** now = TreeStack.top();
	TreeStack.pop();
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = DecK;//语句类型节点
	reTypeAdd = (&(currentP->kind.dec));
	(*now) = currentP;
	TreeStack.push(&currentP->sibling);
}
void process24() {
	signStack.push({ 0, FieldDecMore, "FieldDecMore" });
	signStack.push({ 1, SEMI, "SEMI" });
	signStack.push({ 0, IdList, "IDList" });
	signStack.push({ 0, ArrayType, "ArrayType" });
	TreeNode** t = TreeStack.top();
	TreeStack.pop();
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = DecK;//语句类型节点
	(*t) = currentP;
	TreeStack.push(&((*currentP).sibling));

}

void process25() {
	TreeStack.pop();
	currentP = saveFuncP;
}
void process26() {
	signStack.push({ 0,FieldDecList,"FieldDecList" });
}
void process27() {
	signStack.push({ 0, IdMore, "IDMore" });
	signStack.push({ 1, ID, "ID" });
	currentP->name[currentP->idnum] = LL1NowToken->Sem;
	currentP->idnum++;
}
void process28() {}
void process29() {
	signStack.push({ 0, IdList, "IdList" });
	signStack.push({ 1, COMMA, "COMMA" });
}
void process30() {

}
void process31() {
	signStack.push({ 0, VarDeclaration, "VarDeclaration" });
}
void process32() {
	signStack.push({ 0, VarDecList, "VarDecList" });
	signStack.push({ 1, VAR, "VAR" });
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = VarK;//声明类型节点
	TreeNode** now = TreeStack.top();
	TreeStack.pop();
	(*now) = currentP;
	TreeStack.push(&((*currentP).sibling));
	TreeStack.push(&((*currentP).child[0]));
}
//MR-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//处理产生式 <VarDecList> ::= TypeDef VarIdList;VarDecMore
void process33() {
	//将产生式右部非终极符从右至左依次压入符号栈
	signStack.push({ 0,VarDecMore,"VarDecMore" });
	signStack.push({ 1,SEMI,"SEMI" });             //终极符';'
	signStack.push({ 0,VarIdList,"VarIdList" });
	signStack.push({ 0,TypeDef,"TypeDef" });

	TreeNode** now = TreeStack.top();
	TreeStack.pop();

	TreeNode* newnode = new TreeNode();
	LL1InitTreeNode(newnode);
	newnode->nodekind = DecK;

	currentP = newnode;
	(*now) = currentP;
	TreeStack.push(&((*currentP).sibling));
}

//处理产生式 <VarDecMore> ::= NULL
void process34() {    //后面没有变量声明了
	TreeStack.pop();  //弹出栈中指向下一个变量声明的指针，变量声明部分处理结束
}

//处理产生式 <VarDecMore> ::= VarDecList
void process35() {
	signStack.push({ 0,VarDecList,"VarDecList" });  //非终极符入符号栈
}

//处理产生式 <VarIdList> ::= ID VarIdMore
void process36() {
	signStack.push({ 0,VarIdMore,"VarIdMore" });
	signStack.push({ 1,ID,"ID" });                  //终极符ID

	currentP->name.push_back(LL1NowToken->Sem);    //标识符的名字写入当前节点currentP
	currentP->idnum++;                             //当前节点currentP的标识符数量+1
}

//处理产生式 <VarIdMore> ::= NULL
void process37() {//空函数

}

//处理产生式 <VarIdMore> ::= , VarIdList
void process38() {
	signStack.push({ 0,VarIdList,"VarIdList" });  //右部入符号栈
	signStack.push({ 1,COMMA,"COMMA" });          //终极符','
}

//处理产生式 <ProcDec> ::= NULL
void process39() {//空函数

}

//处理产生式 <ProcDec> ::= ProcDeclaration
void process40() {
	signStack.push({ 0,ProcDeclaration,"ProcDeclaration" }); //非终极符入符号栈
}

/*
处理产生式 <ProcDeclaration> :: = PROCEDURE
								  ProcName(ParamList):BaseType ;
								  ProcDecPart
								  ProcBody ProcDecMore
*/
void process41() {
	//将产生式右部符号从右至左压入符号栈
	signStack.push({ 0,ProcDecMore,"ProcDecMore" });
	signStack.push({ 0,ProcBody,"ProcBody" });
	signStack.push({ 0,ProcDecPart,"ProcDecPart" });
	signStack.push({ 1,SEMI,"SEMI" });                 //终极符';'
	//signStack.push({ 0,BaseType,"BaseType" });
	//signStack.push({ 1,COLON,"COLON" });               //终极符':'
	signStack.push({ 1,RPAREN,"RPAREN" });             //终极符')'
	signStack.push({ 0,ParamList,"ParamList" });
	signStack.push({ 1,LPAREN,"LPAREN" });             //终极符'('
	signStack.push({ 0,ProcName,"ProcName" });
	signStack.push({ 1,PROCEDURE,"PROCEDURE" });

	//创建过程头节点 currentP
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = ProcDecK;
	currentP->lineno = LL1NowToken->Lineshow;

	//弹语法树栈，得到t，赋值（*t）指向这个声明节点
	if (!TreeStack.empty()) {
		TreeNode** t = TreeStack.top();
		TreeStack.pop();
		(*t) = currentP;
	}

	// 4. 压入兄弟节点和三个子节点的指针地址到语法树栈
	// 注意：栈是后进先出，需按 sibling -> child3 -> child2 -> child1 的顺序压入 顺序对吗???????????????????????????????????????????
	TreeStack.push(&(currentP->sibling));  // 兄弟节点
	TreeStack.push(&(currentP->child[2])); // 第三个子节点（过程体）
	TreeStack.push(&(currentP->child[1])); // 第二个子节点（声明部分）
	TreeStack.push(&(currentP->child[0])); // 第一个子节点（形参部分）
}

//处理产生式 <ProDecMore> ::= NULL
void process42() {//空函数

}

//处理产生式 <ProDecMore> ::= ProcDeclaration
void process43() {//过程声明，产生式右部入符号栈
	signStack.push({ 0,ProcDeclaration,"ProcDeclaration" });
}

//处理产生式 <ProName> ::= id
void process44() {
	signStack.push({ 1,ID,"ID" });                //终极符入符号栈

	currentP->name.push_back(LL1NowToken->Sem);   //将函数名字写入节点
	currentP->idnum++;                            //标识符个数+1
}

//处理产生式 <ParamList> ::= NULL
void process45() {
	//检查语法树栈是否为空
	if (!TreeStack.empty()) {
		//弹出指向形参部分的指针地址
		TreeStack.pop();
	}
}

//处理产生式 <ParamList> ::= ParamDecList
void process46() {
	signStack.push({ 0,ParamDecList,"ParamDecList" });  //非终极符入符号栈
}

//处理产生式 <ParamDecList> ::= Param ParamMore
void process47() {
	signStack.push({ 0,ParamMore,"ParamMore" });        //非终极符入符号栈
	signStack.push({ 0,Param,"Param" });
}

//处理产生式 <ParamMore> ::= NULL
void process48() {
	//弹出最后一个形参声明的兄弟节点指针
	if (!TreeStack.empty()) {
		TreeStack.pop();
	}

	////恢复当前节点为函数声明节点????????????????????????????????????????????????
	//currentP = saveFuncP;

	////设置temp为函数返回类型信息的成员地址
	//reTypeAdd = &(currentP->kind.dec);
}

//处理产生式 <ParamMore> ::= ; ParamDecList
void process49() {
	signStack.push({ 0,ParamDecList,"ParamDecList" });    //非终极符入符号栈
	signStack.push({ 1,SEMI,"SEMI" });                    //终结符 ';'入符号栈
}

//处理产生式 <Param> ::= TypeDef FormList
void process50() {
	//将产生式右部非终结符压入符号栈（从右到左）
	signStack.push(make_tuple(0, FormList, "FormList"));  //形参列表
	signStack.push(make_tuple(0, TypeDef, "TypeDef"));    //类型定义

	//创建形参声明节点 currentP
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = DecK;                            //声明节点类型
	currentP->attr.ProcAttr.paramt = valparamType;        //标记为值参（非引用）
	currentP->lineno = LL1NowToken->Lineshow;             //设置行号

	//弹语法树栈，得到t，赋值（*t）指向这个声明节点
	if (!TreeStack.empty()) {
		TreeNode** t = TreeStack.top();
		TreeStack.pop();
		(*t) = currentP;
	}

	//压入兄弟节点指针到语法树栈（用于处理下一个形参）
	TreeStack.push(&(currentP->sibling));
}

//处理产生式 <Param> ::= VAR TypeDef FormList
void process51() {
	//将产生式右部符号压入符号栈（从右到左）
	signStack.push(make_tuple(0, FormList, "FormList"));  //形参列表
	signStack.push(make_tuple(0, TypeDef, "TypeDef"));    //类型定义
	signStack.push(make_tuple(1, VAR, "VAR"));            //终结符 'VAR'

	//创建形参声明节点 currentP
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = DecK;                            //声明节点类型
	currentP->attr.ProcAttr.paramt = varparamType;        //标记为变参（引用参数）
	currentP->lineno = LL1NowToken->Lineshow;             //设置行号

	//弹语法树栈，得到t，赋值（*t）指向这个声明节点
	if (!TreeStack.empty()) {
		TreeNode** parentPtr = TreeStack.top();
		TreeStack.pop();
		*parentPtr = currentP;
	}

	//压入兄弟节点指针到语法树栈（用于处理下一个形参）
	TreeStack.push(&(currentP->sibling));
}


//处理产生式 <FormList> ::= ID FidMore
void process52() {
	signStack.push({ 0,FidMore,"FidMore" });
	signStack.push({ 1,ID,"ID" });                 //终极符ID

	currentP->name.push_back(LL1NowToken->Sem);    //标识符的名字写入当前节点currentP
	currentP->idnum++;                             //当前节点currentP的标识符数量+1
}

//处理产生式 <FidMore> ::= NULL
void process53() {//空函数

}

//处理产生式 <FidMore> ::= , FormList
void process54() {
	signStack.push({ 0,FormList,"FormList" });
	signStack.push({ 1,COMMA,"COMMA" });        //终极符','入符号栈
}

//处理产生式 <ProcDecPart> ::= DeclarePart
void process55() {
	signStack.push({ 0,DeclarePart,"DeclarePart" });   //右部非终极符入符号栈
}

//处理产生式 <ProcBody> ::= ProgramBody
void process56() {
	signStack.push({ 0,ProgramBody,"ProgramBody" });   //右部非终极符入符号栈
}

//处理产生式 <ProgramBody> ::= BEGIN StmList END
void process57() {
	//将产生式右部符号从右至左压入符号栈
	signStack.push(make_tuple(1, END, "END"));           //终结符 'END'
	signStack.push(make_tuple(0, StmList, "StmList"));   //非终结符（语句列表）
	signStack.push(make_tuple(1, BEGIN, "BEGIN"));       //终结符 'BEGIN'

	//弹出语法树栈顶多余指针（确保是 child[2] 指向语句序列）
	if (!TreeStack.empty()) {
		TreeStack.pop();
	}
	TreeNode** t = TreeStack.top();
	TreeStack.pop();
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = StmLK;
	(*t) = currentP;
	TreeStack.push(&(currentP->child[0]));
}

//处理产生式 <StmList> ::= Stm StmMore
void process58() {
	signStack.push({ 0,StmMore,"StmMore" });  //右部非终极符入符号栈
	signStack.push({ 0,Stm,"Stm" });
}

//处理产生式 <StmList> ::= NULL
void process59() {//后面没有其他语句了,即没有兄弟节点
	//检查语法树栈是否为空
	if (!TreeStack.empty()) {
		//弹出最后一条语句的兄弟节点指针地址，表示语句序列处理完毕
		TreeStack.pop();
	}
}

//处理产生式 <StmMore> ::= ; StmList
void process60() {
	signStack.push({ 0,StmList,"StmList" });
	signStack.push({ 1,SEMI,"SEMI" });         // 终结符 ';'
}

//处理产生式 <Stm> ::= ConditionalStm
void process61() {
	//将非终结符 ConditionalStm 压入符号栈
	signStack.push(make_tuple(0, ConditionalStm, "ConditionalStm"));

	//创建条件语句节点 currentP
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = StmtK;                 //语句节点类型
	currentP->kind.stmt = IfK;                  //语句具体类型（IfK）
	currentP->lineno = LL1NowToken->Lineshow;   //设置行号

	//弹语法树栈，得到t，赋值（*t）指向这个语句节点
	if (!TreeStack.empty()) {
		TreeNode** parentPtr = TreeStack.top();
		TreeStack.pop();
		*parentPtr = currentP;
	}

	//压入兄弟节点指针（用于处理下一条语句）
	TreeStack.push(&(currentP->sibling));
}

//处理产生式 <Stm> ::= LoopStm
void process62() {
	//将非终结符 LoopStm 压入符号栈
	signStack.push(make_tuple(0, LoopStm, "LoopStm"));

	//创建循环语句节点 currentP
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = StmtK;                 //语句节点类型
	currentP->kind.stmt = WhileK;               //语句具体类型（WhileK）
	currentP->lineno = LL1NowToken->Lineshow;   //设置行号为当前 Token 行号

	//弹语法树栈，得到t，赋值（*t）指向这个语句节点
	if (!TreeStack.empty()) {
		TreeNode** parentPtr = TreeStack.top();
		TreeStack.pop();
		(*parentPtr) = currentP;
	}

	//压入兄弟节点指针（用于处理下一条语句）
	TreeStack.push(&(currentP->sibling));
}

//处理产生式 <Stm> ::= InputStm
void process63() {
	//将非终结符 InputStm 压入符号栈
	signStack.push(make_tuple(0, InputStm, "InputStm"));

	//创建输入语句节点 currentP
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = StmtK;               //语句节点类型
	currentP->kind.stmt = ReadK;              //语句具体类型（ReadK）
	currentP->lineno = LL1NowToken->Lineshow; //设置行号为当前 Token 行号

	//弹语法树栈，得到t，赋值（*t）指向这个语句节点
	if (!TreeStack.empty()) {
		TreeNode** parentPtr = TreeStack.top();
		TreeStack.pop();
		*parentPtr = currentP;
	}

	//压入兄弟节点指针（用于处理下一条语句）
	TreeStack.push(&(currentP->sibling));
}

//处理产生式 <Stm> ::= OutputStm
void process64() {
	//将非终结符 OutputStm 压入符号栈
	signStack.push(make_tuple(0, OutputStm, "OutputStm"));

	//创建输出语句节点 currentP
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = StmtK;                 //语句节点类型
	currentP->kind.stmt = WriteK;               //语句具体类型（WriteK）
	currentP->lineno = LL1NowToken->Lineshow;   //设置行号为当前 Token 行号

	//弹语法树栈，得到t，赋值（*t）指向这个语句节点
	if (!TreeStack.empty()) {
		TreeNode** parentPtr = TreeStack.top();
		TreeStack.pop();
		*parentPtr = currentP;
	}

	//压入兄弟节点指针（用于处理下一条语句）
	TreeStack.push(&(currentP->sibling));
}

//处理产生式 <Stm> ::= ReturnStm
void process65() {
	//将非终结符 ReturnStm 压入符号栈
	signStack.push(make_tuple(0, ReturnStm, "ReturnStm"));

	//创建返回语句节点 currentP
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = StmtK;                 //语句节点类型
	currentP->kind.stmt = ReturnK;              //语句具体类型（ReturnK）
	currentP->lineno = LL1NowToken->Lineshow;   //设置行号为当前 Token 行号

	//弹语法树栈，得到t，赋值（*t）指向这个语句节点
	if (!TreeStack.empty()) {
		TreeNode** parentPtr = TreeStack.top();
		TreeStack.pop();
		*parentPtr = currentP;
	}

	// 压入兄弟节点指针（用于处理下一条语句）
	TreeStack.push(&(currentP->sibling));
}

//处理产生式 <Stm> ::= id AssCall
void process66() {
	//产生式右边进入符号栈
	signStack.push({ 0,AssCall,"AssCall" });
	signStack.push({ 1,ID,"ID" });
	//创建语句节点（类型暂未确定）
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	currentP->nodekind = StmtK;                 //语句节点类型
	currentP->kind.stmt = AssignK;             //初始化为未确定类型 是否要初始化??????????????????????????????????????????????????????
	currentP->lineno = LL1NowToken->Lineshow;   //设置行号

	////创建变量表达式节点（赋值左部或过程名）
	TreeNode* varNode = new TreeNode();
	varNode->nodekind = ExpK;                   //表达式节点
	varNode->kind.exp = VariK;                  //变量类型 
	varNode->attr.ExpAttr.varkind = IdV;        //标识符变量
	varNode->name.push_back(LL1NowToken->Sem);  //记录标识符名称
	varNode->lineno = LL1NowToken->Lineshow;    //设置行号
	varNode->idnum++;                           //标识符数量+1

	////将变量节点链接为语句节点的第一个子节点
	currentP->child[0] = varNode;

	////弹语法树栈，得到t，赋值（*t）指向这个语句节点
	if (!TreeStack.empty()) {
		TreeNode** parentPtr = TreeStack.top();
		TreeStack.pop();
		(*parentPtr) = currentP;
		//
	}

	////压入兄弟节点指针（用于处理下一条语句）
	TreeStack.push(&(currentP->sibling));
}

//处理产生式 <AssCall> ::= AssignmentRest
void process67() {
	//将非终结符 AssignmentRest 压入符号栈
	//signStack.push({ 0, AssignmentRest, "AssignmentRest" });
	////确认当前语句节点为赋值类型（需确保 currentP 已存在且为语句节点）有必要判断吗????????????????????????????????????????????????????????????
	//if (currentP != nullptr && currentP->nodekind == StmtK) {
	//	currentP->kind.stmt = AssignK;  //设置为赋值语句类型
	//}
	//else {
	//	// 错误处理：当前节点无效或非语句节点
	//	// 可记录错误日志或抛出异常
	//}
	signStack.push({ 0, AssignmentRest, "AssignmentRest" });
	currentP->kind.stmt = AssignK;
}

void process68() {
	//将非终结符 CallStmRest 压入符号栈
	signStack.push(make_tuple(0, CallStmRest, "CallStmRest"));
	currentP->child[0]->attr.ExpAttr.varkind = IdV;
	currentP->kind.stmt = CallK;
	////确认当前语句节点为过程调用类型（需确保 currentP 已存在且为语句节点）
	//if (currentP != nullptr && currentP->nodekind == StmtK) {//有必要判断吗??????????????????????????????
	//	currentP->kind.stmt = CallK;  //设置为过程调用语句类型
	//}
	//else {
	//	// 错误处理：当前节点无效或非语句节点
	//	// 可记录错误日志或抛出异常
	//}

	////确保第一个子节点（标识符）类型为变量表达式
	//if (currentP->child[0] != nullptr && currentP->child[0]->nodekind == ExpK) {//有必要判断吗??????????????????????????????
	//	currentP->child[0]->kind.exp = VariK;          // 变量表达式类型 有必要吗？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	//	currentP->child[0]->attr.ExpAttr.varkind = IdV; // 标识符变量
	//}
}
//XYN-------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool getExpResult = true;//表达式判断
int expflag = 0;	//用于计算括号
bool getExpResult2;	//用于数组

void process69() {   //140
	signStack.push({ 0, Exp ,"Exp" });
	signStack.push({ 1, ASSIGN, "ASSIGN" });
	signStack.push({ 0, VariMore, "VariMore" });

	// 1. 将赋值语句节点中指向赋值右部的指针压入语法树栈
	TreeStack.push(&(currentP->child[1]));
	// 2. 改变当前节点指针为赋值左部节点（准备处理左部变量
	currentP = (*currentP).child[0];
	// 3. 压入特殊栈底标志（用于后续处理）
	TreeNode* t = new TreeNode();
	LL1InitTreeNode(t);
	(*t).nodekind = ExpK;
	(*t).lineno = LL1NowToken->Lineshow;
	(*t).kind.exp = OpK;
	(*t).attr.ExpAttr.op = END;
	OpsignStack.push(t);  //////////////////////////////////////////////////
}
//ConditionalStm ::= IF RelExp THEN StmList ELSE StmList FI
void process70() {
	signStack.push({ 1, FI ,"FI" });
	signStack.push({ 0, StmList, "StmList" });
	signStack.push({ 1, ELSE, "ELSE" });
	signStack.push({ 0, StmList, "StmList" });
	signStack.push({ 1, THEN ,"THEN" });
	signStack.push({ 0, RelExp, "RelExp" });
	signStack.push({ 1, IF, "IF" });
	// 2. 按逆序压入三个子节点的标志（先 ELSE，再 THEN，最后 RelExp）
	TreeStack.push(&(*currentP).child[2]);
	TreeStack.push(&(*currentP).child[1]);
	TreeStack.push(&(*currentP).child[0]);
}
//LoopStm ::= WHILE RelExp DO StmList ENDWH
void process71() {
	signStack.push({ 1, ENDWH ,"ENDWH" });
	signStack.push({ 0, StmList, "StmList" });
	signStack.push({ 1, DO, "DO" });
	signStack.push({ 0, RelExp, "RelExp" });
	signStack.push({ 1, WHILE ,"WHILE" });
	// 2. 按逆序压入两个子节点的标志
	TreeStack.push(&(*currentP).child[1]);
	TreeStack.push(&(*currentP).child[0]);
}
//InputStm ::= READ LPAREN InVar RPAREN
void process72() {
	signStack.push({ 1, RPAREN ,"RPAREN" });
	signStack.push({ 0, InVar, "InVar" });
	signStack.push({ 1, LPAREN, "LPAREN" });
	signStack.push({ 1, READ, "READ" });
}
//Invar ::= ID
void process73() {
	signStack.push({ 1, ID ,"ID" });
	// 2. 将标识符名字存入节点的 name 列表
	(*currentP).name.push_back(LL1NowToken->Sem);
	// 3. 增加节点的标识符计数
	(*currentP).idnum++;
}
//OutputStm ::= WRITE LPAREN Exp RPAREN
void process74() {
	signStack.push({ 1, RPAREN ,"RPAREN" });
	signStack.push({ 0, Exp, "Exp" });
	signStack.push({ 1, LPAREN, "LPAREN" });
	signStack.push({ 1, WRITE, "WRITE" });

	TreeStack.push(&(*currentP).child[0]);

	TreeNode* t = new TreeNode();
	LL1InitTreeNode(t);
	(*t).nodekind = ExpK;
	(*t).lineno = LL1NowToken->Lineshow;
	(*t).kind.exp = OpK;
	(*t).attr.ExpAttr.op = END;
	OpsignStack.push(t);/////////////////////////////////////////////////
}
//ReturnStm ::= RETURN
void process75() {
	signStack.push({ 1, RETURN ,"RETURN" });
}
//CallStmRest ::= LPAREN ActParamList RPAREN
void process76() {
	signStack.push({ 1, RPAREN ,"RPAREN" });
	signStack.push({ 0, ActParamList, "ActParamList" });
	signStack.push({ 1, LPAREN, "LPAREN" });
	//第一个孩子
	TreeStack.push(&(*currentP).child[1]);   //????????????????????还是child[1]
}
//ActParamList ::= 
void process77() {
	TreeStack.pop();
}
//ActParamList ::= Exp ActParamMore
void process78() {
	signStack.push({ 0, ActParamMore ,"ActParamMore" });
	signStack.push({ 0, Exp, "Exp" });

	TreeNode* t = new TreeNode();
	LL1InitTreeNode(t);
	(*t).nodekind = ExpK;
	(*t).lineno = LL1NowToken->Lineshow;
	(*t).kind.exp = OpK;
	(*t).attr.ExpAttr.op = END;
	OpsignStack.push(t);/////////////////////////////////////
}
//ActParamMore ::= 
void process79() {

}
//ActParamMore ::= COMMA ActParamList
void process80() {
	signStack.push({ 0, ActParamList ,"ActParamList" });
	signStack.push({ 1, COMMA, "COMMA" });
	//兄弟节点
	TreeStack.push(&(currentP->sibling));
}
//RelExp ::= Exp OtherRelE
void process81() {
	signStack.push({ 0, OtherRelE ,"OtherRelE" });
	signStack.push({ 0, Exp, "Exp" });

	TreeNode* t = new TreeNode();
	LL1InitTreeNode(t);
	(*t).nodekind = ExpK;
	(*t).lineno = LL1NowToken->Lineshow;
	(*t).kind.exp = OpK;
	(*t).attr.ExpAttr.op = END;
	OpsignStack.push(t);/////////////////////////////////////////////////////

	getExpResult = false;
}
//OtherRelE ::= CmpOp Exp
void process82() {
	if (LL1NowToken->Lineshow == 15) {
		int ii = 0;
	}
	signStack.push({ 0, Exp, "Exp" });
	signStack.push({ 0, CmpOp ,"CmpOp" });
	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	(*currentP).nodekind = ExpK;
	(*currentP).lineno = LL1NowToken->Lineshow;
	(*currentP).kind.exp = OpK;
	(*currentP).attr.ExpAttr.op = LL1NowToken->Lex;
	LexType sTop = (*OpsignStack.top()).attr.ExpAttr.op;//???????????????????????????????opstack的size=0
	//栈顶操作符优先级高
	while (Priosity(sTop) >= Priosity(LL1NowToken->Lex)) {
		TreeNode* t = OpsignStack.top();
		OpsignStack.pop();
		TreeNode* Rnum = OpnumStack.top();
		OpnumStack.pop();
		TreeNode* Lnum = OpnumStack.top();
		OpnumStack.pop();
		(*t).child[0] = Lnum;
		(*t).child[1] = Rnum;
		OpnumStack.push(t);
		sTop = (*OpsignStack.top()).attr.ExpAttr.op;
	}
	OpsignStack.push(currentP);

	getExpResult = true;
}
//Exp ::= Term OtherTerm
void process83() {
	signStack.push({ 0, OtherTerm, "OtherTerm" });
	signStack.push({ 0, Term ,"Term" });
}
//OtherTerm ::= 
void process84() {
	if (LL1NowToken->Lex == RPAREN && expflag > 0) {
		LexType sTop = (*OpsignStack.top()).attr.ExpAttr.op;
		while (sTop != LPAREN) {
			TreeNode* t = OpsignStack.top();
			OpsignStack.pop();
			TreeNode* Rnum = OpnumStack.top();
			OpnumStack.pop();
			TreeNode* Lnum = OpnumStack.top();
			OpnumStack.pop();
			(*t).child[0] = Lnum;
			(*t).child[1] = Rnum;
			OpnumStack.push(t);
			sTop = (*OpsignStack.top()).attr.ExpAttr.op;
		}
		OpsignStack.pop();
		expflag--;
	}

	else {
		if (getExpResult || getExpResult2) {
			LexType sTop = (*OpsignStack.top()).attr.ExpAttr.op;
			while (sTop != END) {
				TreeNode* t = OpsignStack.top();
				OpsignStack.pop();
				TreeNode* Rnum = OpnumStack.top();
				OpnumStack.pop();
				TreeNode* Lnum = OpnumStack.top();
				OpnumStack.pop();
				(*t).child[0] = Lnum;
				(*t).child[1] = Rnum;
				OpnumStack.push(t);
				sTop = (*OpsignStack.top()).attr.ExpAttr.op;
			}
			OpsignStack.pop();
			currentP = OpnumStack.top();
			OpnumStack.pop();
			TreeNode** t = TreeStack.top();
			TreeStack.pop();
			(*t) = currentP;
			if (getExpResult2 == true) {
				getExpResult2 = false;
			}
		}
	}
}
//OtherTerm ::= AddOp Exp
void process85() {
	signStack.push({ 0, Exp, "Exp" });
	signStack.push({ 0, AddOp ,"AddOp" });

	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	(*currentP).nodekind = ExpK;
	(*currentP).lineno = LL1NowToken->Lineshow;
	(*currentP).kind.exp = OpK;
	(*currentP).attr.ExpAttr.op = LL1NowToken->Lex;
	LexType sTop = (*OpsignStack.top()).attr.ExpAttr.op;
	//栈顶操作符优先级高
	while (sTop != LPAREN && Priosity(sTop) >= Priosity(LL1NowToken->Lex)) {
		TreeNode* t = OpsignStack.top();
		OpsignStack.pop();
		TreeNode* Rnum = OpnumStack.top();
		OpnumStack.pop();
		TreeNode* Lnum = OpnumStack.top();
		OpnumStack.pop();
		(*t).child[0] = Lnum;
		(*t).child[1] = Rnum;
		OpnumStack.push(t);
		sTop = (*OpsignStack.top()).attr.ExpAttr.op;
	}
	OpsignStack.push(currentP);
}
//Term ::= Factor OtherFactor
void process86() {
	signStack.push({ 0, OtherFactor, "OtherFactor" });
	signStack.push({ 0, Factor ,"Factor" });
}
//OtherFactor ::= 
void process87() {

}
//OtherFactor ::= MultOp Term
void process88() {
	signStack.push({ 0, Term, "Term" });
	signStack.push({ 0, MultOp ,"MultOp" });

	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	(*currentP).nodekind = ExpK;
	(*currentP).lineno = LL1NowToken->Lineshow;
	(*currentP).kind.exp = OpK;
	(*currentP).attr.ExpAttr.op = LL1NowToken->Lex;
	LexType sTop = (*OpsignStack.top()).attr.ExpAttr.op;
	//栈顶操作符优先级高
	while (sTop != LPAREN && Priosity(sTop) >= Priosity(LL1NowToken->Lex)) {
		TreeNode* t = OpsignStack.top();
		OpsignStack.pop();
		TreeNode* Rnum = OpnumStack.top();
		OpnumStack.pop();
		TreeNode* Lnum = OpnumStack.top();
		OpnumStack.pop();
		(*t).child[0] = Lnum;
		(*t).child[1] = Rnum;
		OpnumStack.push(t);
		sTop = (*OpsignStack.top()).attr.ExpAttr.op;

		OpsignStack.push(currentP);
	}

}
//Factor ::= LPAREN Exp RPAREN
void process89() {
	signStack.push({ 1, RPAREN ,"RPAREN" });
	signStack.push({ 0, Exp, "Exp" });
	signStack.push({ 1, LPAREN, "LPAREN" });

	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	(*currentP).nodekind = ExpK;
	(*currentP).lineno = LL1NowToken->Lineshow;
	(*currentP).kind.exp = OpK;
	(*currentP).attr.ExpAttr.op = LPAREN;
	OpsignStack.push(currentP);////////////////////////////////////////
	expflag++;
}
//Factor ::= INTC
void process90() {
	signStack.push({ 1, INTC, "INTC" });
	TreeNode* t = new TreeNode();InitTreeNode(t);
	t->nodekind = ExpK;
	t->kind.exp = ConstK;
	t->attr.ExpAttr.val = atoi((LL1NowToken->Sem));
	OpnumStack.push(t);
}
//Factor ::= Variable
void process91() {
	signStack.push({ 0, Variable, "Variable" });
}
//Variable ::= ID VariMore
void process92() {
	signStack.push({ 0, VariMore ,"VariMore" });
	signStack.push({ 1, ID, "ID" });

	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	(*currentP).nodekind = ExpK;
	(*currentP).lineno = LL1NowToken->Lineshow;
	(*currentP).kind.exp = VariK;
	(*currentP).idnum++;
	(*currentP).name.push_back(LL1NowToken->Sem);
	OpnumStack.push(currentP);
}
//VariMore ::= 
void process93() {
	(*currentP).attr.ExpAttr.varkind = IdV;
}
//VariMore ::= LMIDPAREN Exp RMIDPAREN
void process94() {
	signStack.push({ 1, RMIDPAREN ,"RMIDPAREN" });
	signStack.push({ 0, Exp, "Exp" });
	signStack.push({ 1, LMIDPAREN, "LMIDPAREN" });

	(*currentP).attr.ExpAttr.varkind = ArrayMembV;
	TreeStack.push(&(*currentP).child[0]);

	TreeNode* t = new TreeNode();
	LL1InitTreeNode(t);
	(*t).nodekind = ExpK;
	(*t).lineno = LL1NowToken->Lineshow;
	(*t).kind.exp = OpK;
	(*t).attr.ExpAttr.op = END;
	OpsignStack.push(t);///////////////////////////////////////////////////////
	getExpResult2 = true;
}
//VariMore ::= DOT FieldVar
void process95() {
	signStack.push({ 0, FieldVar ,"FieldVar" });
	signStack.push({ 1, DOT, "DOT" });

	(*currentP).attr.ExpAttr.varkind = FieldMembV;
	TreeStack.push(&(*currentP).child[0]);
}
//FieldVar ::= ID FieldVarMore
void process96() {
	signStack.push({ 0, FieldVarMore ,"FieldVarMore" });
	signStack.push({ 1, ID, "ID" });

	currentP = new TreeNode();
	LL1InitTreeNode(currentP);
	(*currentP).nodekind = ExpK;
	(*currentP).lineno = LL1NowToken->Lineshow;
	(*currentP).kind.exp = VariK;
	(*currentP).idnum++;
	(*currentP).name.push_back(LL1NowToken->Sem);
	TreeNode** p = TreeStack.top();
	TreeStack.pop();
	(*p) = currentP;
}
//FieldVarMore ::= 
void process97() {
	(*currentP).attr.ExpAttr.varkind = IdV;
}
//FieldVarMore ::= LMIDPAREN Exp RMIDPAREN
void process98() {
	signStack.push({ 1, RMIDPAREN ,"RMIDPAREN" });
	signStack.push({ 0, Exp, "Exp" });
	signStack.push({ 1, LMIDPAREN, "LMIDPAREN" });

	(*currentP).attr.ExpAttr.varkind = ArrayMembV;
	TreeStack.push(&(*currentP).child[0]);

	TreeNode* t = new TreeNode();
	LL1InitTreeNode(t);
	(*t).nodekind = ExpK;
	(*t).lineno = LL1NowToken->Lineshow;
	(*t).kind.exp = OpK;
	(*t).attr.ExpAttr.op = END;
	OpsignStack.push(t);///////////////////////////////////////////////
	getExpResult2 = true;
}
//CmpOp ::= LT
void process99() {
	signStack.push({ 1, LT, "LT" });
}
//CmpOp ::= EQ
void process100() {
	signStack.push({ 1, EQ, "EQ" });
}
//AddOp ::= PLUS
void process101() {
	signStack.push({ 1, PLUS, "PLUS" });
}
//AddOp ::= MINUS
void process102() {
	signStack.push({ 1, MINUS, "MINUS" });
}
//MultOp ::= TIMES
void process103() {
	signStack.push({ 1, TIMES, "TIMES" });
}
//MultOp ::= OVER
void process104() {
	signStack.push({ 1, OVER, "OVER" });
}