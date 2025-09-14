#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <stack>
#include "total.h"
#include "betterloop.h"
using namespace std;
//循环不变式外提
//变量定值表
ARG* vartable[200];//存可以外提的变量
int tablenum=0;//变量定值表一共有多少项
////循环信息栈,因为循环可能是嵌套的所以用栈记录嵌套循环的信息表指针
stack<looptable*> stackforloop;
void printvartable(FILE* outfile) {
	for (int i = 0;i < tablenum;i++) {
		if (vartable[i] != NULL) {
			if (vartable[i]->Attr.Addr.dataLevel == -1) {
				fprintf(outfile, "temp");
				fprintf(outfile, "%d", vartable[i]->Attr.Addr.dataOff);
				fprintf(outfile, " ");
			}
			else {
				fprintf(outfile, "%s ", vartable[i]->Attr.Addr.name);
			}
		}
		fprintf(outfile, "\n");
	}
}

codeFile* LoopOpti(codeFile* firstcode, FILE* outfile) {
	//从第一条代码开始循环处理每一条代码直到中间代码结束
	for (int i = 0;i < 200;i++) {
		vartable[i] = NULL;
	}
	codeFile* nowcode = firstcode;
	while (nowcode != NULL) {
		switch (nowcode->onecode.codekind)
		{
		case AADD:
		case ADD:
		case SUB:
		case MULT:
		case DIV:
		case LTC://比较arg1是否小于arg2，结果存入result
		case EQC://比较arg1和arg2是否相等，结果存入result
			//调用addvartable把结果存到变量定值表,这些指令生成新值并存储在结果变量中,需要跟踪这些变量，以便确定哪些变量在循环中被定值
			addvartable(nowcode->onecode.arg3);break;
		case ASSIG:
			//arg2是被赋值的变量(例如：ASSIG a b NULL中的b),需要跟踪哪些变量在循环中被修改
			addvartable(nowcode->onecode.arg2);break;
		case WHILESTART:
			//调用循环入口处理函数
			whileEntry(nowcode);break;
		case ENDWHILE:
			whileEnd(nowcode);break;
		case CALL:
			callcall(nowcode);break;

		default:
			break;
		}
		//printvartable(outfile);
		nowcode = nowcode->next;
	}
	return firstcode;
}
////循环外提处理
//void loopoutside(codeFile* entry) {
//	//代码层数设置为0
//	int level = 0;
//	codeFile* now = entry->next;//入口代码entry之后是循环第一个代码
//	//找循环结束位置
//	looptable* pp = stackforloop.top();
//	codeFile* ee = pp->exitt;
//	//本层循环的变量信息表
//	int vartablenow = pp->varpos;
//	while (now != ee) {
//		if (now->onecode.codekind == WHILESTART) {
//			//是循环入口，层数+1
//			level++;
//		}
//		else if (now->onecode.codekind == ENDWHILE) {
//			//是循环出口，层数-1
//			level--;
//		}
//		else if (now->onecode.codekind == ADD || now->onecode.codekind == SUB || now->onecode.codekind == MULT || now->onecode.codekind == AADD) {
//			if (level == 0) {
//				//找两个操作分量
//				int arg1 = searchvartable(now->onecode.arg1, vartablenow);
//				int arg2 = searchvartable(now->onecode.arg2, vartablenow);
//				if (arg1 < 0 && arg2 < 0) {
//					//两个操作数在本层循环中都没有被定值，外提删掉一项
//					delvartable(now->onecode.arg3, vartablenow);
//					codeFile* next = now->next;codeFile* pre = now->former;
//					pre->next = next;
//					next->former = pre;
//					codeFile* inin = entry->former;
//					inin->next = now;
//					now->former = inin;//插入合适的位置---循环入口的外面
//					now->next = entry;
//					entry->former = now;
//					now = pre;//!!处理下一条
//				}
//				else {
//					addvartable(now->onecode.arg3);
//				}
//			}
//		}
//		now = now->next;
//	}
//}
////循环外提处理
//void loopoutside(codeFile* entry) {
//	// 代码层数设置为0
//	int level = 0;
//	// 找循环结束位置
//	looptable* pp = stackforloop.top();
//	codeFile* ee = pp->exitt;
//	// 本层循环的变量信息表
//	int vartablenow = pp->varpos;
//	// 首先清空变量表中从vartablenow之后的所有条目
//	for (int i = vartablenow; i < tablenum; i++) {
//		vartable[i] = NULL;
//	}
//	tablenum = vartablenow;
//	// 第一遍遍历：收集循环内被修改的变量
//	codeFile* scan = entry->next;
//	while (scan != ee) {
//		if (scan->onecode.codekind == WHILESTART) {
//			level++;
//		}
//		else if (scan->onecode.codekind == ENDWHILE) {
//			level--;
//		}
//		else if (level == 0) { // 只在本层循环处理
//			if (scan->onecode.codekind == ASSIG) {
//				addvartable(scan->onecode.arg2); // 添加被赋值的变量
//			}
//			else if (scan->onecode.codekind == ADD ||
//				scan->onecode.codekind == SUB ||
//				scan->onecode.codekind == MULT ||
//				scan->onecode.codekind == AADD ||
//				scan->onecode.codekind == DIV) {
//				addvartable(scan->onecode.arg3); // 添加运算结果变量
//			}
//		}
//		scan = scan->next;
//	}
//
//	// 重置level并再次遍历，进行外提
//	level = 0;
//	codeFile* now = entry->next;
//	while (now != ee) {
//		if (now->onecode.codekind == WHILESTART) {
//			level++;
//		}
//		else if (now->onecode.codekind == ENDWHILE) {
//			level--;
//		}
//		else if ((now->onecode.codekind == ADD ||
//			now->onecode.codekind == SUB ||
//			now->onecode.codekind == MULT ||
//			now->onecode.codekind == AADD) && level == 0) {
//
//			// 检查两个操作数是否在循环内被修改
//			int arg1 = searchvartable(now->onecode.arg1, vartablenow);
//			int arg2 = searchvartable(now->onecode.arg2, vartablenow);
//
//			// 如果两个操作数都不在循环内被修改，这是一个循环不变式
//			if (arg1 < 0 && arg2 < 0) {
//				// 外提操作
//				delvartable(now->onecode.arg3, vartablenow);
//				codeFile* next = now->next;
//				codeFile* pre = now->former;
//				pre->next = next;
//				next->former = pre;
//				codeFile* inin = entry->former;
//				inin->next = now;
//				now->former = inin;
//				now->next = entry;
//				entry->former = now;
//				now = pre; // 处理下一条
//			}
//		}
//		now = now->next;
//	}
//}
 
//循环外提处理
void loopoutside(codeFile* entry) {
	// 代码层数设置为0
	int level = 0;
	// 找循环结束位置
	looptable* pp = stackforloop.top();
	codeFile* ee = pp->exitt;
	// 本层循环的变量信息表起始位置
	int vartablenow = pp->varpos;
	// 第一遍遍历：从表中删除循环内被修改的变量
	codeFile* scan = entry->next;
	while (scan != ee) {
		if (scan->onecode.codekind == WHILESTART) {
			level++;
		}
		else if (scan->onecode.codekind == ENDWHILE) {
			level--;
		}
		else if (level == 0) { // 只在本层循环处理
			if (scan->onecode.codekind == ASSIG) {
				// 对于赋值指令(ASSIG)：删除被赋值的变量(arg2)
				delvartable(scan->onecode.arg2, vartablenow);
			}
			else if (scan->onecode.codekind == ADD ||
				scan->onecode.codekind == SUB ||
				scan->onecode.codekind == MULT ||
				scan->onecode.codekind == AADD ||
				scan->onecode.codekind == DIV) {
				//对于运算指令(ADD/SUB等)：删除结果变量(arg3)
				delvartable(scan->onecode.arg3, vartablenow);
				
			}
		}
		scan = scan->next;
	}
	// 重置level并再次遍历，进行外提
	level = 0;
	codeFile* now = entry->next;
	while (now != ee) {
		if (now->onecode.codekind == WHILESTART) {
			level++;
		}
		else if (now->onecode.codekind == ENDWHILE) {
			level--;
		}
		else if ((now->onecode.codekind == ADD ||
			now->onecode.codekind == SUB ||
			now->onecode.codekind == MULT ||
			now->onecode.codekind == AADD) && level == 0) {
			// 检查两个操作数是否在表中（即不在循环内被修改）
			int arg1 = searchvartable(now->onecode.arg1, vartablenow);  // 从表头开始查找
			int arg2 = searchvartable(now->onecode.arg2, vartablenow);  // 从表头开始查找
			// 如果两个操作数都在表中（循环外不变量），这是一个可外提的表达式
			if (arg1 >= 0 && arg2 >= 0) {
				// 先记录下一个操作
				codeFile* next = now->next;
				codeFile* pre = now->former;
				// 从循环内删除这条语句
				pre->next = next;
				next->former = pre;
				// 将其插入到循环前
				codeFile* beforeEntry = entry->former;
				beforeEntry->next = now;
				now->former = beforeEntry;
				now->next = entry;
				entry->former = now;
				// 更新当前处理位置为删除操作前的节点
				now = pre;
			}
		}
		now = now->next;
	}
}


//循环入口处理函数
void whileEntry(codeFile* nowcode) {
//创建新的循环信息表空间
	looptable* loopspac = new looptable();
	//循环往外提标志设为可以外提（1）
	loopspac->flagg = 1;
	//填写当前循环变量定值在vardefset的入口位置
	loopspac->varpos = tablenum;
	//循环入口
	loopspac->entr = nowcode;
	//循环信息亚展
	stackforloop.push(loopspac);
}
//循环出口处理
void whileEnd(codeFile* nowcode)
{
	if (stackforloop.empty()) {
		return;
	}
	looptable* now = stackforloop.top();
	if (now->flagg == 1)
	{
		//填写循环出口位置指针
		now->exitt = nowcode;
		//找到循环入口
		codeFile* entry = now->entr;
		//循环外提处理部分
		loopoutside(entry);
	}
	//弹出栈顶元素,本层处理结束
	stackforloop.pop();
}
//过程调用语句的处理函数，包含这个语句的循环都不能做不变式外提
void callcall(codeFile* nowcode) {
		//创建临时栈来进行遍历
		std::stack<looptable*> tempStack = stackforloop;
		while (!tempStack.empty()) {
			looptable* currentLoop = tempStack.top();
			tempStack.pop();
			// 将所有打开的循环设置为不可外提状态
			currentLoop->flagg = 0;
		}
}

//循环变量定值表查找，begin是开始位置
int searchvartable(ARG* now, int begin) {
	if (strcmp(now->Attr.Addr.name, "i") == 0) {
		int i = 0;
	}
	if (now->form == Addrform) {
	//得到变量的层数和偏移
		int level = now->Attr.Addr.dataLevel;
		int off = now->Attr.Addr.dataOff;
		//层数和偏移都相等是查找到了
		for (int i = begin;i < tablenum;i++) {
			if ((vartable[i]->Attr.Addr.dataOff == off) && (vartable[i]->Attr.Addr.dataLevel == level)&& (vartable[i]->Attr.Addr.name != NULL) &&
				(now->Attr.Addr.name != NULL) &&
				(strcmp(vartable[i]->Attr.Addr.name, now->Attr.Addr.name) == 0)) {
				return i;
			}
		}
	}
	return -1;
}
//删掉一项
void delvartable(ARG* now, int begin) {
	int pos = searchvartable(now, begin);
	if (pos != -1) {
		for (int i = pos;i < tablenum;i++) {
			vartable[i] = vartable[i + 1];
		}
		tablenum--;//后面的向前移动
	}
}
//添加一项
void addvartable(ARG* now) {
	int begin = 0;
	//看是不是已经存在了
	if (!stackforloop.empty()) {
		begin = stackforloop.top()->varpos;
	}
	int a = searchvartable(now, begin);
	if (a == -1) { vartable[tablenum++] = now; }
}

