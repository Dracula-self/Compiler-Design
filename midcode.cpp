#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include<fstream>
#include "syntaxAnalysis.h"
#include"SemanticAnalysis.h"
#include"total.h"
#include "midcode.h"
using namespace std;
codeFile* firstCode = NULL;//第一条中间代码
codeFile* lastCode = NULL;//最后一条中间代码
//主程序的display偏移
int mainoff;
int tempOffset;//临时变量的编号
int Label = -1;//标号
//。
char* stringToChar(string& str) {
	size_t len = str.length();
	char* result = new char[len + 1];
	for (size_t i = 0; i < len; i++) {
		result[i] = str[i];
	}
	result[len] = '\0';
	return result;
}
//新建一个ARG结构
ARG* NewTemp(AccessKind now) {
	ARG* t = new ARG();
	t->form = Addrform;
	t->Attr.Addr.name[0] = '\0';
	t->Attr.Addr.dataLevel = -1;
	t->Attr.Addr.dataOff = tempOffset;//偏移
	t->Attr.Addr.access= now;
	tempOffset++;
	return t;
}
//新建数值类ARG
ARG* ARGvalue(int now) {
	ARG* t = new ARG();
	t->form = ValueForm;
	t->Attr.value = now;
	return t;
}
//产生一个新的标号
int NewLabel() {
	Label++;
	return Label;
}
//新建标号类ARG
ARG* ARGLabel(int now) {
	ARG* t = new ARG();
	t->form = LabelForm;
	t->Attr.label = now;
	return t;
}
//新建地址类ARG
ARG* ARGAddr(const char* id, int level, int off, AccessKind acc) {
	ARG* t = new ARG();
	t->form = Addrform;
	t->Attr.Addr.dataLevel = level;
	t->Attr.Addr.dataOff = off;
	t->Attr.Addr.access = acc;
	strcpy(t->Attr.Addr.name, id);
	return t;

}
//

//中间代码的构建
codeFile* GenCode(Codekind coki,ARG*arg1,ARG*arg2,ARG*arg3) {
	codeFile* now = new codeFile();
	now->onecode.codekind = coki;
	now->onecode.arg1 = arg1;
	now->onecode.arg2 = arg2;
	now->onecode.arg3 = arg3;
	now->next = NULL;
	now->former = NULL;
	if (firstCode == NULL) {
		//表明当前产生的代码是第一条
		firstCode = now;
	}
	else {
		lastCode->next = now;
		now->former = lastCode;
	}
	lastCode = now;
	return now;
}
//把codekind转换成string
string codekindToString(Codekind kind) {
	return codekindStrings[kind];
}
void printarg(ARG* now) {
	//打印ARG
	switch (now->form) {
	case ValueForm:fprintf(outfile, "%d", now->Attr.value);break;
	case LabelForm:fprintf(outfile, "%d", now->Attr.label);break;

	case Addrform:
		if (now->Attr.Addr.dataLevel != -1) {
			fprintf(outfile, now->Attr.Addr.name);
		}
		else {
			//是临时变量，取值-1
			fprintf(outfile, "temp");
			//临时变量要输入临时变量的编码
			fprintf(outfile, "%d", now->Attr.Addr.dataOff);
		}
		break;
	default:break;
	}
}

//输出所有中间代码
void printmidcode(codeFile* head,FILE*outfile) {
	codeFile* now = head;
	int num = 0;
	while (now != NULL) {
		fprintf(outfile, "%d:", num);
		fprintf(outfile, codekindToString(now->onecode.codekind).c_str());//首先打印中间代码的类别
		fprintf(outfile, "     ");
		if (now->onecode.arg1 != NULL) {
			printarg(now->onecode.arg1);
		}
		else fprintf(outfile, "NULL");
		fprintf(outfile, "     ");
		if (now->onecode.arg2 != NULL) {
			printarg(now->onecode.arg2);
		}
		else fprintf(outfile, "NULL");
		fprintf(outfile, "     ");
		if (now->onecode.arg3 != NULL) {
			printarg(now->onecode.arg3);
		}
		else fprintf(outfile, "NULL");
		fprintf(outfile, "     ");
		fprintf(outfile, "\n");
		now = now->next;
		num++;
	}
}

//中间代码生成主函数
codeFile* GenMidCode(TreeNode* t) {
	TreeNode* now = t->child[1];
	//处理所有声明节点
	while (now != NULL) {
		//如果当前节点是过程声明部分
		if (now->nodekind == ProcDecK) {
			//检查它是否有名称
			if (now->idnum > 0 && now->name.size() > 0 && !now->name[0].empty()) {
				//这是一个具体的过程声明节点
				GenProcDec(now);
			}
			else {
				//这可能是一个标记节点，检查它的子节点
				//遍历所有子节点，查找过程声明
				for (int i = 0; i < 3; i++) {
					TreeNode* child = now->child[i];
					if (child != NULL && child->nodekind == ProcDecK) {
						//处理子节点中的过程声明
						GenProcDec(child);
					}
					//child = child->sibling;
				}
			}
		}
		////移动到下一个兄弟节点
		now = now->sibling;
	}
	//生成主程序的代码
	ARG* off = ARGvalue(mainoff);
	codeFile* code = GenCode(MENTRY, NULL, NULL, off);
	tempOffset = mainoff + 1;
	GenBody(t->child[2]);
	ARG* sizearg = ARGvalue(tempOffset);
	code->onecode.arg2 = sizearg;
	return firstCode;
}

//生成过程入口、出口、过程体中间代码
void GenProcDec(TreeNode* now) {
	//首先检查节点是否合法
	if (now == NULL || now->nodekind != ProcDecK) {
		return;
	}
	SymbolTable* in = NULL;
	//尝试直接从节点获取符号表项
	if (now->table[0] != NULL) {
		in = now->table[0];
	}
	//如果符号表项为空，尝试通过过程名查找
	else if (now->name.size() > 0) {
		char* procName = stringToChar(now->name[0]);
		//调用FindEntry查找符号表项
		FindEntry(procName, true, &in);
		//更新节点的符号表引用
		if (in != NULL) {
			now->table[0] = in;
		}
	}
	//检查是否有有效的符号表项
	if (in == NULL) {
		//这是一个空的过程声明部分，直接返回
		return;
	}
	int nowprocentry = NewLabel();
	in->attrIR.More.ProcAttr.procEntryNum = nowprocentry;
	//过程层数和display偏移量信息并构造相应的ARG结构
	int proclevel = in->attrIR.More.ProcAttr.level;
	ARG* levelARG = ARGvalue(proclevel);
	//过程内部还有声明就调用...生成过程的中间代码（嵌套调用）
	TreeNode* t = now->child[1];
	while (t != NULL) {
		if (t->nodekind == ProcDecK) {
			GenProcDec(t);
			// 检查t的所有子节点是否有过程声明----为了支持嵌套！！
			for (int i = 0; i < 3; i++) {
				if (t->child[i] != NULL && t->child[i]->nodekind == ProcDecK) {
					GenProcDec(t->child[i]);
				}
			}
		}
		
		t = t->sibling;
	}
	ARG* arg1 = ARGLabel(nowprocentry);
	codeFile* code = GenCode(PENTRY, arg1, NULL, levelARG);
	//display表的偏移量
	int disoff = in->attrIR.More.ProcAttr.spOff;
	//初始化临时变量开始编号为过程活动记录变量区域第一个地址
	tempOffset = disoff + proclevel + 1;
	//检查t是否为NULL
	if (now != NULL && now->child[2] != NULL) {
		//genbody生成语句部分中间代码
		GenBody(now->child[2]);
	}
	//得到过程活动记录大小回填进入过程入口中间代码
	int size = tempOffset;
	ARG* sizearg = ARGvalue(size);
	code->onecode.arg2 = sizearg;
	//生成过程出口中间代码
	GenCode(ENDPROC, NULL, NULL, NULL);
}

//语句体中间代码生成函数,循环调用语句中间代码处理函数处理每条语句
void GenBody(TreeNode* now) {
	TreeNode* now1 = now;
	if (now1->nodekind == StmLK) {
		//若是语句列表节点
		now1 = now1->child[0];
	}
	while (now1 != NULL) {
		GenStatement(now1);
		now1 = now1->sibling;
	}
}
//根据语句具体类型调用相应的语句处理函数
void GenStatement(TreeNode* now) {
	if (now->kind.stmt == AssignK) {
		//赋值语句
		GenAssignS(now);
	}
	else if (now->kind.stmt == CallK) {
		//过程调用
		GenCallS(now);
	}
	else if (now->kind.stmt == ReadK) {
		//读语句
		GenReadS(now);
	}
	else if (now->kind.stmt == WriteK) {
	//写语句
		GenWriteS(now);
	}
	else if (now->kind.stmt == IfK) {
	//条件语句
		GenIfS(now);
	}
	else if (now->kind.stmt == WhileK) {
	//循环语句
		GenWhileS(now);
	}
	else if (now->kind.stmt == ReturnK) {
		//返回语句直接生成返回中间代码
		GenCode(RETURNC, NULL, NULL, NULL);

	}
	else {
		printf("中间代码生成阶段出错：语法树有错，没有这种语句！");
	}
}
//赋值语句中间代码生成
void GenAssignS(TreeNode* a) {
	//处理赋值左部
	ARG* larg = GenVar(a->child[0]);
	//处理右边
	ARG* rarg = GenExpr(a->child[1]);
	GenCode(ASSIG, rarg, larg, NULL);
}

//变量midcode生成,将语法树中的变量节点转换为中间代码中使用的ARG结构
ARG* GenVar(TreeNode* a) {
	if (a == NULL || a->idnum <= 0) {
		printf("错误: 表达式节点无效\n");
		return NULL;
	}
	// 获取变量名
	std::string varName = a->name[0];
	const char* varNameChar = varName.c_str();
	// 从符号表中查找变量信息
	SymbolTable* entry = NULL;
	if (a->table[0] != NULL) {
		entry = a->table[0];
	}
	else {
		// 如果table[0]为空，通过变量名查找符号表
		bool found = FindEntry(const_cast<char*>(varNameChar), true, &entry);
		if (!found || entry == NULL) {
			printf("错误: 变量 %s 未在符号表中找到\n", varName.c_str());
			return NULL;
		}
	}
	// 创建基本的地址类ARG
	ARG* vararg = ARGAddr(varNameChar, entry->attrIR.More.VarAttr.level,
		entry->attrIR.More.VarAttr.off, entry->attrIR.More.VarAttr.access);
	// 创建基本的地址类ARG
	ARG* varg = NULL;
	switch (a->attr.ExpAttr.varkind) {
	case IdV:
		//是标识符变量
		varg = vararg;
		break;
	case ArrayMembV:
		//是数组成员变量需要计算元素地址
		varg = GenArray(vararg, a, a->attr.ArrayAttr.low, entry->attrIR.idtype->More.ArrayAttr.elemTy->size);
		break;
	case FieldMembV:
		//是域成员变量需要计算字段地址
		varg = GenField(vararg, a, entry->attrIR.idtype->More.body);
		break;
	default:
		break;
	}
	return varg;
}

//数组变量 元素地址 = 数组基地址 + (下标值-下界)*元素大小
ARG* GenArray(ARG* v1arg, TreeNode* t, int low, int size) {
	//处理下标表达式
	ARG* xiabiao = GenExpr(t->child[0]);
	//生成数组下界和大小的arg结构
	ARG* lowarg = ARGvalue(low);
	ARG* sizee = ARGvalue(size);
   //生成三个临时变量,三条中间代码指令
	ARG* temp1 = NewTemp(Dir);
	ARG* temp2 = NewTemp(Dir);
	ARG* temp3 = NewTemp(InDir);
	GenCode(SUB, xiabiao, lowarg, temp1);
	GenCode(MULT, temp1, sizee, temp2);
	GenCode(AADD, v1arg, temp2, temp3);
	return temp3;//一个间接访问(InDir)的地址，表示数组元素的最终地址
}
//域成员变量  (record)  字段地址 = 记录基地址 + 字段偏移量
ARG* GenField(ARG* v1arg, TreeNode* t, FieldChain* head) {
	FieldChain* entry = NULL;
	//查找域表得到域名的偏移
	FindField(stringToChar(t->child[0]->name[0]), head, &entry);
	int offnow = entry->offset;
	//，构造偏移的ARG结构
	ARG* offarg = ARGvalue(offnow);
	ARG* temp1 = NewTemp(InDir);
	GenCode(AADD, v1arg, offarg, temp1);
	TreeNode* pp = t->child[0];
	ARG* FieldV = NULL;
	if (pp->attr.ExpAttr.varkind == ArrayMembV) {
	//若域是数组名字,如果字段本身是数组类型
		//递归调用GenArray计算数组元素地址
			//使用temp1作为新的数组基地址
		int low = pp->attr.ArrayAttr.low;
		int size = entry->unitType->More.ArrayAttr.elemTy->size;
		FieldV = GenArray(temp1, t->child[0], low, size);
	}
	else {
		FieldV = temp1;
	}

	return FieldV;

}
//从lextype到操作符的对应
Codekind midclass(LexType p) {
	switch (p) {
	case PLUS:return ADD;break;
	case MINUS:return SUB;break;
	case TIMES:return MULT;break;
	case LT:return LTC;break;
	case EQ:return EQC;break;
	case OVER:return DIV;break;
	default:break;
	}

}
//表达式(常数、变量、复杂表达式)
ARG* GenExpr(TreeNode* t) {
	ARG* arg = NULL;
	ARG* larg = NULL;
	ARG* rarg = NULL;
	ARG* temp1 = NULL;
	switch (t->kind.exp) {
	case VariK:
		//变量表达式
		arg = GenVar(t);
		break;
	case ConstK://常量表达式
		arg = ARGvalue(t->attr.ExpAttr.val);break;
	case OpK:
		//操作符类型表达式
		larg = GenExpr(t->child[0]);
		rarg = GenExpr(t->child[1]);
		//根据op类别生成相应类别的中间代码
		Codekind now;
		now = midclass(t->attr.ExpAttr.op);
		temp1 = NewTemp(Dir);
		GenCode(now, larg, rarg, temp1);
		arg = temp1;
		break;
	default:break;

	}
	return arg;
}

void GenCallS(TreeNode* t) {
	//直接使用t的名称作为过程名
	SymbolTable* entry = NULL;
	char* procNameChar = stringToChar(t->name[0]);  // 使用t的name，而不是t->child[0]的name
	//查找过程的符号表项
	FindEntry(procNameChar, true, &entry);
	delete[] procNameChar;

	if (entry == NULL) {
		fprintf(stderr, "Error: Procedure %s not found\n", t->name[0].c_str());
		return;
	}
	// 处理参数 - 第一个参数是t->child[0]，而不是t->child[1]
	TreeNode* t1 = t->child[0];  // 第一个参数是t->child[0]
	ARG* Earg = NULL;
	ParamTable* paramm = entry->attrIR.More.ProcAttr.param;

	while (t1 != NULL) {
		Earg = GenExpr(t1);
		ARG* offarg = ARGvalue(paramm->entry->attrIR.More.VarAttr.off);
		if (paramm->entry->attrIR.More.VarAttr.access == Dir) {
			//若形参是值参
			GenCode(VALACT, Earg, offarg, NULL);
		}
		else {
			GenCode(VARACT, Earg, offarg, NULL);
		}
		t1 = t1->sibling;
		paramm = paramm->next;
	}
	if (strcmp(entry->idname, "inner") == 0) { entry->attrIR.More.ProcAttr.procEntryNum = 1; }//////
	int label = entry->attrIR.More.ProcAttr.procEntryNum;
	ARG* labelarg = ARGLabel(label);
	int nowoff = entry->attrIR.More.ProcAttr.spOff;
	ARG* nowoffarg = ARGvalue(nowoff);
	//计算参数个数
	int paramCount = 0;
	TreeNode* paramNode = t->child[0];
	while (paramNode != NULL) {
		paramCount++;
		paramNode = paramNode->sibling;
	}
	//创建包含参数个数的ARG
	ARG* paramArg = ARGvalue(paramCount);
	//这里应该是call 1 
	GenCode(CALL, labelarg, paramArg, nowoffarg); //paramarg是参数个数
}


//读语句中间代码生成read(v1)---(READ,v1)
void GenReadS(TreeNode* now) {
	// 检查节点有效性
	if (now == NULL || now->idnum <= 0) {
		printf("错误: ReadS节点无效\n");
		return;
	}
	std::string varName = now->name[0];
	char* varNameChar = stringToChar(varName);
	// 通过变量名查找符号表（在所有层级中查找）
	SymbolTable* entry = NULL;
	bool found = FindEntry(varNameChar, true, &entry);
	if (!found || entry == NULL) {
		printf("错误: 变量 %s 未在符号表中找到\n", varName.c_str());
		return;
	}
	ARG* read = ARGAddr(varNameChar, entry->attrIR.More.VarAttr.level,
		entry->attrIR.More.VarAttr.off, entry->attrIR.More.VarAttr.access);
	GenCode(READC, read, NULL, NULL);
}

//条件语句中间代码
//(GT, x, 5, t1)      // t1 = x > 5
//(JUMP0, t1, L1, _)  // 如果t1为假，跳转到L1
//(ADD, x, 1, y)      // then部分: y = x + 1
//(JUMP, _, L2, _)    // 跳转到L2
//(LABEL, L1, _, _)   // else入口标号
//(SUB, x, 1, y)      // else部分: y = x - 1
//(LABEL, L2, _, _)   // if语句出口标号
void GenIfS(TreeNode* t) {
	//else入口标号和ARG
	int elselabel = NewLabel();
	ARG* elsearg = ARGLabel(elselabel);
	//if语句出口
	int iflabel = NewLabel();
	ARG* ifarg = ARGLabel(iflabel);
	ARG* earg = GenExpr(t->child[0]);//生成条件表达式中间代码
	//条件跳转代码，不满足if条件时候
	GenCode(JUMP0, earg, elsearg, NULL);
	//处理then部分
	GenBody(t->child[1]);
	//跳转代码,到if出口
	GenCode(JUMP, ifarg, NULL, NULL);
	//处理else入口标号
	GenCode(LABEL, elsearg, NULL, NULL);
	//else中间代码
	GenBody(t->child[2]);
	//if语句出口
	GenCode(LABEL, ifarg, NULL, NULL);
}
//写语句中间代码
void GenWriteS(TreeNode* t) {
	ARG* p = GenExpr(t->child[0]);
	GenCode(WRITEC, p, NULL, NULL);
}
//循环语句中间代码while (x > 0) do x: = x - 1;
//(WHILESTART, L1, , )        // 标记while循环开始
//(LABEL, L1, , )             // while入口标号
//(GT, x, 0, t1)              // t1 = x > 0
//(JUMP0, t1, L2, )           // 如果t1为假，跳转到L2
//(SUB, x, 1, x)              // x = x - 1
//(JUMP, L1, , )              // 跳转回while入口
//(LABEL, L2, , )             // while出口标号
//(ENDWHILE, L2, , )          // 标记while循环结束

void GenWhileS(TreeNode* t) {
	//while入口标号
	int inwhile = NewLabel();
	ARG* in = ARGLabel(inwhile);
	//while入口声明
	GenCode(WHILESTART, in, NULL, NULL);
	//条件表达式的中间代码
	ARG* jump = GenExpr(t->child[0]);
	//while出口
	int outwhile = NewLabel();
	ARG* out = ARGLabel(outwhile);
	//不满足条件的跳转到while出口
	GenCode(JUMP0, jump, out, NULL);
	//满足条件的处理循环体
	GenBody(t->child[1]);
	//这次循环完了之后转到while入口继续下次的循环
	GenCode(JUMP, in, NULL, NULL);
	//while结束
	GenCode(ENDWHILE, out, NULL, NULL);
}
