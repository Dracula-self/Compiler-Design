#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<fstream>
#include"SemanticAnalysis.h"

using namespace std;
const int initoff = 7;//��ʼƫ����

SymbolTable symbolTableStack[1000];//���ű�ջ
int NowTableLevel = 0;//��ǰ���ű����,���ű������1��ʼ����nowlevel==0��֤����ǰû�з��ű�
int semanticError = 0;//��¼��������������

int off = 0, off1 = 0;

/*---------------------------------------------------------���ű�---------------------------------------------------------------*/
//1.����һ�����ű� ƫ��������newTable->attrIR.More.VarAttr.off
void CreateTable(void) {
    off1 = off;
    off = 0;
    NowTableLevel++;

    //////////////////////////////////////////////////////////
    symbolTableStack[NowTableLevel].next = nullptr;
}

//2.����һ�����ű�
void DestroyTable() {
    //���ջ�Ƿ�Ϊ��
    if (NowTableLevel == 0) {
        cout << "�������: �ݻٷ��ű�ʧ�ܣ���ǰû�з��ű�" << endl;
        if (outfile) {
            fprintf(outfile, "�������: �ݻٷ��ű�ʧ�ܣ���ǰû�з��ű�\n");
        }
        semanticError++;
        return;
    }

    //��ȡ��ǰ���ű�
    SymbolTable* currentTable = symbolTableStack[NowTableLevel].next;

    symbolTableStack[NowTableLevel].next = nullptr; //������ǰ���ű�

    off = off1;

    while (currentTable != nullptr) {
        SymbolTable* temp = currentTable;
        currentTable = currentTable->next;
        delete temp; // �ͷ��ڴ�
    }

    // ��ӡ��ǰ���������ڵ��ԣ�//////////////////////////////////////////////////////////////////////////////////////////
    cout << "�ݻٵķ��ű��ڵ�" << NowTableLevel << "��" << endl;
    if (outfile) {
        fprintf(outfile, "�ݻٵķ��ű��ڵ�%d��\n", NowTableLevel);
    }

    NowTableLevel--;//������һ
}

//3.�ǼǱ�ʶ�������Ե����ű�  ��TreeNodeSymbol Type* table[10]����???????????????????????????????????????????
bool Enter(char* Id, AttributeIR* AttribP, SymbolTable** Entry, TreeNode* t) {
    //���� FindEntry ���ҵ�ǰ���ű�����ʶ���Ƿ��Ѵ��ڣ�����������������������������������������Ϊʲô�������з��ű�
    bool present = FindEntry(Id, false, Entry);

    //�����ʶ���Ѵ��ڣ����ش���
    if (present) {
        cout << "����: ��ʶ��'" << Id << "'�ظ����� -Enter" << endl;
        if (outfile) {
            fprintf(outfile, "����: ��ʶ��'%s'�ظ����� -Enter\n", Id);
        }
        semanticError++;
        return false;
    }
    else {
        //����һ���µķ��ű���
        SymbolTable* newEntry = new SymbolTable();
        strcpy_s(newEntry->idname, Id);//���Ʊ�ʶ������
        newEntry->attrIR = *AttribP;//���Ʊ�ʶ������
        newEntry->next = nullptr;//��ʼ�� next ָ��

        //���·��ű�����뵱ǰ���ű�β��
        if (symbolTableStack[NowTableLevel].next == nullptr) {
            //�����ǰ���ű�Ϊ�գ������Ϊ��һ���ڵ�
            symbolTableStack[NowTableLevel].next = newEntry;
        }
        else {
            //���򣬱��������ű�β������������
            SymbolTable* currentTable = symbolTableStack[NowTableLevel].next;
            while (currentTable->next != nullptr) {
                currentTable = currentTable->next;
            }
            currentTable->next = newEntry;
        }

        // �������﷨���ڵ�
        if (t && t->idnum < 10) {
            t->table[t->idnum - 1] = newEntry;
        }

        *Entry = newEntry;

        //���سɹ�
        std::cout << "��ʶ��'" << Id << "'�ɹ�������ű� -Enter" << endl;
        if (outfile) {
            fprintf(outfile, "��ʶ��'%s'�ɹ�������ű� -Enter\n", Id);
        }
        return true;
    }
}

//4.���ű��в��ұ�ʶ��
//flag=true:�����з��ű��в��ң�flag=false:�ڵ�ǰ���ű��в���
bool FindEntry(char* id, bool flag, SymbolTable** Entry) {
    if (flag) {// �����з��ű��в���
        for (int i = NowTableLevel; i > 0; i--) {
            if (SearchOneTable(id, i, Entry)) {
                return true;
            }
        }
        *Entry = nullptr;
        return false; // ���δ�ҵ������� false
    }
    else {// ���ڵ�ǰ���ű����
        return SearchOneTable(id, NowTableLevel, Entry);
    }
    *Entry = nullptr;
    return false;
}

//5.������в�������
bool FindField(char* Id, FieldChain* head, FieldChain** Entry) {
    FieldChain* current = head; //�����ͷ��ʼ����

    // �������
    while (current != nullptr) {
        //�Ƚϵ�ǰ�ڵ�ı�ʶ��������Ŀ��Id
        if (strcmp(current->idname, Id) == 0) {
            //����ҵ�ƥ��ı�ʶ��
            *Entry = current; //����Entryָ���ҵ��������
            return true; //����true��ʾ���ҳɹ�
        }
        current = current->next; //�ƶ�����һ���ڵ�
    }

    //��������������δ�ҵ�ƥ��ı�ʶ��
    //if (Entry != nullptr) {
    //    *Entry = nullptr; // ����EntryΪ��
    //}
    return false; // ����false��ʾ����ʧ��
}

//6.��ӡ���ű�
void PrintSymbTable(const string& path) {
    ofstream output;
    output.open(path, ios::app);
    if (!output.is_open()) {
        printf("�޷����ļ�: %s -PrintSymbTable\n", path.c_str());
        if (outfile) {
            fprintf(outfile, "�޷����ļ�: %s -PrintSymbTable\n", path.c_str());
        }
        return;
    }

    // ͬʱ��ӡ������̨���ļ�
    cout << "��ʼ��ӡ���ű�..." << endl;
    if (outfile) {
        fprintf(outfile, "��ʼ��ӡ���ű�...\n");
    }

    // �ӵ�һ�㿪ʼ�����δ�ӡ������ű�
    for (int level = 1; level <= NowTableLevel; level++) {
        SymbolTable* currentEntry = symbolTableStack[level].next; // ��ȡ��ǰ��ķ��ű�

        // ������ļ�
        output << "��" << level << "����ű�:" << endl;

        // ���������̨��outfile
        cout << "��" << level << "����ű�:" << endl;
        if (outfile) {
            fprintf(outfile, "��%d����ű�:\n", level);
        }

        // ������ǰ��ķ��ű�
        ParamTable* param = nullptr;
        while (currentEntry != nullptr) {
            // ���� IdKind �����ӡ
            switch (currentEntry->attrIR.kind) {
            case typeKind: // ���ͷ��ű�
                output << "------------���ͷ��ű�------------" << endl;
                output << "������: " << currentEntry->idname << "   ��������: " << currentEntry->attrIR.idtype->kind << "   ���ʹ�С: " << currentEntry->attrIR.idtype->size << endl;

                cout << "------------���ͷ��ű�------------" << endl;
                cout << "������: " << currentEntry->idname << "   ��������: " << currentEntry->attrIR.idtype->kind << "   ���ʹ�С: " << currentEntry->attrIR.idtype->size << endl;

                if (outfile) {
                    fprintf(outfile, "------------���ͷ��ű�------------\n");
                    fprintf(outfile, "������: %s   ��������: %d   ���ʹ�С: %d\n",
                        currentEntry->idname,
                        currentEntry->attrIR.idtype->kind,
                        currentEntry->attrIR.idtype->size);
                }
                break;

            case varKind: // �������ű�
                output << "------------�������ű�------------" << endl;
                output << "������: " << currentEntry->idname << "   ���ʷ�ʽ: " << currentEntry->attrIR.More.VarAttr.access << "   �㼶: " << currentEntry->attrIR.More.VarAttr.level << "   ƫ����: " << currentEntry->attrIR.More.VarAttr.off << "   ����: " << currentEntry->attrIR.idtype->kind << endl;

                cout << "------------�������ű�------------" << endl;
                cout << "������: " << currentEntry->idname << "   ���ʷ�ʽ: " << currentEntry->attrIR.More.VarAttr.access << "   �㼶: " << currentEntry->attrIR.More.VarAttr.level << "   ƫ����: " << currentEntry->attrIR.More.VarAttr.off << "   ����: " << currentEntry->attrIR.idtype->kind << endl;

                if (outfile) {
                    fprintf(outfile, "------------�������ű�------------\n");
                    fprintf(outfile, "������: %s   ���ʷ�ʽ: %d   �㼶: %d   ƫ����: %d   ����: %d\n",
                        currentEntry->idname,
                        currentEntry->attrIR.More.VarAttr.access,
                        currentEntry->attrIR.More.VarAttr.level,
                        currentEntry->attrIR.More.VarAttr.off,
                        currentEntry->attrIR.idtype->kind);
                }
                break;

            case procKind: // ���̷��ű�
                output << "------------���̷��ű�------------" << endl;
                output << "������: " << currentEntry->idname << "   �㼶: " << currentEntry->attrIR.More.ProcAttr.level << "   ������: ";

                cout << "------------���̷��ű�------------" << endl;
                cout << "������: " << currentEntry->idname << "   �㼶: " << currentEntry->attrIR.More.ProcAttr.level << "   ������: ";

                if (outfile) {
                    fprintf(outfile, "------------���̷��ű�------------\n");
                    fprintf(outfile, "������: %s   �㼶: %d   ������: ",
                        currentEntry->idname,
                        currentEntry->attrIR.More.ProcAttr.level);
                }

                param = currentEntry->attrIR.More.ProcAttr.param;
                while (param != nullptr) {
                    output << param->entry << " ";
                    cout << param->entry << " ";
                    if (outfile) {
                        fprintf(outfile, "%p ", param->entry);
                    }
                    param = param->next;
                }

                output << "   �����ַ: " << currentEntry->attrIR.More.ProcAttr.code << "   �ռ��С: " << currentEntry->attrIR.More.ProcAttr.size << endl;

                cout << "   �����ַ: " << currentEntry->attrIR.More.ProcAttr.code << "   �ռ��С: " << currentEntry->attrIR.More.ProcAttr.size << endl;

                if (outfile) {
                    fprintf(outfile, "   �����ַ: %d   �ռ��С: %d\n",
                        currentEntry->attrIR.More.ProcAttr.code,
                        currentEntry->attrIR.More.ProcAttr.size);
                }
                break;

            default:
                output << "δ֪���ű�����" << endl;
                cout << "δ֪���ű�����" << endl;
                if (outfile) {
                    fprintf(outfile, "δ֪���ű�����\n");
                }
                break;
            }

            output << endl; // ��ӡ���зָ���ͬ���ű���
            cout << endl;
            if (outfile) {
                fprintf(outfile, "\n");
            }

            currentEntry = currentEntry->next; // �ƶ�����һ�����ű���
        }

        output << endl; // ��ӡ���зָ���ͬ��ķ��ű�
        cout << endl;
        if (outfile) {
            fprintf(outfile, "\n");
        }
    }

    output.flush();
    output.close();

    cout << "���ű��ӡ���" << endl;
    if (outfile) {
        fprintf(outfile, "���ű��ӡ���\n");
    }
}
//7.�ڵ������ű��в��ұ�ʶ��
bool SearchOneTable(char* id, int currentlevel, SymbolTable** Entry) {
    SymbolTable* current = symbolTableStack[currentlevel].next;
    while (current != nullptr) {
        if (strcmp(current->idname, id) == 0) {
            *Entry = current; // �����ҵ��ķ��ű���
            return true; // ��ʶ���ҵ�
        }
        current = current->next;
    }
    return false; // ��ʶ��δ�ҵ�
}




/*-----------------------------------------------------------�������----------------------------------------------------------*/
//1.�������������
void Analyze(TreeNode* currentP) {
    ///////////////////////////////////////////////////////////////////////
    if (currentP == nullptr) {
        cout << "����: �﷨�����ڵ�Ϊ��" << endl;
        fprintf(outfile, "����: �﷨�����ڵ�Ϊ��\n");
        ///////////////////////////////////////////////////////////////////////////////////
        semanticError++;
        return;
    }

    CreateTable(); //1.1����һ�����ű�
    initialize();  //2.2��ʼ�����������ڲ���ʾ����
    // ѭ���������������������
    LoopProgram(currentP);

    ////////////////////////////////////////////

    // ����Ƿ����������
    if (semanticError > 0) {
        cout << "������������������� " << semanticError << " ������" << endl;
        fprintf(outfile, "������������������� %d ������\n", semanticError);
        exit(1); // ���ڴ���ʱ�˳�
    }

    // ������������
    //WithStm(currentP);
     // �������ű�
    //DestroyTable();
}
//66)typedef enum { ProK, PheadK, TypeK, VarK, ProcDecK, StmLK, DecK, StmtK, ExpK }NodeKind;

//�����﷨����������Ӧ�ĺ��������﷨�����
void LoopProgram(TreeNode* t) {
    if (t == nullptr)
        return;
    if (t->nodekind == ProK) {   //���ڵ�
        LoopProgram(t->child[0]);
        LoopProgram(t->child[1]);
        LoopProgram(t->child[2]);
        return;
    }if (t->nodekind == PheadK) {   //����ͷ
        return;
    }
    if (t->nodekind == TypeK) {     //��������
        TypeDecPart(t->child[0]);  //7.�����������ַ���������
        LoopProgram(t->sibling);
        return;
    }
    if (t->nodekind == VarK) {   //��������
        VarDecList(t->child[0]);  //8.�����������ַ���������
        LoopProgram(t->sibling);
        return;
    }
    if (t->nodekind == ProcDecK) {   //��������(��������
        //ProcDecPart(t);   //9.�����������ַ���������
        ProcDecPart(t->child[0]);
        LoopProgram(t->sibling);
        return;
    }

    if (t->nodekind == StmLK) {    //������(�������
        Body(t->child[0]);   //12.ִ���岿�ַ���������
        return;
    }
    if (t->nodekind == StmtK) {  //���ڵ�  ???????
        Body(t->child[0]);
        return;
    }
}

TypeIR* intPtr, * charPtr, * boolPtr;
//�ڲ���ʾ�������������ͣ��ַ����ͣ���������
//2.��ʼ�����������ڲ���ʾ����
void initialize(void) {
    // ��ʼ����������
    intPtr = new TypeIR();
    // intPtr->serial = 1;        //�����Ա��е�λ�����  ?
    intPtr->size = 1;  //������ռ�ռ��С
    intPtr->kind = intTy;

    // ��ʼ���ַ�����
    charPtr = new TypeIR();
    //  charPtr->serial = 2;
    charPtr->size = 1;
    charPtr->kind = charTy;

    // ��ʼ����������
    boolPtr = new TypeIR();
    // boolPtr->serial = 3;
    boolPtr->size = 1;
    boolPtr->kind = boolTy;
}

//typedef enum { ArrayK, CharK, IntegerK, RecordK, IdK, decNull }  DecKind
//3.���ͷ�������
TypeIR* TypeProcess(TreeNode* t, DecKind deckind) {
    TypeIR* typePtr = nullptr;

    if (deckind == IdK) {
        // �����ʶ������
        typePtr = nameType(t);
    }
    else if (deckind == ArrayK) {
        // ������������
        typePtr = arrayType(t);
    }
    else if (deckind == RecordK) {
        // �����¼����
        typePtr = recordType(t);
    }
    else if (deckind == IntegerK) {
        // ������������
        return intPtr;
    }
    else if (deckind == CharK) {
        // �����ַ�����
        return charPtr;
    }
    else {
        // ����������������
        cout << "�������: ��" << t->lineno << "�У�δ֪���������: '" << deckind << "'  -TypeProcess" << endl;
        fprintf(outfile, "�������: ��%d�У�δ֪���������: '%d'  -TypeProcess\n", t->lineno, deckind);
        semanticError++;
    }
    return typePtr;
}

//4.�Զ��������ڲ��ṹ��������
TypeIR* nameType(TreeNode* t) {
    SymbolTable** Entry = new SymbolTable*;  //fhb��������

    if (t == nullptr || t->name.empty()) {
        cout << "�������:��Ч�Ľڵ�  -nameType" << endl;
        cout << "����: ��Ч�Ľڵ�" << endl;
        fprintf(outfile, "�������:��Ч�Ľڵ�  -nameType\n");
        fprintf(outfile, "����: ��Ч�Ľڵ�\n");
        semanticError++;
        return nullptr;
    }

    for (auto& name : t->name) {
        char* typeName = stringToChar(name);
        // ���� FindEntry �����ڷ��ű��в���������
        bool present = FindEntry(typeName, false, Entry);
        if (present) {
            // ����ʶ���������Ƿ�Ϊ typeKind
            if ((*Entry)->attrIR.kind == typeKind) {
                // ���������ڲ���ʾ
                return (*Entry)->attrIR.idtype;
            }
            else {
                // ����������ͱ�ʶ�����������ͱ�ʶ������
                cout << "�������: ��" << t->lineno << "�У������ͱ�ʶ������: '" << typeName << "'  -nameType" << endl;
                fprintf(outfile, "�������: ��%d�У������ͱ�ʶ������: '%s'  -nameType\n", t->lineno, typeName);
                semanticError++;
                return nullptr;
            }
        }
    }//for

    // ������������� name ��δ�ҵ���������������������
    cout << "�������: ��" << t->lineno << "�У��Զ������ͱ�ʶ��δ���� -nameType" << endl;
    fprintf(outfile, "�������: ��%d�У��Զ������ͱ�ʶ��δ���� -nameType\n", t->lineno);
    semanticError++;
    return nullptr;
}

//5.���������ڲ���ʾ������
TypeIR* arrayType(TreeNode* t) {
    // ��������Ͻ��Ƿ�С���½�
    if (t->attr.ArrayAttr.up < t->attr.ArrayAttr.low) {
        cout << "�������: ��" << t->lineno << "�У������Ͻ�С���½� -arrayType" << endl;
        fprintf(outfile, "�������: ��%d�У������Ͻ�С���½� -arrayType\n", t->lineno);
        semanticError++;
    }

    TypeIR* arrayType = new TypeIR();
    arrayType->kind = arrayTy;
    arrayType->More.ArrayAttr.low = t->attr.ArrayAttr.low;
    arrayType->More.ArrayAttr.up = t->attr.ArrayAttr.up;

    arrayType->More.ArrayAttr.indexTy = intPtr;
    arrayType->More.ArrayAttr.elemTy = TypeProcess(t, t->attr.ArrayAttr.childtype);

    // �������鳤��
    int arraySize = t->attr.ArrayAttr.up - t->attr.ArrayAttr.low;
    arrayType->size = arraySize;

    // ����������ڲ���ʾָ��
    return arrayType;
}


//6.�����¼���͵��ڲ���ʾ����
TypeIR* recordType(TreeNode* t) {
    // �½���¼���ͽڵ�
    TypeIR* recordType = new TypeIR();
    recordType->kind = recordTy;
    recordType->size = 0; // ��ʼ��СΪ 0
    FieldChain* lastField = nullptr; // ������������

    // ������¼���е������
    TreeNode* currentField = t->child[0]; // �����һ���ӽڵ��Ǽ�¼��Ŀ�ʼ
    //int recordoffset = 0;   //��¼�ֶε�ƫ����
    while (currentField != nullptr) {
        // ����һ���ڵ����ж�� ID �����
        for (auto& Name : currentField->name) {
            //����ת��
            char* idName = stringToChar(Name);
            //  if(lastField !=nullptr){
              // �����µ������͵�Ԫ�ṹ
            FieldChain* newField = new FieldChain();
            //newField->idname = idName; //char   char*???
            strcpy_s(newField->idname, idName);
            // ����������
            newField->unitType = TypeProcess(currentField, currentField->kind.dec);
            if (newField->unitType == nullptr) {
                //error
                cout << "�������: ��" << t->lineno << "�У���Ч���ֶ�����: '" << idName << "'  -arrayType" << endl;
                fprintf(outfile, "�������: ��%d�У���Ч���ֶ�����: '%s'  -arrayType\n", t->lineno, idName);
                semanticError++;
                return nullptr;
            }

            // ����ƫ����    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            newField->offset = recordType->size;
            recordType->size += newField->unitType->size;

            // ���ӵ�����
            if (lastField == nullptr) {
                recordType->More.body = newField; // ��һ����
            }
            else {
                lastField->next = newField; // ���ӵ���һ����
            }
            lastField = newField;
        }

        // �ƶ�����һ���ֵܽڵ�
        currentField = currentField->sibling;
    }

    // ���ؼ�¼���͵��ڲ���ʾָ��
    return recordType;
}

//7.�����������ַ���������
void TypeDecPart(TreeNode* t) {
    // ��ʼ�� present Ϊ 0���������ͱ��Ϊ typeKind
    bool present = false;

    // ����������������
    while (t != nullptr) {
        SymbolTable** Entry = new SymbolTable*;

        if (!t->name.empty()) {
            for (auto& name : t->name) {
                char* typeName = stringToChar(name);
                // ���õǼ����Ժ��� enter
                present = FindEntry(typeName, false, Entry);   //��鱾��
                if (present) {
                    // ��� present = 1����ʾ�ظ�����������
                    cout << "�������: ��" << t->lineno << "�У��ظ�����: '" << typeName << "'  -TypeDecPart" << endl;
                    fprintf(outfile, "�������: ��%d�У��ظ�����: '%s'  -TypeDecPart\n", t->lineno, typeName);
                    semanticError++;
                }
                else {
                    AttributeIR* AttribT = new AttributeIR;
                    AttribT->kind = typeKind;
                    //flagtree = true;//����ΪTRUE
                    AttribT->idtype = TypeProcess(t, t->kind.dec);
                    //flagtree = false;
                    Enter(typeName, AttribT, Entry, t);
                }
            }
        }

        // ȡ��һ����
        t = t->sibling;
    }
    return;
}

//8.�����������ַ���������
void VarDecList(TreeNode* t) {
    while (t != nullptr) {
        // ��鵱ǰ�ڵ�������Ƿ�Ϊ��������
        if (t->nodekind == DecK) {
            SymbolTable** Entry = new SymbolTable*;

            if (!t->name.empty()) {
                for (auto& name : t->name) {
                    char* typeName = stringToChar(name);
                    // �������ʹ����� TypeProcess������������͵��ڲ���ʾ
                    TypeIR* varType = TypeProcess(t, t->kind.dec);
                    bool flag = FindEntry(typeName, false, Entry);//�жϱ�����ʶ���Ƿ����
                    if (flag)
                    {
                        // ��� present = 1����ʾ�ظ�����������
                        cout << "�������: ��" << t->lineno << "�У�������ʶ���ظ�����: '" << typeName << "'  -VarDecList" << endl;
                        fprintf(outfile, "�������: ��%d�У�������ʶ���ظ�����: '%s'  -VarDecList\n", t->lineno, typeName);
                        semanticError++;
                    }
                    else {
                        AttributeIR* AttribV = new AttributeIR;
                        AttribV->kind = varKind;
                        AttribV->idtype = varType;
                        AttribV->More.VarAttr.access = Dir;//ֱ�Ӻͼ����ô�ж�?????
                        AttribV->More.VarAttr.level = NowTableLevel;
                        if (AttribV->idtype != nullptr)
                            // AttribV->More.VarAttr.off = AttribV->More.VarAttr.off + AttribV->idtype->size;//����ƫ��
                            AttribV->More.VarAttr.off = off + AttribV->idtype->size;
                        //off += AttribV->idtype->size; // ����ȫ��offset!!!!!!!!!!!!!!!!!!!!!!???????????????
                        Enter(typeName, AttribV, Entry, t);//������ű��ջ
                    }
                }
            }
        }
        t = t->sibling;
    }
    return;
}

//9.�����������ַ���������
void ProcDecPart(TreeNode* t) {
    if (t == nullptr) {
        return;
    }
    else {
        SymbolTable* Entry = HeadProcess(t);
    }

    //�����βΣ������βα�������ķ��ű��ڵ�param��
    if (t != nullptr) {
        LoopProgram(t->child[1]);    //�Ի�ѭ��
    }

    // ������һ������
    if (t == nullptr || t->child[2] == NULL) {
        return;
    }
    else {
        Body(t->child[2]->child[0]); //�������
    }
    return;
}

//10.��������ͷ��������
SymbolTable* HeadProcess(TreeNode* t) {
    if (t == nullptr || t->name.empty()) {
        cout << "�������: ����ͷ�ڵ���Ч -HeadProcess" << endl;
        fprintf(outfile, "�������: ����ͷ�ڵ���Ч -HeadProcess\n");
        semanticError++;
        return nullptr;
    }
    else {
        SymbolTable** Entry = new SymbolTable*;
        char* typeName = stringToChar(t->name[0]);
        // ���ù��̱�ʶ��������
        AttributeIR* AttribP = new AttributeIR();
        AttribP->kind = procKind;
        AttribP->More.ProcAttr.level = NowTableLevel;
        //������
        ParamTable* paramt = new ParamTable();
        AttribP->More.ProcAttr.param = paramt;

        // �Ǽǹ��̱�ʶ�������ű�
        Enter(typeName, AttribP, Entry, t);//������ű��ջ

        //����д�������
        CreateTable();
        //�����βΣ������βα�������ķ��ű��ڵ�param��
        (*Entry)->attrIR.More.ProcAttr.param = ParaDecList(t);//�����β�   //���ﲻ�Ǻ���para������Ҫ����

        return (*Entry);
    }
}

// 11.�βη���������
ParamTable * ParaDecList(TreeNode * t) {
    ParamTable* head = nullptr; // �βη��ű�����ͷָ��
    ParamTable* tail = nullptr; // �βη��ű�����βָ��

    if (t->child[0] != nullptr) {
        TreeNode* p = t->child[0];

        // �����β���������
        while (p != nullptr) {
            // ������ǰ�ڵ��е������β���
            for (int i = 0; i < p->idnum; ++i) {  //�����Ļ�֮ǰ�����ж���
                std::string paramName = p->name[i]; // ��ȡ�β���
                char* typeName = stringToChar(paramName);

                SymbolTable** Entry = new SymbolTable*;
                bool flag = FindEntry(typeName, false, Entry);
                if (flag) {
                    //error
                    cout << "�������: ��" << t->lineno << "�У��βα�ʶ���ظ�����: '" << typeName << "'  -ParaDecList" << endl;
                    fprintf(outfile, "�������: ��%d�У��βα�ʶ���ظ�����: '%s'  -ParaDecList\n", t->lineno, typeName);
                    semanticError++;
                }
                else {
                    AttributeIR* AttribPa = new AttributeIR();
                    AttribPa->kind = varKind;
                    AttribPa->idtype = TypeProcess(p, p->kind.dec);
                    AttribPa->More.VarAttr.access = Dir;//??????????????????????????
                    AttribPa->More.VarAttr.level = NowTableLevel;
                    // a->More.VarAttr.off = a->More.VarAttr.off + a->idtype->size;
                    AttribPa->More.VarAttr.off = off + AttribPa->idtype->size;
                    Enter(typeName, AttribPa, Entry, t);

                    ParamTable* newParam = new ParamTable();

                    // �����βη��ű�����ӵ��βη��ű�����
                    if (head == nullptr) {
                        head = newParam; // ����ǵ�һ���βΣ�����Ϊͷָ��
                    }
                    else {
                        tail->next = newParam; // �������ӵ�β��
                    }
                    newParam->entry = (*Entry);
                    newParam->next = NULL;
                    tail = newParam; // ����βָ��
                }
            }
            // ȡ��һ���β�����
            p = p->sibling;
        }
    }
    // �����βη��ű�����ͷָ��
    return head;
}

//12.ִ���岿�ַ���������
void Body(TreeNode* t) {
    if (t == nullptr) return;
    while (t != nullptr) {
        statement(t);
        t = t->sibling;
    }
}

//13.������з���������
void statement(TreeNode* t) {
    if (t == nullptr) {
        return; // ����ڵ�Ϊ�գ�ֱ�ӷ���
    }

    // ����������͵�����Ӧ�Ĵ�����
    switch (t->kind.stmt) {
    case IfK:
        // ���� if ���
        ifstatement(t);
        break;
    case WhileK:
        // ���� while ���
        whilestatement(t);
        break;
    case AssignK:
        // ����ֵ���
        assignstatement(t);
        break;
    case ReadK:
        // ��������
        readstatement(t);
        break;
    case WriteK:
        // ����д���
        writestatement(t);
        break;
    case CallK:
        // ����������
        callstatement(t);
        break;
    case ReturnK:
        // ���������
        returnstatement(t);
        break;
    default:
        // δ֪�������
        cout << "�������: ��" << t->lineno << "�У�δ֪������� -statement" << endl;
        fprintf(outfile, "�������: ��%d�У�δ֪������� -statement\n", t->lineno);
        semanticError++;
        break;
    }
    return;
    //�ݹ鴦���ֵܽڵ㣨��������е���һ����䣩
    //statement(t->sibling);
}

//14.���ʽ����������
TypeIR* Expr(TreeNode* t, AccessKind* Ekind) {
    if (t == nullptr) {
        return nullptr; // ����ڵ�Ϊ�գ�ֱ�ӷ���
    }
    TypeIR* Eptr = nullptr; // ���ʽ�����ڲ���ʾ��ָ��

    //?????????????????Ҫ��Ҫ���м��find????????????????????????????

    // ���ʽΪ�������÷��ʷ�ʽΪֱ�ӷ���(Dir)������������ָ��(intPtr)
    if (t->kind.exp == ConstK) {
        (*Ekind) = Dir;
        ///////////////////////////////////////////////////////////////////////////
        if (t->attr.ExpAttr.ischar == false) {
            // ������������
            Eptr = intPtr;
        }
        else if (t->attr.ExpAttr.ischar == true) {
            // �����ַ�����
            Eptr = charPtr;
        }
        return Eptr;
    }
    // ���ʽΪ�����ڷ��ű��в��ұ����� ����ͬ�������ͣ�
    else if (t->kind.exp == VariK) {

        SymbolTable** Entry = new SymbolTable*;

        char* typeName = stringToChar(t->name[0]);
        // ���� FindEntry �����ڷ��ű��в���������
        bool flag = FindEntry(typeName, true, Entry);//�жϱ�ʶ���Ƿ�������
        if (!flag)
        {
            cout << "�������: ��" << t->lineno << "�У�δ��������: '" << typeName << "' -Expr" << endl;
            fprintf(outfile, "�������: ��%d�У�δ��������: '%s' -Expr\n", t->lineno, typeName);
            semanticError++;
        }

        else {//////////////////////////////////////////////////////////////////////////////////
            // ����Ƿ����ӽڵ㣨�� a[i] �� a.i��
            if (t->child[0] != nullptr) {
                //�������ͷ���Ԫ�ص��ڲ���ʾ��kind
                //��������������ʽ������   ��������Ԫ�ص�����
                // ���1��������� a[...]
                if ((*Entry)->attrIR.idtype->kind == arrayTy) {
                    // ����ӽڵ��Ƿ��Ǳ��ʽ���� a[1] �� a[i]��
                    if (t->attr.ExpAttr.varkind == ArrayMembV) {
                        Expr(t->child[0], Ekind);
                        if (Expr(t->child[0], Ekind) != nullptr) {
                            if (Expr(t->child[0], Ekind)->kind == arrayTy)
                                return Expr(t->child[0], Ekind);
                        }

                        return (*Entry)->attrIR.idtype->More.ArrayAttr.elemTy; // ��������Ԫ������
                    }
                    // ���2���Ƿ�������� a.i
                    else {
                        cout << "�������: ��" << t->lineno << "�У����� '" << t->name[0]
                            << "' ����ʹ�� '.' �����ֶ�" << endl;
                        fprintf(outfile, "�������: ��%d�У����� '%s' ����ʹ�� '.' �����ֶ�\n",
                            t->lineno, t->name[0].c_str());
                        semanticError++;
                        return nullptr;
                    }

                }


                //��¼  �ڼ�¼�����в����ֶ��� �����ֶε�����
                else if ((*Entry)->attrIR.idtype->kind == recordTy) {
                    FieldChain* aa = (*Entry)->attrIR.idtype->More.body;
                    while (aa != nullptr)
                    {
                        if (aa->idname == t->child[0]->name[0])
                            return aa->unitType;
                    }
                    //error
                    //cout << "�������: ��¼��û�д�Ԫ��:" << typeName << "-��%d�У�-Expr\n" << t->lineno << endl;
                    // printf("������󣺵�%d�У�-Expr\n", t->lineno);
                    cout << "�������: ��" << t->lineno << "�У���¼��û�д�Ԫ��: '" << typeName << "' -Expr" << endl;
                    fprintf(outfile, "�������: ��%d�У���¼��û�д�Ԫ��: '%s' -Expr\n", t->lineno, typeName);
                    semanticError++;
                }
            }
            else {

                // ���� Var = id �����Σ��򵥱�����
               // SymbolTable* entry = FindEntry(t->name[0]); // �ڷ��ű��в��ұ�ʶ��

                if (Entry != nullptr && (*Entry)->attrIR.kind == varKind) {
                    *Ekind = InDir; // ����Ϊ��ӷ���
                }
                else {
                    cout << "�������: ��" << t->lineno << "�У�δ֪����: '" << typeName << "' -Expr" << endl;
                    fprintf(outfile, "�������: ��%d�У�δ֪����: '%s' -Expr\n", t->lineno, typeName);
                    semanticError++;
                }

                return (*Entry)->attrIR.idtype;
            }

        }

    }

    else if (t->kind.exp == OpK) {
        AccessKind* Ekindt = new AccessKind;
        TypeIR* a = Expr(t->child[0], Ekindt);
        TypeIR* b = Expr(t->child[1], Ekindt);
        if (a == b) {
            (*Ekind) = Dir;
            return a;
        }
        else {
            cout << "�������: ��" << t->lineno << "�У����ʽ������Ͳ�ƥ�� -Expr" << endl;
            fprintf(outfile, "�������: ��%d�У����ʽ������Ͳ�ƥ�� -Expr\n", t->lineno);
            semanticError++;
        }

    }
    return nullptr;
}

//15.��������ķ���������//////////////////////////name///////////////////////////////
//���var:=var0[E]��var0�ǲ����������ͱ�����E�ǲ��Ǻ�������±��������ƥ�䡣
TypeIR* arrayVar(TreeNode* t) {
    //���var0�ǲ����������ͱ���
    string arrayName = t->name[0]; //���������
    SymbolTable* arrayEntry = new SymbolTable();
    bool found = FindEntry(stringToChar(arrayName), true, &arrayEntry);//�������з��ű�

    if (!found) {
        cout << "�������: ��" << t->lineno << "�У�������� '" << arrayName << "' δ���� -arrayVar" << endl;
        fprintf(outfile, "�������: ��%d�У�������� '%s' δ���� -arrayVar\n", t->lineno, arrayName.c_str());
        semanticError++;
    }

    //�����������Ƿ�Ϊ����
    if (arrayEntry->attrIR.kind != varKind) {
        cout << "�������: ��" << t->lineno << "�У�'" << arrayName << "' ���Ǳ�����ʶ�� -arrayVar" << endl;
        fprintf(outfile, "�������: ��%d�У�'%s' ���Ǳ�����ʶ�� -arrayVar\n", t->lineno, arrayName.c_str());
        semanticError++;
    }

    //�����������Ƿ�Ϊ��������
    if (arrayEntry->attrIR.idtype->kind != TypeKind::arrayTy) {
        cout << "�������: ��" << t->lineno << "�У�'" << arrayName << "' �����������ͱ��� -arrayVar" << endl;
        fprintf(outfile, "�������: ��%d�У�'%s' �����������ͱ��� -arrayVar\n", t->lineno, arrayName.c_str());
        semanticError++;
        //return nullptr;
    }

    //���E�ǲ��Ǻ�������±��������ƥ��
    //��ȡ������±���ʽ�ڵ�
    TreeNode* indexNode = t->child[0]; // �±���ʽ�ǵ�һ���ӽڵ�

    if (indexNode == nullptr) {
        cout << "�������: ��" << t->lineno << "�У����������ȱ���±���ʽ -arrayVar" << endl;
        fprintf(outfile, "�������: ��%d�У����������ȱ���±���ʽ -arrayVar\n", t->lineno);
        semanticError++;
        //return nullptr;
    }
    else {////////////////////////////////////////////////////////////////////////////
        AccessKind* ekind = new AccessKind;
        TypeIR* indexType = Expr(indexNode, ekind); // �����±���ʽ������
        if (indexNode->idnum != 0) {//�±���ʽ���ǳ������ʽ
            SymbolTable* indexEntry = new SymbolTable;
            bool flag = FindEntry(stringToChar(indexNode->name[0]), true, &indexEntry);//�жϱ�ʶ���Ƿ�����

            //�����±���ʽ�еı����Ƿ�����
            if (!flag) {
                cout << "�������: ��" << t->lineno << "�У������±����" << t->child[0]->name[0] << "δ���� -arrayVar" << endl;
                fprintf(outfile, "�������: ��%d�У������±����%sδ���� -arrayVar\n",
                    t->lineno, t->child[0]->name[0].c_str());
                semanticError++;
                //return nullptr;
            }

            //�����±���ʽ�еı����Ƿ�Ϊ����
            if (indexEntry->attrIR.idtype->kind != TypeKind::intTy) {
                cout << "�������: ��" << t->lineno << "�У������±����" << t->child[0]->name[0] << "�������� -arrayVar" << endl;
                fprintf(outfile, "�������: ��%d�У������±����%s�������� -arrayVar\n",
                    t->lineno, t->child[0]->name[0].c_str());
                semanticError++;
                //return nullptr;
            }

            //�����±���ʽ������
            if (indexType == nullptr) {
                cout << "�������: ��" << t->lineno << "�У��޷������±���ʽ������ -arrayVar" << endl;
                fprintf(outfile, "�������: ��%d�У��޷������±���ʽ������ -arrayVar\n", t->lineno);
                semanticError++;
                //return nullptr;
            }

            // ����±���ʽ�����Ƿ��������±�����ƥ��
            if (indexType == nullptr) {
                cout << "�������: ��" << t->lineno << "�У��±���ʽΪ�� -arrayVar" << endl;
                fprintf(outfile, "�������: ��%d�У��±���ʽΪ�� -arrayVar\n", t->lineno);
                semanticError++;
            }
            else {
                if (indexType->kind != TypeKind::intTy) {
                    cout << "�������: ��" << t->lineno << "�У��±���ʽ�������� -arrayVar" << endl;
                    fprintf(outfile, "�������: ��%d�У��±���ʽ�������� -arrayVar\n", t->lineno);
                    semanticError++;
                    //return nullptr;
                }
            }
        }
        else {//�����±��ǳ������ʽ���ж��Ƿ�Խ��
            if (indexType == nullptr) {
                cout << "�������: ��" << t->lineno << "�У��±���ʽΪ�� -arrayVar" << endl;
                fprintf(outfile, "�������: ��%d�У��±���ʽΪ�� -arrayVar\n", t->lineno);
                semanticError++;
            }
            else {
                if (indexType->kind != TypeKind::intTy) {
                    cout << "�������: ��" << t->lineno << "�У��±���ʽ�������� -arrayVar" << endl;
                    fprintf(outfile, "�������: ��%d�У��±���ʽ�������� -arrayVar\n", t->lineno);
                    semanticError++;
                    //return nullptr;
                }
                else {/////////////////////////////////////////////////////////////////////////////
                    int arrayup = arrayEntry->attrIR.idtype->More.ArrayAttr.up - 1;
                    int arraylow = arrayEntry->attrIR.idtype->More.ArrayAttr.low - 1;
                    if (indexNode->attr.ExpAttr.val<arraylow || indexNode->attr.ExpAttr.val>arrayup) {
                        cout << "�������: ��" << t->lineno << "�У������±�Խ�� -arrayVar" << endl;
                        fprintf(outfile, "�������: ��%d�У������±�Խ�� -arrayVar\n", t->lineno);
                        semanticError++;
                    }
                }
            }
        }
    }
    // ����ָ������Ԫ�������ڲ���ʾ��ָ��
    return arrayEntry->attrIR.idtype;
}
//16.��¼������������ķ���������
TypeIR* recordVar(TreeNode* t) {
    //��ȡ��¼������
    if (t->name.empty()) {
        cout << "�������: ��" << t->lineno << "�У���¼������ȱ�ٱ����������� -recordVar" << endl;
        fprintf(outfile, "�������: ��%d�У���¼������ȱ�ٱ����������� -recordVar\n", t->lineno);
        semanticError++;
        return nullptr;
    }

    string varName = t->name[0];
    SymbolTable* varEntry = new SymbolTable;

    bool found = FindEntry(stringToChar(varName), true, &varEntry);//�����з��ű��в��ұ�ʶ���Ƿ�����

    //�ڷ��ű��в��Ҽ�¼����
    if (!found) {
        cout << "�������: ��" << t->lineno << "�У���¼���� '" << varName << "' δ���� -recordVar" << endl;
        fprintf(outfile, "�������: ��%d�У���¼���� '%s' δ���� -recordVar\n", t->lineno, varName.c_str());
        semanticError++;
    }

    //����¼�����Ƿ�Ϊ����
    if (varEntry->attrIR.kind != varKind) {
        cout << "�������: ��" << t->lineno << "�У�'" << varName << "' ���Ǳ��� -recordVar" << endl;
        fprintf(outfile, "�������: ��%d�У�'%s' ���Ǳ��� -recordVar\n", t->lineno, varName.c_str());
        semanticError++;
    }

    //����¼�����Ƿ�Ϊ��¼����
    if (varEntry->attrIR.idtype->kind != TypeKind::recordTy) {
        cout << "�������: ��" << t->lineno << "�У�'" << varName << "' ���Ǽ�¼���ͱ��� -recordVar" << endl;
        fprintf(outfile, "�������: ��%d�У�'%s' ���Ǽ�¼���ͱ��� -recordVar\n", t->lineno, varName.c_str());
        semanticError++;
    }

    //��ȡ��¼���͵����
    FieldChain* fieldChain = varEntry->attrIR.idtype->More.body;

    if (fieldChain == nullptr) {
        cout << "�������: ��" << t->lineno << "�У���¼���� '" << varName << "' �����Ϊ�� -recordVar" << endl;
        fprintf(outfile, "�������: ��%d�У���¼���� '%s' �����Ϊ�� -recordVar\n", t->lineno, varName.c_str());
        semanticError++;
    }

    TreeNode* fieldNode = t->child[0];

    while (fieldNode != nullptr) {
        if (fieldNode->idnum != 0) {//�жϵ�ǰ�ڵ��Ƿ�������
            SymbolTable* Entry1 = new SymbolTable;
            bool flag = FindEntry(stringToChar(fieldNode->name[0]), true, &Entry1);//�ڷ��ű��в��Ҹ������Ƿ�������
            if (!flag) {
                cout << "�������: ��" << t->lineno << "�У������ '" << fieldNode->name[0] << "' δ���� -recordVar" << endl;
                fprintf(outfile, "�������: ��%d�У������ '%s' δ���� -recordVar\n",
                    t->lineno, fieldNode->name[0].c_str());
                semanticError++;
            }
            else {
                //������в�������
                FieldChain* foundDomain = new FieldChain;
                bool fieldFound = FindField(stringToChar(fieldNode->name[0]), fieldChain, &foundDomain);//������в�������
                if (!fieldFound) {//û�ҵ�
                    cout << "�������: ��" << t->lineno << "�У�δ��������ҵ������ '" << fieldNode->name[0] << "' -recordVar" << endl;
                    fprintf(outfile, "�������: ��%d�У�δ��������ҵ������ '%s' -recordVar\n",
                        t->lineno, fieldNode->name[0].c_str());
                    semanticError++;
                    break;
                }
            }
        }
        //else {
        //    cout << "�������: ��" << t->lineno << "�У���ǰ�ڵ������û������ -recordVar" << endl;
        //    semanticError++;
        //}
        fieldNode = fieldNode->child[0];
    }

    //����ָ���¼Ԫ�������ڲ���ʾ��ָ��
    return varEntry->attrIR.idtype;
}
//17.��ֵ����������
void assignstatement(TreeNode* t) {
    //��ȡ��ֵ������ֵ����ֵ�ڵ�
    TreeNode* leftChild = t->child[0]; //��ֵ�ǵ�һ���ӽڵ�
    TreeNode* rightChild = t->child[1]; //��ֵ�ǵڶ����ӽڵ�

    if (leftChild == nullptr || rightChild == nullptr) {
        cout << "�������: ��" << t->lineno << "�У���ֵ���ȱ����ֵ����ֵ -assignstatement" << endl;
        fprintf(outfile, "�������: ��%d�У���ֵ���ȱ����ֵ����ֵ -assignstatement\n", t->lineno);
        semanticError++;
    }

    //�����ֵ�Ƿ�Ϊ��ʶ��
    if (leftChild->nodekind != ExpK || leftChild->kind.exp != VariK) {
        cout << "�������: ��" << t->lineno << "�У���ֵ������ֵ���Ǳ�ʶ�� -assignstatement" << endl;
        fprintf(outfile, "�������: ��%d�У���ֵ������ֵ���Ǳ�ʶ�� -assignstatement\n", t->lineno);
        semanticError++;
    }
    else {
        //�ڷ��ű��в�����ֵ��ʶ��
        SymbolTable* leftEntry = new SymbolTable;
        bool found = FindEntry(stringToChar(leftChild->name[0]), true, &leftEntry);

        if (!found) {
            cout << "�������: ��" << t->lineno << "�У���ʶ�� '" << leftChild->name[0] << "' δ���� -assignstatement" << endl;
            fprintf(outfile, "�������: ��%d�У���ʶ�� '%s' δ���� -assignstatement\n",
                t->lineno, leftChild->name[0].c_str());
            semanticError++;
        }
        else {
            //��ȡ��ֵ������
            TypeIR* leftType = leftEntry->attrIR.idtype;
            TypeIR* leftKind = leftType;

            //�����˱�ʶ���Ƿ�Ϊ����
            if (leftEntry->attrIR.kind != varKind) {
                cout << "�������: ��" << t->lineno << "�У���ֵ������˱�ʶ�����Ǳ��� -assignstatement" << endl;
                fprintf(outfile, "�������: ��%d�У���ֵ������˱�ʶ�����Ǳ��� -assignstatement\n", t->lineno);
                semanticError++;
            }

            if (leftChild->child[0] != nullptr) {
                if (leftType->kind == arrayTy) {//�ж��ǲ�����������
                    AccessKind* ekind = new AccessKind;
                    leftKind = Expr(leftChild->child[0], ekind);
                    TreeNode* EXP = leftChild->child[0];
                    if (EXP->kind.exp == ConstK) {
                        arrayVar(leftChild);
                    }
                }
                else if (leftType->kind == recordTy) {//�ж��ǲ��Ǽ�¼����
                    FieldChain* leftField = leftType->More.body;
                    bool flag = true;
                    while (leftField != nullptr) {
                        if (leftField->idname == leftChild->child[0]->name[0]) {//�ж������Ƿ���ڸ�Ԫ��
                            leftKind = leftField->unitType;
                            flag = false;
                            break;
                        }
                        leftField = leftField->next;
                    }
                    if (flag) {
                        cout << "�������: ��" << t->lineno << "�У���¼���͵�����û�и�ֵ������Ԫ�� -assignstatement" << endl;
                        fprintf(outfile, "�������: ��%d�У���¼���͵�����û�и�ֵ������Ԫ�� -assignstatement\n", t->lineno);
                        semanticError++;
                    }
                    recordVar(leftChild);
                }
            }

            //������ֵ���ʽ
            AccessKind Ekind;
            TypeIR* rightType = Expr(rightChild, &Ekind); //������ֵ���ʽ������
            if (rightType == nullptr) {
                cout << "�������: ��" << t->lineno << "�У��޷�������ֵ���ʽ������ -assignstatement" << endl;
                fprintf(outfile, "�������: ��%d�У��޷�������ֵ���ʽ������ -assignstatement\n", t->lineno);
                semanticError++;
            }
            else {
                if (rightChild == nullptr) {
                    cout << "�������: ��" << t->lineno << "�У���ֵ���ʽΪ�� -assignstatement" << endl;
                    fprintf(outfile, "�������: ��%d�У���ֵ���ʽΪ�� -assignstatement\n", t->lineno);
                    semanticError++;
                }
                else {
                    if (rightChild->idnum != 0) {
                        // ��ǰ��ע�͵Ĵ���
                    }
                    else {
                        // �մ����
                    }
                }
            }
            //��鸳ֵ�����˵������Ƿ�����
            if (leftKind != rightType) {
                cout << "�������: ��" << t->lineno << "�У���ֵ�����˵����Ͳ�ƥ�� -assignstatement" << endl;
                fprintf(outfile, "�������: ��%d�У���ֵ�����˵����Ͳ�ƥ�� -assignstatement\n", t->lineno);
                semanticError++;
            }
        }
    }
}

//18.���̵���������������
void callstatement(TreeNode* t) {
    //��ȡ������
    if (t->name.empty()) {
        /*cout << "�������: ��" << t->lineno << "�У����̵��������ȱ�ٹ����� -callstatement" << endl;
        fprintf(outfile, "�������: ��%d�У����̵��������ȱ�ٹ����� -callstatement\n", t->lineno);*/
        semanticError++;
    }

    string procName = t->name[0];
    SymbolTable* procEntry = new SymbolTable;
    bool found = FindEntry(stringToChar(procName), true, &procEntry);//�жϹ��̱�ʶ���Ƿ�����

    if (!found) {
        cout << "�������: ��" << t->lineno << "�У����� '" << procName << "' δ���� -callstatement" << endl;
        fprintf(outfile, "�������: ��%d�У����� '%s' δ���� -callstatement\n", t->lineno, procName.c_str());
        semanticError++;
    }
    else {
        //����ʶ���Ƿ�Ϊ������
        if (procEntry->attrIR.kind != procKind) {
            cout << "�������: ��" << t->lineno << "�У���ʶ��'" << procName << "' ���ǹ����� -callstatement" << endl;
            fprintf(outfile, "�������: ��%d�У���ʶ��'%s' ���ǹ����� -callstatement\n", t->lineno, procName.c_str());
            semanticError++;
        }
        else {
            //��ȡ�βα�
            ParamTable* paramTable = procEntry->attrIR.More.ProcAttr.param;
            if (paramTable == nullptr) {
                cout << "�������: ��" << t->lineno << "�У����� '" << procName << "' û���βα� -callstatement" << endl;
                fprintf(outfile, "�������: ��%d�У����� '%s' û���βα� -callstatement\n", t->lineno, procName.c_str());
                semanticError++;
            }

            //��ȡʵ���б�
            TreeNode* actualParamNode = t->child[0]; //ʵ���б��ǵ�һ���ӽڵ�
            if (actualParamNode == nullptr) {
                cout << "�������: ��" << t->lineno << "�У����̵��������ȱ��ʵ���б� -callstatement" << endl;
                fprintf(outfile, "�������: ��%d�У����̵��������ȱ��ʵ���б� -callstatement\n", t->lineno);
                semanticError++;
            }

            //�����βα��ʵ���б������ʵ�ζ�Ӧ��ϵ
            ParamTable* currentParam = paramTable;
            TreeNode* currentActualParam = actualParamNode;
            while (currentParam != nullptr && currentActualParam != nullptr) {
                //��ȡ�βε�����
                TypeIR* paramType = currentParam->entry->attrIR.idtype;
                if (paramType == nullptr) {
                    cout << "�������: ��" << t->lineno << "�У��β�����Ϊ�� -callstatement" << endl;
                    fprintf(outfile, "�������: ��%d�У��β�����Ϊ�� -callstatement\n", t->lineno);
                    semanticError++;
                }

                //����ʵ�ε�����
                AccessKind ekind;
                TypeIR* actualParamType = Expr(currentActualParam, &ekind);
                if (actualParamType == nullptr) {
                    cout << "�������: ��" << t->lineno << "�У��޷�����ʵ������ -callstatement" << endl;
                    fprintf(outfile, "�������: ��%d�У��޷�����ʵ������ -callstatement\n", t->lineno);
                    semanticError++;
                }

                if (paramType != actualParamType) {
                    if (paramType == nullptr) {
                        cout << "�������: ��" << t->lineno << "�У��β�����Ϊ�� -callstatement" << endl;
                        fprintf(outfile, "�������: ��%d�У��β�����Ϊ�� -callstatement\n", t->lineno);
                        semanticError++;
                    }
                    else if (actualParamType == nullptr) {
                        cout << "�������: ��" << t->lineno << "�У�ʵ������Ϊ�� -callstatement" << endl;
                        fprintf(outfile, "�������: ��%d�У�ʵ������Ϊ�� -callstatement\n", t->lineno);
                        semanticError++;
                    }
                    else {
                        if (paramType->kind == arrayTy && actualParamType->kind == arrayTy) {//����������;�Ϊ��������
                            if (paramType->size == actualParamType->size && paramType->More.ArrayAttr.elemTy == actualParamType->More.ArrayAttr.elemTy)
                                ;
                            else {
                                cout << "�������: ��" << t->lineno << "�У��βκ�ʵ�β�ƥ�� -callstatement" << endl;
                                fprintf(outfile, "�������: ��%d�У��βκ�ʵ�β�ƥ�� -callstatement\n", t->lineno);
                                semanticError++;
                            }
                        }
                        else {
                            cout << "�������: ��" << t->lineno << "�У��βκ�ʵ�β�ƥ�� -callstatement" << endl;
                            fprintf(outfile, "�������: ��%d�У��βκ�ʵ�β�ƥ�� -callstatement\n", t->lineno);
                            semanticError++;
                        }
                    }
                }

                //����βκ�ʵ�������Ƿ�ƥ��
                if (paramType == nullptr) {
                    cout << "�������: ��" << t->lineno << "�У��β�����Ϊ�� -callstatement" << endl;
                    fprintf(outfile, "�������: ��%d�У��β�����Ϊ�� -callstatement\n", t->lineno);
                    semanticError++;
                }
                else if (actualParamType == nullptr) {
                    cout << "�������: ��" << t->lineno << "�У�ʵ������Ϊ�� -callstatement" << endl;
                    fprintf(outfile, "�������: ��%d�У�ʵ������Ϊ�� -callstatement\n", t->lineno);
                    semanticError++;
                }
                else {
                    if (paramType->kind != actualParamType->kind) {
                        cout << "�������: ��" << t->lineno << "�У��βκ�ʵ�����Ͳ�ƥ�� -callstatement" << endl;
                        fprintf(outfile, "�������: ��%d�У��βκ�ʵ�����Ͳ�ƥ�� -callstatement\n", t->lineno);
                        semanticError++;
                    }
                }

                //�ƶ�����һ���βκ�ʵ��
                currentParam = currentParam->next;
                currentActualParam = currentActualParam->sibling;
            }

            //����βκ�ʵ�������Ƿ�һ��
            if (currentParam != nullptr || currentActualParam != nullptr) {
                cout << "�������: ��" << t->lineno << "�У��βκ�ʵ��������ƥ�� -callstatement" << endl;
                fprintf(outfile, "�������: ��%d�У��βκ�ʵ��������ƥ�� -callstatement\n", t->lineno);
                semanticError++;
            }
            return;
        }
    }
}

//19.����������������
void ifstatement(TreeNode* t) {
    //��ȡ�������ʽ�ڵ�
    TreeNode* conditionNode = t->child[0]; //�������ʽ�ǵ�һ���ӽڵ�
    if (conditionNode == nullptr) {
        cout << "�������: ��" << t->lineno << "�У���������е��������ʽΪ�� -ifstatement" << endl;
        fprintf(outfile, "�������: ��%d�У���������е��������ʽΪ�� -ifstatement\n", t->lineno);
        semanticError++;
    }
    else {
        //�����������ʽ������
        AccessKind ekind;
        TypeIR* conditionType = Expr(conditionNode, &ekind); //���ñ��ʽ��������

        if (conditionType == nullptr) {
            cout << "�������: ��" << t->lineno << "�У��޷������������ʽ������ -ifstatement" << endl;
            fprintf(outfile, "�������: ��%d�У��޷������������ʽ������ -ifstatement\n", t->lineno);
            semanticError++;
        }

        SymbolTable* Entry = new SymbolTable;

        if (conditionNode->idnum != 0) {
            for (int i = 0; i < conditionNode->name.size(); i++) {
                bool found = FindEntry(stringToChar(conditionNode->name[i]), true, &Entry);
                if (!found) {
                    cout << "�������: ��" << t->lineno << "�У����� '" << conditionNode->name[i] << "' δ���� -ifstatement" << endl;
                    fprintf(outfile, "�������: ��%d�У����� '%s' δ���� -ifstatement\n",
                        t->lineno, conditionNode->name[i].c_str());
                    semanticError++;
                }
                else {
                    if (Entry->attrIR.idtype->kind != TypeKind::intTy && Entry->attrIR.idtype->kind != TypeKind::boolTy) {
                        cout << "�������: " << "��" << t->lineno << "�У���������е��������ʽ���ǲ������� -ifstatement" << endl;
                        fprintf(outfile, "�������: ��%d�У���������е��������ʽ���ǲ������� -ifstatement\n", t->lineno);
                        semanticError++;
                    }
                }
            }
        }
        else {
            if (conditionNode->nodekind != NodeKind::ExpK) {
                cout << "�������: " << "��" << t->lineno << "�У���������е��������ʽ���ǲ������� -ifstatement" << endl;
                fprintf(outfile, "�������: ��%d�У���������е��������ʽ���ǲ������� -ifstatement\n", t->lineno);
                semanticError++;
            }
        }

        //���� then ������в���
        TreeNode* thenStmt = t->child[1]; //then ��������ǵڶ����ӽڵ�
        if (thenStmt == nullptr) {
            cout << "�������: ��" << t->lineno << "�У���������е� then �������Ϊ�� -ifstatement" << endl;
            fprintf(outfile, "�������: ��%d�У���������е� then �������Ϊ�� -ifstatement\n", t->lineno);
            semanticError++;
        }
        else {
            while (thenStmt != nullptr) {
                //�ݹ鴦�� then ��������е�ÿ�����
                statement(thenStmt);
                thenStmt = thenStmt->sibling;
            }
        }

        //���� else ������в��֣�����У�
        TreeNode* elseStmt = t->child[2]; //else ��������ǵ������ӽڵ�
        if (elseStmt == nullptr) {
            cout << "�������: ��" << t->lineno << "�У���������е� else �������Ϊ�� -ifstatement" << endl;
            fprintf(outfile, "�������: ��%d�У���������е� else �������Ϊ�� -ifstatement\n", t->lineno);
            semanticError++;
        }
        else {
            while (elseStmt != nullptr) {
                //�ݹ鴦�� else ��������е�ÿ�����
                statement(elseStmt);
                elseStmt = elseStmt->sibling;
            }
        }
    }
    return;
}

//20.ѭ��������������
void whilestatement(TreeNode* t) {
    //��ȡ�������ʽ�ڵ�
    TreeNode* conditionNode = t->child[0];
    if (conditionNode == nullptr) {
        cout << "�������: " << "��" << t->lineno << "�У�ѭ������е��������ʽΪ�� -whilestatement" << endl;
        fprintf(outfile, "�������: ��%d�У�ѭ������е��������ʽΪ�� -whilestatement\n", t->lineno);
        semanticError++;
    }

    //�����������ʽ������
    AccessKind ekind;
    TypeIR* conditionType = Expr(conditionNode, &ekind); //���ñ��ʽ��������

    if (conditionType == nullptr) {
        cout << "�������: " << "��" << t->lineno << "�У��޷�����ѭ������е��������ʽ���� -whilestatement" << endl;
        fprintf(outfile, "�������: ��%d�У��޷�����ѭ������е��������ʽ���� -whilestatement\n", t->lineno);
        semanticError++;
    }

    if (conditionNode != nullptr) {
        SymbolTable* Entry = new SymbolTable;

        if (conditionNode->idnum != 0) {
            for (int i = 0; i < conditionNode->name.size(); i++) {
                bool found = FindEntry(stringToChar(conditionNode->name[i]), true, &Entry);
                if (!found) {
                    cout << "�������: ��" << t->lineno << "�У����� '" << conditionNode->name[i] << "' δ���� -whilestatement" << endl;
                    fprintf(outfile, "�������: ��%d�У����� '%s' δ���� -whilestatement\n",
                        t->lineno, conditionNode->name[i].c_str());
                    semanticError++;
                }
                else {
                    if (Entry->attrIR.idtype->kind != TypeKind::intTy && Entry->attrIR.idtype->kind != TypeKind::boolTy) {
                        cout << "�������: " << "��" << t->lineno << "�У�ѭ������е��������ʽ���ǲ������� -whilestatement" << endl;
                        fprintf(outfile, "�������: ��%d�У�ѭ������е��������ʽ���ǲ������� -whilestatement\n", t->lineno);
                        semanticError++;
                    }
                }
            }
        }
        else {
            if (conditionNode->nodekind != NodeKind::ExpK) {
                cout << "�������: " << "��" << t->lineno << "�У�ѭ������е��������ʽ���ǲ������� -whilestatement" << endl;
                fprintf(outfile, "�������: ��%d�У�ѭ������е��������ʽ���ǲ������� -whilestatement\n", t->lineno);
                semanticError++;
            }
        }

        //��ȡѭ����������нڵ�
        TreeNode* bodyNode = t->child[1]; //ѭ�����ǵڶ����ӽڵ�
        if (bodyNode == nullptr) {
            cout << "�������: " << "��" << t->lineno << "�У�ѭ������е�ѭ����Ϊ�� -whilestatement" << endl;
            fprintf(outfile, "�������: ��%d�У�ѭ������е�ѭ����Ϊ�� -whilestatement\n", t->lineno);
            semanticError++;
        }
        else {
            //����ѭ�����е��������
            while (bodyNode != nullptr) {
                statement(bodyNode); //���� statement ���������������
                bodyNode = bodyNode->sibling;
            }
        }
    }

    return;
}

//21.��������������
void readstatement(TreeNode* t) {
    if (t->name.empty()) {
        cout << "�������: ��" << t->lineno << "�У��ڵ�ȱ������" << endl;
        fprintf(outfile, "�������: ��%d�У��ڵ�ȱ������\n", t->lineno);
        semanticError++;
        return;
    }

    string varName = t->name[0];
    SymbolTable* entry = new SymbolTable;
    bool found = FindEntry(stringToChar(varName), true, &entry);//falseΪ�鵱ǰ���ű�trueΪ��ȫ�����ű�

    if (!found) {
        cout << "�������: " << "��" << t->lineno << "�У�������ʶ�� '" << varName << "' δ���� -readstatement" << endl;
        fprintf(outfile, "�������: ��%d�У�������ʶ�� '%s' δ���� -readstatement\n",
            t->lineno, varName.c_str());
        semanticError++;
    }
    else {
        // ����ʶ���Ƿ�Ϊ����
        if (entry->attrIR.kind != varKind) {
            cout << "�������: " << "��" << t->lineno << "�У�'" << varName << "' ���Ǳ�����ʶ�� readstatement" << endl;
            fprintf(outfile, "�������: ��%d�У�'%s' ���Ǳ�����ʶ�� readstatement\n",
                t->lineno, varName.c_str());
            semanticError++;
        }
    }
    return;
}

//22.����������������
void returnstatement(TreeNode* t) {
    return;
}

//23.д������������
void writestatement(TreeNode* t) {
    //��ȡд����еı��ʽ
    TreeNode* exprNode = t->child[0]; //���ʽ��д���ĵ�һ���ӽڵ�
    if (exprNode == nullptr) {
        cout << "�������: " << "��" << t->lineno << "�У�д����еı��ʽΪ�� -writestatement" << endl;
        fprintf(outfile, "�������: ��%d�У�д����еı��ʽΪ�� -writestatement\n", t->lineno);
        semanticError++;
    }

    //�������ʽ������
    AccessKind* ekind = new AccessKind;
    TypeIR* exprType = Expr(exprNode, ekind); //���ñ��ʽ��������

    if (exprType == nullptr) {
        cout << "�������: ��" << t->lineno << "�У��޷�����д����еı��ʽ���� -writestatement" << endl;
        fprintf(outfile, "�������: ��%d�У��޷�����д����еı��ʽ���� -writestatement\n", t->lineno);
        semanticError++;
    }

    return;
}