#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <stack>
#include "total.h"
#include "betterloop.h"
using namespace std;
//ѭ������ʽ����
//������ֵ��
ARG* vartable[200];//���������ı���
int tablenum=0;//������ֵ��һ���ж�����
////ѭ����Ϣջ,��Ϊѭ��������Ƕ�׵�������ջ��¼Ƕ��ѭ������Ϣ��ָ��
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
	//�ӵ�һ�����뿪ʼѭ������ÿһ������ֱ���м�������
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
		case LTC://�Ƚ�arg1�Ƿ�С��arg2���������result
		case EQC://�Ƚ�arg1��arg2�Ƿ���ȣ��������result
			//����addvartable�ѽ���浽������ֵ��,��Щָ��������ֵ���洢�ڽ��������,��Ҫ������Щ�������Ա�ȷ����Щ������ѭ���б���ֵ
			addvartable(nowcode->onecode.arg3);break;
		case ASSIG:
			//arg2�Ǳ���ֵ�ı���(���磺ASSIG a b NULL�е�b),��Ҫ������Щ������ѭ���б��޸�
			addvartable(nowcode->onecode.arg2);break;
		case WHILESTART:
			//����ѭ����ڴ�����
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
////ѭ�����ᴦ��
//void loopoutside(codeFile* entry) {
//	//�����������Ϊ0
//	int level = 0;
//	codeFile* now = entry->next;//��ڴ���entry֮����ѭ����һ������
//	//��ѭ������λ��
//	looptable* pp = stackforloop.top();
//	codeFile* ee = pp->exitt;
//	//����ѭ���ı�����Ϣ��
//	int vartablenow = pp->varpos;
//	while (now != ee) {
//		if (now->onecode.codekind == WHILESTART) {
//			//��ѭ����ڣ�����+1
//			level++;
//		}
//		else if (now->onecode.codekind == ENDWHILE) {
//			//��ѭ�����ڣ�����-1
//			level--;
//		}
//		else if (now->onecode.codekind == ADD || now->onecode.codekind == SUB || now->onecode.codekind == MULT || now->onecode.codekind == AADD) {
//			if (level == 0) {
//				//��������������
//				int arg1 = searchvartable(now->onecode.arg1, vartablenow);
//				int arg2 = searchvartable(now->onecode.arg2, vartablenow);
//				if (arg1 < 0 && arg2 < 0) {
//					//�����������ڱ���ѭ���ж�û�б���ֵ������ɾ��һ��
//					delvartable(now->onecode.arg3, vartablenow);
//					codeFile* next = now->next;codeFile* pre = now->former;
//					pre->next = next;
//					next->former = pre;
//					codeFile* inin = entry->former;
//					inin->next = now;
//					now->former = inin;//������ʵ�λ��---ѭ����ڵ�����
//					now->next = entry;
//					entry->former = now;
//					now = pre;//!!������һ��
//				}
//				else {
//					addvartable(now->onecode.arg3);
//				}
//			}
//		}
//		now = now->next;
//	}
//}
////ѭ�����ᴦ��
//void loopoutside(codeFile* entry) {
//	// �����������Ϊ0
//	int level = 0;
//	// ��ѭ������λ��
//	looptable* pp = stackforloop.top();
//	codeFile* ee = pp->exitt;
//	// ����ѭ���ı�����Ϣ��
//	int vartablenow = pp->varpos;
//	// ������ձ������д�vartablenow֮���������Ŀ
//	for (int i = vartablenow; i < tablenum; i++) {
//		vartable[i] = NULL;
//	}
//	tablenum = vartablenow;
//	// ��һ��������ռ�ѭ���ڱ��޸ĵı���
//	codeFile* scan = entry->next;
//	while (scan != ee) {
//		if (scan->onecode.codekind == WHILESTART) {
//			level++;
//		}
//		else if (scan->onecode.codekind == ENDWHILE) {
//			level--;
//		}
//		else if (level == 0) { // ֻ�ڱ���ѭ������
//			if (scan->onecode.codekind == ASSIG) {
//				addvartable(scan->onecode.arg2); // ��ӱ���ֵ�ı���
//			}
//			else if (scan->onecode.codekind == ADD ||
//				scan->onecode.codekind == SUB ||
//				scan->onecode.codekind == MULT ||
//				scan->onecode.codekind == AADD ||
//				scan->onecode.codekind == DIV) {
//				addvartable(scan->onecode.arg3); // �������������
//			}
//		}
//		scan = scan->next;
//	}
//
//	// ����level���ٴα�������������
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
//			// ��������������Ƿ���ѭ���ڱ��޸�
//			int arg1 = searchvartable(now->onecode.arg1, vartablenow);
//			int arg2 = searchvartable(now->onecode.arg2, vartablenow);
//
//			// �������������������ѭ���ڱ��޸ģ�����һ��ѭ������ʽ
//			if (arg1 < 0 && arg2 < 0) {
//				// �������
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
//				now = pre; // ������һ��
//			}
//		}
//		now = now->next;
//	}
//}
 
//ѭ�����ᴦ��
void loopoutside(codeFile* entry) {
	// �����������Ϊ0
	int level = 0;
	// ��ѭ������λ��
	looptable* pp = stackforloop.top();
	codeFile* ee = pp->exitt;
	// ����ѭ���ı�����Ϣ����ʼλ��
	int vartablenow = pp->varpos;
	// ��һ��������ӱ���ɾ��ѭ���ڱ��޸ĵı���
	codeFile* scan = entry->next;
	while (scan != ee) {
		if (scan->onecode.codekind == WHILESTART) {
			level++;
		}
		else if (scan->onecode.codekind == ENDWHILE) {
			level--;
		}
		else if (level == 0) { // ֻ�ڱ���ѭ������
			if (scan->onecode.codekind == ASSIG) {
				// ���ڸ�ֵָ��(ASSIG)��ɾ������ֵ�ı���(arg2)
				delvartable(scan->onecode.arg2, vartablenow);
			}
			else if (scan->onecode.codekind == ADD ||
				scan->onecode.codekind == SUB ||
				scan->onecode.codekind == MULT ||
				scan->onecode.codekind == AADD ||
				scan->onecode.codekind == DIV) {
				//��������ָ��(ADD/SUB��)��ɾ���������(arg3)
				delvartable(scan->onecode.arg3, vartablenow);
				
			}
		}
		scan = scan->next;
	}
	// ����level���ٴα�������������
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
			// ��������������Ƿ��ڱ��У�������ѭ���ڱ��޸ģ�
			int arg1 = searchvartable(now->onecode.arg1, vartablenow);  // �ӱ�ͷ��ʼ����
			int arg2 = searchvartable(now->onecode.arg2, vartablenow);  // �ӱ�ͷ��ʼ����
			// ����������������ڱ��У�ѭ���ⲻ������������һ��������ı��ʽ
			if (arg1 >= 0 && arg2 >= 0) {
				// �ȼ�¼��һ������
				codeFile* next = now->next;
				codeFile* pre = now->former;
				// ��ѭ����ɾ���������
				pre->next = next;
				next->former = pre;
				// ������뵽ѭ��ǰ
				codeFile* beforeEntry = entry->former;
				beforeEntry->next = now;
				now->former = beforeEntry;
				now->next = entry;
				entry->former = now;
				// ���µ�ǰ����λ��Ϊɾ������ǰ�Ľڵ�
				now = pre;
			}
		}
		now = now->next;
	}
}


//ѭ����ڴ�����
void whileEntry(codeFile* nowcode) {
//�����µ�ѭ����Ϣ��ռ�
	looptable* loopspac = new looptable();
	//ѭ���������־��Ϊ�������ᣨ1��
	loopspac->flagg = 1;
	//��д��ǰѭ��������ֵ��vardefset�����λ��
	loopspac->varpos = tablenum;
	//ѭ�����
	loopspac->entr = nowcode;
	//ѭ����Ϣ��չ
	stackforloop.push(loopspac);
}
//ѭ�����ڴ���
void whileEnd(codeFile* nowcode)
{
	if (stackforloop.empty()) {
		return;
	}
	looptable* now = stackforloop.top();
	if (now->flagg == 1)
	{
		//��дѭ������λ��ָ��
		now->exitt = nowcode;
		//�ҵ�ѭ�����
		codeFile* entry = now->entr;
		//ѭ�����ᴦ����
		loopoutside(entry);
	}
	//����ջ��Ԫ��,���㴦�����
	stackforloop.pop();
}
//���̵������Ĵ������������������ѭ��������������ʽ����
void callcall(codeFile* nowcode) {
		//������ʱջ�����б���
		std::stack<looptable*> tempStack = stackforloop;
		while (!tempStack.empty()) {
			looptable* currentLoop = tempStack.top();
			tempStack.pop();
			// �����д򿪵�ѭ������Ϊ��������״̬
			currentLoop->flagg = 0;
		}
}

//ѭ��������ֵ����ң�begin�ǿ�ʼλ��
int searchvartable(ARG* now, int begin) {
	if (strcmp(now->Attr.Addr.name, "i") == 0) {
		int i = 0;
	}
	if (now->form == Addrform) {
	//�õ������Ĳ�����ƫ��
		int level = now->Attr.Addr.dataLevel;
		int off = now->Attr.Addr.dataOff;
		//������ƫ�ƶ�����ǲ��ҵ���
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
//ɾ��һ��
void delvartable(ARG* now, int begin) {
	int pos = searchvartable(now, begin);
	if (pos != -1) {
		for (int i = pos;i < tablenum;i++) {
			vartable[i] = vartable[i + 1];
		}
		tablenum--;//�������ǰ�ƶ�
	}
}
//���һ��
void addvartable(ARG* now) {
	int begin = 0;
	//���ǲ����Ѿ�������
	if (!stackforloop.empty()) {
		begin = stackforloop.top()->varpos;
	}
	int a = searchvartable(now, begin);
	if (a == -1) { vartable[tablenum++] = now; }
}

