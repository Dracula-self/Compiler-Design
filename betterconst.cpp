#define _CRT_SECURE_NO_WARNINGS
#include "total.h"
#include "betterconst.h"
#include <iostream>
using namespace std;
consttable* constTable = NULL;//常量定值表
codeFile* BLOCK[200];
//基本快划分函数，
int DivBaseBlock(codeFile*firstCode) {
	int num = 0;
	for (int i = 0;i < 200;i++) {
		BLOCK[i] = NULL;
	}
	codeFile* nowcode = firstCode;
	BLOCK[num++] = nowcode;
	if (nowcode->next != NULL) {
		nowcode = nowcode->next;
	}
	while (nowcode != NULL) {
		//根据是什么类型的代码转到不同的地方进行处理
		switch (nowcode->onecode.codekind)
		{
		case WHILESTART:
		case LABEL:
		case PENTRY:
		case MENTRY://这时候尽到了一个新的块里面,记录进入总块数
			BLOCK[num++] = nowcode;break;
		case JUMP:
		case ENDPROC:
		case JUMP0:
		case RETURNC:
		case ENDWHILE://endwhile是从下一个语句开始进入新的基本快
			if (nowcode->next != NULL) {
				nowcode = nowcode->next;
				BLOCK[num++] = nowcode;
			}
			break;
		case VARACT://当前是变参数，找到对应的过程调用语句作为基本快的结束
			nowcode = nowcode->next;
			while (nowcode->onecode.codekind != CALL) {
				nowcode = nowcode->next;
			}
			if (nowcode->next != NULL) {
				nowcode = nowcode->next;
				BLOCK[num++] = nowcode;
			}

		default:
			break;
		}
		nowcode = nowcode->next;
	}
	return num;	
}
//常量表达式优化主函数，调用DivBaseBlock将中间代码划分为基本块
//对每个基本块调用OptiBlock进行优化
codeFile* constoptimize(codeFile*firstcode) {
//划分基本快
	int blocknum = DivBaseBlock(firstcode);
	//每个基本快都进行常量表达式优化
	for (int i = 0;i < blocknum;i++) {
		constTable = NULL;
		OptiBlock(i);
		consttable* pp = NULL;
		while (constTable != NULL) {
		//释放空间
			pp = constTable;
			constTable = pp->next;
			
			delete(pp);
		}

	}
	return firstcode;
}
//对特定基本块进行优化,遍历基本块中的每条指令,根据指令类型进行不同处理 :
//算术指令和比较指令：调用ArithC进行计算
//赋值指令：更新常量定值表
//条件跳转和数组访问：替换操作数
void OptiBlock(int i) {
	codeFile* now = BLOCK[i];//这样得到的是i块的第一条代码
	ARG* arg1;ARG* arg2;
	int isdel = 0;
	while ( (now != NULL)&&(now != BLOCK[i + 1]) ) {
		switch (now->onecode.codekind)
		{
		case ADD:
		case SUB:
		case MULT:
		case DIV:
		case LTC:
		case EQC:
			//运算优化
			isdel = ArithC(now);
			if (isdel) {
			///删除当前代码
				codeFile* pre = now->former;
				codeFile* next = now->next;
				if (pre != NULL) {
					pre->next = next;
				
				}
				if (next != NULL) {
					next->former = pre;
				}
				delete(now);
				now = pre;//当前指针指向之前的前一个
			}
			break;
		case ASSIG:
			//对赋值右边进行值替换，替换之后右边是常数就加入常量定指标
			Substiarg(now, 1);
			arg1 = now->onecode.arg1;
			arg2 = now->onecode.arg2;
			//加入常量定值表
			if (arg1->form == ValueForm) {
				addconsttable(arg2, arg1->Attr.value);
			}
			else {
			//否则删除赋值左边变量的定值
				delconsttable(arg2);
			}
			break;
		case JUMP0:
		case WRITEC:
			//对第一个分量进行值替换
			Substiarg(now, 1);
			break;
		case AADD:
			//对第二个分量进行值替换
			Substiarg(now, 2);
			break;
		default:
			break;
		}
		//printConsttable(i);//输出当前常量定值表
		now = now->next;
	}
}
//对算数和关系比较进行处理，对运算分量1和2进行值替换，都是常数则把结果写入常量定值表，并删除四元式
int ArithC(codeFile* code) {
	int res=0;
//先对分量1进行值替换
	Substiarg(code, 1);
	ARG* arg1 = code->onecode.arg1;
	Substiarg(code, 2);
	ARG* arg2 = code->onecode.arg2;
	ARG* arg3 = code->onecode.arg3;
	if ((arg1->form == ValueForm) && (arg2->form == ValueForm)) {
	//两边都是值
		int val1 = arg1->Attr.value;
		int val2 = arg2->Attr.value;
		switch (code->onecode.codekind)
		{
		case ADD:res = val1 + val2;break;
		case SUB:res = val1 - val2;break;
		case MULT:res = val1 * val2;break;
		case DIV:res = val1 / val2;break;
		case LTC:res =( val1 < val2) ? 1 : 0;break;
		case EQC:res = (val1 == val2) ? 1 : 0;break;
		default:
			break;
		}
		//运算结果写入常量定值表
		addconsttable(arg3, res);
		res = 1;
	}
	return res;	
}
////尝试用常量替换变量,检查指定操作数是否为变量形式
//如果是，查找该变量在常量定值表中的值
//找到则创建新的常量操作数替换原操作数
void Substiarg(codeFile* code, int i) {
	ARG* argnow = NULL;
	consttable* entry = NULL;
	argnow = (i == 1) ? code->onecode.arg1 : code->onecode.arg2;
	//是地址形式（AddrForm）意味着它引用了一个变量或内存位置
	if (argnow->form == Addrform) {
	//查找地址在常量定值表中有没有对应的常量值,有则进行值替换
		if (findconst(argnow, &entry)) {
			ARG*pp = new ARG();
			pp->Attr.value = entry->constvalue;
			pp->form = ValueForm;
			if (i == 1) { code->onecode.arg1 = pp; }
			else {
				code->onecode.arg2 = pp;
			}

		}
	}
}
//常量定值表查看当前变量是否有值
int findconst(ARG* arg, consttable** entry) {
	int isin = 0;
	consttable* now = constTable;
	while ((now != NULL) && (isin == 0)) {
		//增加判断条件，确保完全匹配
		if ((now->var->Attr.Addr.dataLevel == arg->Attr.Addr.dataLevel) &&
			(now->var->Attr.Addr.dataOff == arg->Attr.Addr.dataOff) &&
			//如果有变量名，也比较变量名
			(now->var->Attr.Addr.name == NULL && arg->Attr.Addr.name == NULL ||
				now->var->Attr.Addr.name != NULL && arg->Attr.Addr.name != NULL &&
				strcmp(now->var->Attr.Addr.name, arg->Attr.Addr.name) == 0)) {
			isin = 1;
			(*entry) = now;
		}
		now = now->next;
	}
	return isin;
}
//把变量和常量值写入常量定值表
void addconsttable(ARG* arg,int res) {
	//要是已经有了就改变已经有的值
	consttable* entry = NULL;
	int isin = findconst(arg, &entry);
	if (isin) {
		entry->constvalue = res;//已经有了就改变
	}
	else {//还没有就新建一项
		consttable* now = new consttable();
		now->constvalue = res;
		now->var = arg;
		now->pre = NULL;
		now->next = NULL;
		//然后加入常量定值表
		if (constTable == NULL) {
			constTable = now;
		}
		else {
			consttable* pp = constTable;
			while (pp->next != NULL) {
				pp = pp->next;
			}
			pp->next = now;
			now->pre = pp;
		}
	}
}
//删除常量定值表中的arg
void delconsttable(ARG* arg) {
	consttable* entry = NULL;
	int isin = findconst(arg, &entry);
	if (isin == 0) { return; }
	else {
		consttable* pp = entry->pre;
		consttable* qq = entry->next;
		pp->next = qq;
		qq->pre = pp;
		delete(entry);
	}
}
//输出当前的常量定值表
void printConsttable(int i) {
	consttable* pp = constTable;
	fprintf(outfile,"当前的常量定值表：\n");
	fprintf(outfile, "%d%s", i, ":");
	while (pp != NULL) {
		fprintf(outfile,"(");
		if (pp->var->Attr.Addr.dataLevel != -1) {
			//当前是普通变量
			fprintf(outfile, "%d", pp->var->Attr.Addr.dataOff);
		}
		else {
	    //当前是临时变量
			fprintf(outfile,"temp");
			fprintf(outfile,"%d", pp->var->Attr.Addr.dataOff);
		}
		fprintf(outfile,",");
		fprintf(outfile,"%d", pp->constvalue);
		fprintf(outfile,")");
		fprintf(outfile," ");
		pp = pp->next;
	}
	fprintf(outfile,"\n");
}

