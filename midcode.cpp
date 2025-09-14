#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include<fstream>
#include "syntaxAnalysis.h"
#include"SemanticAnalysis.h"
#include"total.h"
#include "midcode.h"
using namespace std;
codeFile* firstCode = NULL;//��һ���м����
codeFile* lastCode = NULL;//���һ���м����
//�������displayƫ��
int mainoff;
int tempOffset;//��ʱ�����ı��
int Label = -1;//���
//��
char* stringToChar(string& str) {
	size_t len = str.length();
	char* result = new char[len + 1];
	for (size_t i = 0; i < len; i++) {
		result[i] = str[i];
	}
	result[len] = '\0';
	return result;
}
//�½�һ��ARG�ṹ
ARG* NewTemp(AccessKind now) {
	ARG* t = new ARG();
	t->form = Addrform;
	t->Attr.Addr.name[0] = '\0';
	t->Attr.Addr.dataLevel = -1;
	t->Attr.Addr.dataOff = tempOffset;//ƫ��
	t->Attr.Addr.access= now;
	tempOffset++;
	return t;
}
//�½���ֵ��ARG
ARG* ARGvalue(int now) {
	ARG* t = new ARG();
	t->form = ValueForm;
	t->Attr.value = now;
	return t;
}
//����һ���µı��
int NewLabel() {
	Label++;
	return Label;
}
//�½������ARG
ARG* ARGLabel(int now) {
	ARG* t = new ARG();
	t->form = LabelForm;
	t->Attr.label = now;
	return t;
}
//�½���ַ��ARG
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

//�м����Ĺ���
codeFile* GenCode(Codekind coki,ARG*arg1,ARG*arg2,ARG*arg3) {
	codeFile* now = new codeFile();
	now->onecode.codekind = coki;
	now->onecode.arg1 = arg1;
	now->onecode.arg2 = arg2;
	now->onecode.arg3 = arg3;
	now->next = NULL;
	now->former = NULL;
	if (firstCode == NULL) {
		//������ǰ�����Ĵ����ǵ�һ��
		firstCode = now;
	}
	else {
		lastCode->next = now;
		now->former = lastCode;
	}
	lastCode = now;
	return now;
}
//��codekindת����string
string codekindToString(Codekind kind) {
	return codekindStrings[kind];
}
void printarg(ARG* now) {
	//��ӡARG
	switch (now->form) {
	case ValueForm:fprintf(outfile, "%d", now->Attr.value);break;
	case LabelForm:fprintf(outfile, "%d", now->Attr.label);break;

	case Addrform:
		if (now->Attr.Addr.dataLevel != -1) {
			fprintf(outfile, now->Attr.Addr.name);
		}
		else {
			//����ʱ������ȡֵ-1
			fprintf(outfile, "temp");
			//��ʱ����Ҫ������ʱ�����ı���
			fprintf(outfile, "%d", now->Attr.Addr.dataOff);
		}
		break;
	default:break;
	}
}

//��������м����
void printmidcode(codeFile* head,FILE*outfile) {
	codeFile* now = head;
	int num = 0;
	while (now != NULL) {
		fprintf(outfile, "%d:", num);
		fprintf(outfile, codekindToString(now->onecode.codekind).c_str());//���ȴ�ӡ�м��������
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

//�м��������������
codeFile* GenMidCode(TreeNode* t) {
	TreeNode* now = t->child[1];
	//�������������ڵ�
	while (now != NULL) {
		//�����ǰ�ڵ��ǹ�����������
		if (now->nodekind == ProcDecK) {
			//������Ƿ�������
			if (now->idnum > 0 && now->name.size() > 0 && !now->name[0].empty()) {
				//����һ������Ĺ��������ڵ�
				GenProcDec(now);
			}
			else {
				//�������һ����ǽڵ㣬��������ӽڵ�
				//���������ӽڵ㣬���ҹ�������
				for (int i = 0; i < 3; i++) {
					TreeNode* child = now->child[i];
					if (child != NULL && child->nodekind == ProcDecK) {
						//�����ӽڵ��еĹ�������
						GenProcDec(child);
					}
					//child = child->sibling;
				}
			}
		}
		////�ƶ�����һ���ֵܽڵ�
		now = now->sibling;
	}
	//����������Ĵ���
	ARG* off = ARGvalue(mainoff);
	codeFile* code = GenCode(MENTRY, NULL, NULL, off);
	tempOffset = mainoff + 1;
	GenBody(t->child[2]);
	ARG* sizearg = ARGvalue(tempOffset);
	code->onecode.arg2 = sizearg;
	return firstCode;
}

//���ɹ�����ڡ����ڡ��������м����
void GenProcDec(TreeNode* now) {
	//���ȼ��ڵ��Ƿ�Ϸ�
	if (now == NULL || now->nodekind != ProcDecK) {
		return;
	}
	SymbolTable* in = NULL;
	//����ֱ�Ӵӽڵ��ȡ���ű���
	if (now->table[0] != NULL) {
		in = now->table[0];
	}
	//������ű���Ϊ�գ�����ͨ������������
	else if (now->name.size() > 0) {
		char* procName = stringToChar(now->name[0]);
		//����FindEntry���ҷ��ű���
		FindEntry(procName, true, &in);
		//���½ڵ�ķ��ű�����
		if (in != NULL) {
			now->table[0] = in;
		}
	}
	//����Ƿ�����Ч�ķ��ű���
	if (in == NULL) {
		//����һ���յĹ����������֣�ֱ�ӷ���
		return;
	}
	int nowprocentry = NewLabel();
	in->attrIR.More.ProcAttr.procEntryNum = nowprocentry;
	//���̲�����displayƫ������Ϣ��������Ӧ��ARG�ṹ
	int proclevel = in->attrIR.More.ProcAttr.level;
	ARG* levelARG = ARGvalue(proclevel);
	//�����ڲ����������͵���...���ɹ��̵��м���루Ƕ�׵��ã�
	TreeNode* t = now->child[1];
	while (t != NULL) {
		if (t->nodekind == ProcDecK) {
			GenProcDec(t);
			// ���t�������ӽڵ��Ƿ��й�������----Ϊ��֧��Ƕ�ף���
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
	//display���ƫ����
	int disoff = in->attrIR.More.ProcAttr.spOff;
	//��ʼ����ʱ������ʼ���Ϊ���̻��¼���������һ����ַ
	tempOffset = disoff + proclevel + 1;
	//���t�Ƿ�ΪNULL
	if (now != NULL && now->child[2] != NULL) {
		//genbody������䲿���м����
		GenBody(now->child[2]);
	}
	//�õ����̻��¼��С��������������м����
	int size = tempOffset;
	ARG* sizearg = ARGvalue(size);
	code->onecode.arg2 = sizearg;
	//���ɹ��̳����м����
	GenCode(ENDPROC, NULL, NULL, NULL);
}

//������м�������ɺ���,ѭ����������м���봦��������ÿ�����
void GenBody(TreeNode* now) {
	TreeNode* now1 = now;
	if (now1->nodekind == StmLK) {
		//��������б�ڵ�
		now1 = now1->child[0];
	}
	while (now1 != NULL) {
		GenStatement(now1);
		now1 = now1->sibling;
	}
}
//�������������͵�����Ӧ����䴦����
void GenStatement(TreeNode* now) {
	if (now->kind.stmt == AssignK) {
		//��ֵ���
		GenAssignS(now);
	}
	else if (now->kind.stmt == CallK) {
		//���̵���
		GenCallS(now);
	}
	else if (now->kind.stmt == ReadK) {
		//�����
		GenReadS(now);
	}
	else if (now->kind.stmt == WriteK) {
	//д���
		GenWriteS(now);
	}
	else if (now->kind.stmt == IfK) {
	//�������
		GenIfS(now);
	}
	else if (now->kind.stmt == WhileK) {
	//ѭ�����
		GenWhileS(now);
	}
	else if (now->kind.stmt == ReturnK) {
		//�������ֱ�����ɷ����м����
		GenCode(RETURNC, NULL, NULL, NULL);

	}
	else {
		printf("�м�������ɽ׶γ����﷨���д�û��������䣡");
	}
}
//��ֵ����м��������
void GenAssignS(TreeNode* a) {
	//����ֵ��
	ARG* larg = GenVar(a->child[0]);
	//�����ұ�
	ARG* rarg = GenExpr(a->child[1]);
	GenCode(ASSIG, rarg, larg, NULL);
}

//����midcode����,���﷨���еı����ڵ�ת��Ϊ�м������ʹ�õ�ARG�ṹ
ARG* GenVar(TreeNode* a) {
	if (a == NULL || a->idnum <= 0) {
		printf("����: ���ʽ�ڵ���Ч\n");
		return NULL;
	}
	// ��ȡ������
	std::string varName = a->name[0];
	const char* varNameChar = varName.c_str();
	// �ӷ��ű��в��ұ�����Ϣ
	SymbolTable* entry = NULL;
	if (a->table[0] != NULL) {
		entry = a->table[0];
	}
	else {
		// ���table[0]Ϊ�գ�ͨ�����������ҷ��ű�
		bool found = FindEntry(const_cast<char*>(varNameChar), true, &entry);
		if (!found || entry == NULL) {
			printf("����: ���� %s δ�ڷ��ű����ҵ�\n", varName.c_str());
			return NULL;
		}
	}
	// ���������ĵ�ַ��ARG
	ARG* vararg = ARGAddr(varNameChar, entry->attrIR.More.VarAttr.level,
		entry->attrIR.More.VarAttr.off, entry->attrIR.More.VarAttr.access);
	// ���������ĵ�ַ��ARG
	ARG* varg = NULL;
	switch (a->attr.ExpAttr.varkind) {
	case IdV:
		//�Ǳ�ʶ������
		varg = vararg;
		break;
	case ArrayMembV:
		//�������Ա������Ҫ����Ԫ�ص�ַ
		varg = GenArray(vararg, a, a->attr.ArrayAttr.low, entry->attrIR.idtype->More.ArrayAttr.elemTy->size);
		break;
	case FieldMembV:
		//�����Ա������Ҫ�����ֶε�ַ
		varg = GenField(vararg, a, entry->attrIR.idtype->More.body);
		break;
	default:
		break;
	}
	return varg;
}

//������� Ԫ�ص�ַ = �������ַ + (�±�ֵ-�½�)*Ԫ�ش�С
ARG* GenArray(ARG* v1arg, TreeNode* t, int low, int size) {
	//�����±���ʽ
	ARG* xiabiao = GenExpr(t->child[0]);
	//���������½�ʹ�С��arg�ṹ
	ARG* lowarg = ARGvalue(low);
	ARG* sizee = ARGvalue(size);
   //����������ʱ����,�����м����ָ��
	ARG* temp1 = NewTemp(Dir);
	ARG* temp2 = NewTemp(Dir);
	ARG* temp3 = NewTemp(InDir);
	GenCode(SUB, xiabiao, lowarg, temp1);
	GenCode(MULT, temp1, sizee, temp2);
	GenCode(AADD, v1arg, temp2, temp3);
	return temp3;//һ����ӷ���(InDir)�ĵ�ַ����ʾ����Ԫ�ص����յ�ַ
}
//���Ա����  (record)  �ֶε�ַ = ��¼����ַ + �ֶ�ƫ����
ARG* GenField(ARG* v1arg, TreeNode* t, FieldChain* head) {
	FieldChain* entry = NULL;
	//�������õ�������ƫ��
	FindField(stringToChar(t->child[0]->name[0]), head, &entry);
	int offnow = entry->offset;
	//������ƫ�Ƶ�ARG�ṹ
	ARG* offarg = ARGvalue(offnow);
	ARG* temp1 = NewTemp(InDir);
	GenCode(AADD, v1arg, offarg, temp1);
	TreeNode* pp = t->child[0];
	ARG* FieldV = NULL;
	if (pp->attr.ExpAttr.varkind == ArrayMembV) {
	//��������������,����ֶα�������������
		//�ݹ����GenArray��������Ԫ�ص�ַ
			//ʹ��temp1��Ϊ�µ��������ַ
		int low = pp->attr.ArrayAttr.low;
		int size = entry->unitType->More.ArrayAttr.elemTy->size;
		FieldV = GenArray(temp1, t->child[0], low, size);
	}
	else {
		FieldV = temp1;
	}

	return FieldV;

}
//��lextype���������Ķ�Ӧ
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
//���ʽ(���������������ӱ��ʽ)
ARG* GenExpr(TreeNode* t) {
	ARG* arg = NULL;
	ARG* larg = NULL;
	ARG* rarg = NULL;
	ARG* temp1 = NULL;
	switch (t->kind.exp) {
	case VariK:
		//�������ʽ
		arg = GenVar(t);
		break;
	case ConstK://�������ʽ
		arg = ARGvalue(t->attr.ExpAttr.val);break;
	case OpK:
		//���������ͱ��ʽ
		larg = GenExpr(t->child[0]);
		rarg = GenExpr(t->child[1]);
		//����op���������Ӧ�����м����
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
	//ֱ��ʹ��t��������Ϊ������
	SymbolTable* entry = NULL;
	char* procNameChar = stringToChar(t->name[0]);  // ʹ��t��name��������t->child[0]��name
	//���ҹ��̵ķ��ű���
	FindEntry(procNameChar, true, &entry);
	delete[] procNameChar;

	if (entry == NULL) {
		fprintf(stderr, "Error: Procedure %s not found\n", t->name[0].c_str());
		return;
	}
	// ������� - ��һ��������t->child[0]��������t->child[1]
	TreeNode* t1 = t->child[0];  // ��һ��������t->child[0]
	ARG* Earg = NULL;
	ParamTable* paramm = entry->attrIR.More.ProcAttr.param;

	while (t1 != NULL) {
		Earg = GenExpr(t1);
		ARG* offarg = ARGvalue(paramm->entry->attrIR.More.VarAttr.off);
		if (paramm->entry->attrIR.More.VarAttr.access == Dir) {
			//���β���ֵ��
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
	//�����������
	int paramCount = 0;
	TreeNode* paramNode = t->child[0];
	while (paramNode != NULL) {
		paramCount++;
		paramNode = paramNode->sibling;
	}
	//������������������ARG
	ARG* paramArg = ARGvalue(paramCount);
	//����Ӧ����call 1 
	GenCode(CALL, labelarg, paramArg, nowoffarg); //paramarg�ǲ�������
}


//������м��������read(v1)---(READ,v1)
void GenReadS(TreeNode* now) {
	// ���ڵ���Ч��
	if (now == NULL || now->idnum <= 0) {
		printf("����: ReadS�ڵ���Ч\n");
		return;
	}
	std::string varName = now->name[0];
	char* varNameChar = stringToChar(varName);
	// ͨ�����������ҷ��ű������в㼶�в��ң�
	SymbolTable* entry = NULL;
	bool found = FindEntry(varNameChar, true, &entry);
	if (!found || entry == NULL) {
		printf("����: ���� %s δ�ڷ��ű����ҵ�\n", varName.c_str());
		return;
	}
	ARG* read = ARGAddr(varNameChar, entry->attrIR.More.VarAttr.level,
		entry->attrIR.More.VarAttr.off, entry->attrIR.More.VarAttr.access);
	GenCode(READC, read, NULL, NULL);
}

//��������м����
//(GT, x, 5, t1)      // t1 = x > 5
//(JUMP0, t1, L1, _)  // ���t1Ϊ�٣���ת��L1
//(ADD, x, 1, y)      // then����: y = x + 1
//(JUMP, _, L2, _)    // ��ת��L2
//(LABEL, L1, _, _)   // else��ڱ��
//(SUB, x, 1, y)      // else����: y = x - 1
//(LABEL, L2, _, _)   // if�����ڱ��
void GenIfS(TreeNode* t) {
	//else��ڱ�ź�ARG
	int elselabel = NewLabel();
	ARG* elsearg = ARGLabel(elselabel);
	//if������
	int iflabel = NewLabel();
	ARG* ifarg = ARGLabel(iflabel);
	ARG* earg = GenExpr(t->child[0]);//�����������ʽ�м����
	//������ת���룬������if����ʱ��
	GenCode(JUMP0, earg, elsearg, NULL);
	//����then����
	GenBody(t->child[1]);
	//��ת����,��if����
	GenCode(JUMP, ifarg, NULL, NULL);
	//����else��ڱ��
	GenCode(LABEL, elsearg, NULL, NULL);
	//else�м����
	GenBody(t->child[2]);
	//if������
	GenCode(LABEL, ifarg, NULL, NULL);
}
//д����м����
void GenWriteS(TreeNode* t) {
	ARG* p = GenExpr(t->child[0]);
	GenCode(WRITEC, p, NULL, NULL);
}
//ѭ������м����while (x > 0) do x: = x - 1;
//(WHILESTART, L1, , )        // ���whileѭ����ʼ
//(LABEL, L1, , )             // while��ڱ��
//(GT, x, 0, t1)              // t1 = x > 0
//(JUMP0, t1, L2, )           // ���t1Ϊ�٣���ת��L2
//(SUB, x, 1, x)              // x = x - 1
//(JUMP, L1, , )              // ��ת��while���
//(LABEL, L2, , )             // while���ڱ��
//(ENDWHILE, L2, , )          // ���whileѭ������

void GenWhileS(TreeNode* t) {
	//while��ڱ��
	int inwhile = NewLabel();
	ARG* in = ARGLabel(inwhile);
	//while�������
	GenCode(WHILESTART, in, NULL, NULL);
	//�������ʽ���м����
	ARG* jump = GenExpr(t->child[0]);
	//while����
	int outwhile = NewLabel();
	ARG* out = ARGLabel(outwhile);
	//��������������ת��while����
	GenCode(JUMP0, jump, out, NULL);
	//���������Ĵ���ѭ����
	GenBody(t->child[1]);
	//���ѭ������֮��ת��while��ڼ����´ε�ѭ��
	GenCode(JUMP, in, NULL, NULL);
	//while����
	GenCode(ENDWHILE, out, NULL, NULL);
}
