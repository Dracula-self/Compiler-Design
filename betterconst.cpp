#define _CRT_SECURE_NO_WARNINGS
#include "total.h"
#include "betterconst.h"
#include <iostream>
using namespace std;
consttable* constTable = NULL;//������ֵ��
codeFile* BLOCK[200];
//�����컮�ֺ�����
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
		//������ʲô���͵Ĵ���ת����ͬ�ĵط����д���
		switch (nowcode->onecode.codekind)
		{
		case WHILESTART:
		case LABEL:
		case PENTRY:
		case MENTRY://��ʱ�򾡵���һ���µĿ�����,��¼�����ܿ���
			BLOCK[num++] = nowcode;break;
		case JUMP:
		case ENDPROC:
		case JUMP0:
		case RETURNC:
		case ENDWHILE://endwhile�Ǵ���һ����俪ʼ�����µĻ�����
			if (nowcode->next != NULL) {
				nowcode = nowcode->next;
				BLOCK[num++] = nowcode;
			}
			break;
		case VARACT://��ǰ�Ǳ�������ҵ���Ӧ�Ĺ��̵��������Ϊ������Ľ���
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
//�������ʽ�Ż�������������DivBaseBlock���м���뻮��Ϊ������
//��ÿ�����������OptiBlock�����Ż�
codeFile* constoptimize(codeFile*firstcode) {
//���ֻ�����
	int blocknum = DivBaseBlock(firstcode);
	//ÿ�������춼���г������ʽ�Ż�
	for (int i = 0;i < blocknum;i++) {
		constTable = NULL;
		OptiBlock(i);
		consttable* pp = NULL;
		while (constTable != NULL) {
		//�ͷſռ�
			pp = constTable;
			constTable = pp->next;
			
			delete(pp);
		}

	}
	return firstcode;
}
//���ض�����������Ż�,�����������е�ÿ��ָ��,����ָ�����ͽ��в�ͬ���� :
//����ָ��ͱȽ�ָ�����ArithC���м���
//��ֵָ����³�����ֵ��
//������ת��������ʣ��滻������
void OptiBlock(int i) {
	codeFile* now = BLOCK[i];//�����õ�����i��ĵ�һ������
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
			//�����Ż�
			isdel = ArithC(now);
			if (isdel) {
			///ɾ����ǰ����
				codeFile* pre = now->former;
				codeFile* next = now->next;
				if (pre != NULL) {
					pre->next = next;
				
				}
				if (next != NULL) {
					next->former = pre;
				}
				delete(now);
				now = pre;//��ǰָ��ָ��֮ǰ��ǰһ��
			}
			break;
		case ASSIG:
			//�Ը�ֵ�ұ߽���ֵ�滻���滻֮���ұ��ǳ����ͼ��볣����ָ��
			Substiarg(now, 1);
			arg1 = now->onecode.arg1;
			arg2 = now->onecode.arg2;
			//���볣����ֵ��
			if (arg1->form == ValueForm) {
				addconsttable(arg2, arg1->Attr.value);
			}
			else {
			//����ɾ����ֵ��߱����Ķ�ֵ
				delconsttable(arg2);
			}
			break;
		case JUMP0:
		case WRITEC:
			//�Ե�һ����������ֵ�滻
			Substiarg(now, 1);
			break;
		case AADD:
			//�Եڶ�����������ֵ�滻
			Substiarg(now, 2);
			break;
		default:
			break;
		}
		//printConsttable(i);//�����ǰ������ֵ��
		now = now->next;
	}
}
//�������͹�ϵ�ȽϽ��д������������1��2����ֵ�滻�����ǳ�����ѽ��д�볣����ֵ����ɾ����Ԫʽ
int ArithC(codeFile* code) {
	int res=0;
//�ȶԷ���1����ֵ�滻
	Substiarg(code, 1);
	ARG* arg1 = code->onecode.arg1;
	Substiarg(code, 2);
	ARG* arg2 = code->onecode.arg2;
	ARG* arg3 = code->onecode.arg3;
	if ((arg1->form == ValueForm) && (arg2->form == ValueForm)) {
	//���߶���ֵ
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
		//������д�볣����ֵ��
		addconsttable(arg3, res);
		res = 1;
	}
	return res;	
}
////�����ó����滻����,���ָ���������Ƿ�Ϊ������ʽ
//����ǣ����Ҹñ����ڳ�����ֵ���е�ֵ
//�ҵ��򴴽��µĳ����������滻ԭ������
void Substiarg(codeFile* code, int i) {
	ARG* argnow = NULL;
	consttable* entry = NULL;
	argnow = (i == 1) ? code->onecode.arg1 : code->onecode.arg2;
	//�ǵ�ַ��ʽ��AddrForm����ζ����������һ���������ڴ�λ��
	if (argnow->form == Addrform) {
	//���ҵ�ַ�ڳ�����ֵ������û�ж�Ӧ�ĳ���ֵ,�������ֵ�滻
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
//������ֵ��鿴��ǰ�����Ƿ���ֵ
int findconst(ARG* arg, consttable** entry) {
	int isin = 0;
	consttable* now = constTable;
	while ((now != NULL) && (isin == 0)) {
		//�����ж�������ȷ����ȫƥ��
		if ((now->var->Attr.Addr.dataLevel == arg->Attr.Addr.dataLevel) &&
			(now->var->Attr.Addr.dataOff == arg->Attr.Addr.dataOff) &&
			//����б�������Ҳ�Ƚϱ�����
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
//�ѱ����ͳ���ֵд�볣����ֵ��
void addconsttable(ARG* arg,int res) {
	//Ҫ���Ѿ����˾͸ı��Ѿ��е�ֵ
	consttable* entry = NULL;
	int isin = findconst(arg, &entry);
	if (isin) {
		entry->constvalue = res;//�Ѿ����˾͸ı�
	}
	else {//��û�о��½�һ��
		consttable* now = new consttable();
		now->constvalue = res;
		now->var = arg;
		now->pre = NULL;
		now->next = NULL;
		//Ȼ����볣����ֵ��
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
//ɾ��������ֵ���е�arg
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
//�����ǰ�ĳ�����ֵ��
void printConsttable(int i) {
	consttable* pp = constTable;
	fprintf(outfile,"��ǰ�ĳ�����ֵ��\n");
	fprintf(outfile, "%d%s", i, ":");
	while (pp != NULL) {
		fprintf(outfile,"(");
		if (pp->var->Attr.Addr.dataLevel != -1) {
			//��ǰ����ͨ����
			fprintf(outfile, "%d", pp->var->Attr.Addr.dataOff);
		}
		else {
	    //��ǰ����ʱ����
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

