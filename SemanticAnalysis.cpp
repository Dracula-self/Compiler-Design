#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<fstream>
#include"SemanticAnalysis.h"

using namespace std;
const int initoff = 7;//初始偏移量

SymbolTable symbolTableStack[1000];//符号表栈
int NowTableLevel = 0;//当前符号表层数,符号表层数从1开始，若nowlevel==0则证明当前没有符号表
int semanticError = 0;//记录语义分析错误个数

int off = 0, off1 = 0;

/*---------------------------------------------------------符号表---------------------------------------------------------------*/
//1.创建一个符号表 偏移量赋给newTable->attrIR.More.VarAttr.off
void CreateTable(void) {
    off1 = off;
    off = 0;
    NowTableLevel++;

    //////////////////////////////////////////////////////////
    symbolTableStack[NowTableLevel].next = nullptr;
}

//2.撤销一个符号表
void DestroyTable() {
    //检查栈是否为空
    if (NowTableLevel == 0) {
        cout << "语义错误: 摧毁符号表失败，当前没有符号表" << endl;
        if (outfile) {
            fprintf(outfile, "语义错误: 摧毁符号表失败，当前没有符号表\n");
        }
        semanticError++;
        return;
    }

    //获取当前符号表
    SymbolTable* currentTable = symbolTableStack[NowTableLevel].next;

    symbolTableStack[NowTableLevel].next = nullptr; //弹出当前符号表

    off = off1;

    while (currentTable != nullptr) {
        SymbolTable* temp = currentTable;
        currentTable = currentTable->next;
        delete temp; // 释放内存
    }

    // 打印当前层数（用于调试）//////////////////////////////////////////////////////////////////////////////////////////
    cout << "摧毁的符号表在第" << NowTableLevel << "层" << endl;
    if (outfile) {
        fprintf(outfile, "摧毁的符号表在第%d层\n", NowTableLevel);
    }

    NowTableLevel--;//层数减一
}

//3.登记标识符和属性到符号表  将TreeNodeSymbol Type* table[10]填入???????????????????????????????????????????
bool Enter(char* Id, AttributeIR* AttribP, SymbolTable** Entry, TreeNode* t) {
    //调用 FindEntry 查找当前符号表，检查标识符是否已存在？？？？？？？？？？？？？？？？？？？？？为什么不是所有符号表
    bool present = FindEntry(Id, false, Entry);

    //如果标识符已存在，返回错误
    if (present) {
        cout << "错误: 标识符'" << Id << "'重复声明 -Enter" << endl;
        if (outfile) {
            fprintf(outfile, "错误: 标识符'%s'重复声明 -Enter\n", Id);
        }
        semanticError++;
        return false;
    }
    else {
        //创建一个新的符号表项
        SymbolTable* newEntry = new SymbolTable();
        strcpy_s(newEntry->idname, Id);//复制标识符名称
        newEntry->attrIR = *AttribP;//复制标识符属性
        newEntry->next = nullptr;//初始化 next 指针

        //将新符号表项插入当前符号表尾部
        if (symbolTableStack[NowTableLevel].next == nullptr) {
            //如果当前符号表为空，新项成为第一个节点
            symbolTableStack[NowTableLevel].next = newEntry;
        }
        else {
            //否则，遍历到符号表尾部并插入新项
            SymbolTable* currentTable = symbolTableStack[NowTableLevel].next;
            while (currentTable->next != nullptr) {
                currentTable = currentTable->next;
            }
            currentTable->next = newEntry;
        }

        // 关联到语法树节点
        if (t && t->idnum < 10) {
            t->table[t->idnum - 1] = newEntry;
        }

        *Entry = newEntry;

        //返回成功
        std::cout << "标识符'" << Id << "'成功插入符号表 -Enter" << endl;
        if (outfile) {
            fprintf(outfile, "标识符'%s'成功插入符号表 -Enter\n", Id);
        }
        return true;
    }
}

//4.符号表中查找标识符
//flag=true:在所有符号表中查找，flag=false:在当前符号表中查找
bool FindEntry(char* id, bool flag, SymbolTable** Entry) {
    if (flag) {// 在所有符号表中查找
        for (int i = NowTableLevel; i > 0; i--) {
            if (SearchOneTable(id, i, Entry)) {
                return true;
            }
        }
        *Entry = nullptr;
        return false; // 如果未找到，返回 false
    }
    else {// 仅在当前符号表查找
        return SearchOneTable(id, NowTableLevel, Entry);
    }
    *Entry = nullptr;
    return false;
}

//5.在域表中查找域名
bool FindField(char* Id, FieldChain* head, FieldChain** Entry) {
    FieldChain* current = head; //从域表头开始查找

    // 遍历域表
    while (current != nullptr) {
        //比较当前节点的标识符名称与目标Id
        if (strcmp(current->idname, Id) == 0) {
            //如果找到匹配的标识符
            *Entry = current; //设置Entry指向找到的域表项
            return true; //返回true表示查找成功
        }
        current = current->next; //移动到下一个节点
    }

    //如果遍历完域表仍未找到匹配的标识符
    //if (Entry != nullptr) {
    //    *Entry = nullptr; // 设置Entry为空
    //}
    return false; // 返回false表示查找失败
}

//6.打印符号表
void PrintSymbTable(const string& path) {
    ofstream output;
    output.open(path, ios::app);
    if (!output.is_open()) {
        printf("无法打开文件: %s -PrintSymbTable\n", path.c_str());
        if (outfile) {
            fprintf(outfile, "无法打开文件: %s -PrintSymbTable\n", path.c_str());
        }
        return;
    }

    // 同时打印到控制台和文件
    cout << "开始打印符号表..." << endl;
    if (outfile) {
        fprintf(outfile, "开始打印符号表...\n");
    }

    // 从第一层开始，依次打印各层符号表
    for (int level = 1; level <= NowTableLevel; level++) {
        SymbolTable* currentEntry = symbolTableStack[level].next; // 获取当前层的符号表

        // 输出到文件
        output << "第" << level << "层符号表:" << endl;

        // 输出到控制台和outfile
        cout << "第" << level << "层符号表:" << endl;
        if (outfile) {
            fprintf(outfile, "第%d层符号表:\n", level);
        }

        // 遍历当前层的符号表
        ParamTable* param = nullptr;
        while (currentEntry != nullptr) {
            // 根据 IdKind 分类打印
            switch (currentEntry->attrIR.kind) {
            case typeKind: // 类型符号表
                output << "------------类型符号表------------" << endl;
                output << "类型名: " << currentEntry->idname << "   类型种类: " << currentEntry->attrIR.idtype->kind << "   类型大小: " << currentEntry->attrIR.idtype->size << endl;

                cout << "------------类型符号表------------" << endl;
                cout << "类型名: " << currentEntry->idname << "   类型种类: " << currentEntry->attrIR.idtype->kind << "   类型大小: " << currentEntry->attrIR.idtype->size << endl;

                if (outfile) {
                    fprintf(outfile, "------------类型符号表------------\n");
                    fprintf(outfile, "类型名: %s   类型种类: %d   类型大小: %d\n",
                        currentEntry->idname,
                        currentEntry->attrIR.idtype->kind,
                        currentEntry->attrIR.idtype->size);
                }
                break;

            case varKind: // 变量符号表
                output << "------------变量符号表------------" << endl;
                output << "变量名: " << currentEntry->idname << "   访问方式: " << currentEntry->attrIR.More.VarAttr.access << "   层级: " << currentEntry->attrIR.More.VarAttr.level << "   偏移量: " << currentEntry->attrIR.More.VarAttr.off << "   类型: " << currentEntry->attrIR.idtype->kind << endl;

                cout << "------------变量符号表------------" << endl;
                cout << "变量名: " << currentEntry->idname << "   访问方式: " << currentEntry->attrIR.More.VarAttr.access << "   层级: " << currentEntry->attrIR.More.VarAttr.level << "   偏移量: " << currentEntry->attrIR.More.VarAttr.off << "   类型: " << currentEntry->attrIR.idtype->kind << endl;

                if (outfile) {
                    fprintf(outfile, "------------变量符号表------------\n");
                    fprintf(outfile, "变量名: %s   访问方式: %d   层级: %d   偏移量: %d   类型: %d\n",
                        currentEntry->idname,
                        currentEntry->attrIR.More.VarAttr.access,
                        currentEntry->attrIR.More.VarAttr.level,
                        currentEntry->attrIR.More.VarAttr.off,
                        currentEntry->attrIR.idtype->kind);
                }
                break;

            case procKind: // 过程符号表
                output << "------------过程符号表------------" << endl;
                output << "过程名: " << currentEntry->idname << "   层级: " << currentEntry->attrIR.More.ProcAttr.level << "   参数表: ";

                cout << "------------过程符号表------------" << endl;
                cout << "过程名: " << currentEntry->idname << "   层级: " << currentEntry->attrIR.More.ProcAttr.level << "   参数表: ";

                if (outfile) {
                    fprintf(outfile, "------------过程符号表------------\n");
                    fprintf(outfile, "过程名: %s   层级: %d   参数表: ",
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

                output << "   代码地址: " << currentEntry->attrIR.More.ProcAttr.code << "   空间大小: " << currentEntry->attrIR.More.ProcAttr.size << endl;

                cout << "   代码地址: " << currentEntry->attrIR.More.ProcAttr.code << "   空间大小: " << currentEntry->attrIR.More.ProcAttr.size << endl;

                if (outfile) {
                    fprintf(outfile, "   代码地址: %d   空间大小: %d\n",
                        currentEntry->attrIR.More.ProcAttr.code,
                        currentEntry->attrIR.More.ProcAttr.size);
                }
                break;

            default:
                output << "未知符号表类型" << endl;
                cout << "未知符号表类型" << endl;
                if (outfile) {
                    fprintf(outfile, "未知符号表类型\n");
                }
                break;
            }

            output << endl; // 打印空行分隔不同符号表项
            cout << endl;
            if (outfile) {
                fprintf(outfile, "\n");
            }

            currentEntry = currentEntry->next; // 移动到下一个符号表项
        }

        output << endl; // 打印空行分隔不同层的符号表
        cout << endl;
        if (outfile) {
            fprintf(outfile, "\n");
        }
    }

    output.flush();
    output.close();

    cout << "符号表打印完成" << endl;
    if (outfile) {
        fprintf(outfile, "符号表打印完成\n");
    }
}
//7.在单个符号表中查找标识符
bool SearchOneTable(char* id, int currentlevel, SymbolTable** Entry) {
    SymbolTable* current = symbolTableStack[currentlevel].next;
    while (current != nullptr) {
        if (strcmp(current->idname, id) == 0) {
            *Entry = current; // 返回找到的符号表项
            return true; // 标识符找到
        }
        current = current->next;
    }
    return false; // 标识符未找到
}




/*-----------------------------------------------------------语义分析----------------------------------------------------------*/
//1.语义分析主函数
void Analyze(TreeNode* currentP) {
    ///////////////////////////////////////////////////////////////////////
    if (currentP == nullptr) {
        cout << "错误: 语法树根节点为空" << endl;
        fprintf(outfile, "错误: 语法树根节点为空\n");
        ///////////////////////////////////////////////////////////////////////////////////
        semanticError++;
        return;
    }

    CreateTable(); //1.1创建一个符号表
    initialize();  //2.2初始化基本类型内部表示函数
    // 循环处理主程序的声明部分
    LoopProgram(currentP);

    ////////////////////////////////////////////

    // 检查是否有语义错误
    if (semanticError > 0) {
        cout << "语义分析结束，共发现 " << semanticError << " 个错误" << endl;
        fprintf(outfile, "语义分析结束，共发现 %d 个错误\n", semanticError);
        exit(1); // 存在错误时退出
    }

    // 处理主程序体
    //WithStm(currentP);
     // 撤销符号表
    //DestroyTable();
}
//66)typedef enum { ProK, PheadK, TypeK, VarK, ProcDecK, StmLK, DecK, StmtK, ExpK }NodeKind;

//遍历语法树，调用相应的函数处理语法树结点
void LoopProgram(TreeNode* t) {
    if (t == nullptr)
        return;
    if (t->nodekind == ProK) {   //根节点
        LoopProgram(t->child[0]);
        LoopProgram(t->child[1]);
        LoopProgram(t->child[2]);
        return;
    }if (t->nodekind == PheadK) {   //程序头
        return;
    }
    if (t->nodekind == TypeK) {     //类型声明
        TypeDecPart(t->child[0]);  //7.类型声明部分分析处理函数
        LoopProgram(t->sibling);
        return;
    }
    if (t->nodekind == VarK) {   //变量声明
        VarDecList(t->child[0]);  //8.变量声明部分分析处理函数
        LoopProgram(t->sibling);
        return;
    }
    if (t->nodekind == ProcDecK) {   //函数声明(过程声明
        //ProcDecPart(t);   //9.过程声明部分分析处理函数
        ProcDecPart(t->child[0]);
        LoopProgram(t->sibling);
        return;
    }

    if (t->nodekind == StmLK) {    //程序体(语句序列
        Body(t->child[0]);   //12.执行体部分分析处理函数
        return;
    }
    if (t->nodekind == StmtK) {  //语句节点  ???????
        Body(t->child[0]);
        return;
    }
}

TypeIR* intPtr, * charPtr, * boolPtr;
//内部表示函数：整数类型，字符类型，布尔类型
//2.初始化基本类型内部表示函数
void initialize(void) {
    // 初始化整数类型
    intPtr = new TypeIR();
    // intPtr->serial = 1;        //在属性表中的位置序号  ?
    intPtr->size = 1;  //类型所占空间大小
    intPtr->kind = intTy;

    // 初始化字符类型
    charPtr = new TypeIR();
    //  charPtr->serial = 2;
    charPtr->size = 1;
    charPtr->kind = charTy;

    // 初始化布尔类型
    boolPtr = new TypeIR();
    // boolPtr->serial = 3;
    boolPtr->size = 1;
    boolPtr->kind = boolTy;
}

//typedef enum { ArrayK, CharK, IntegerK, RecordK, IdK, decNull }  DecKind
//3.类型分析函数
TypeIR* TypeProcess(TreeNode* t, DecKind deckind) {
    TypeIR* typePtr = nullptr;

    if (deckind == IdK) {
        // 处理标识符类型
        typePtr = nameType(t);
    }
    else if (deckind == ArrayK) {
        // 处理数组类型
        typePtr = arrayType(t);
    }
    else if (deckind == RecordK) {
        // 处理记录类型
        typePtr = recordType(t);
    }
    else if (deckind == IntegerK) {
        // 处理整型类型
        return intPtr;
    }
    else if (deckind == CharK) {
        // 处理字符类型
        return charPtr;
    }
    else {
        // 处理其他情况或错误
        cout << "语义错误: 第" << t->lineno << "行，未知或错误类型: '" << deckind << "'  -TypeProcess" << endl;
        fprintf(outfile, "语义错误: 第%d行，未知或错误类型: '%d'  -TypeProcess\n", t->lineno, deckind);
        semanticError++;
    }
    return typePtr;
}

//4.自定义类型内部结构分析函数
TypeIR* nameType(TreeNode* t) {
    SymbolTable** Entry = new SymbolTable*;  //fhb哪里来的

    if (t == nullptr || t->name.empty()) {
        cout << "语义错误:无效的节点  -nameType" << endl;
        cout << "错误: 无效的节点" << endl;
        fprintf(outfile, "语义错误:无效的节点  -nameType\n");
        fprintf(outfile, "错误: 无效的节点\n");
        semanticError++;
        return nullptr;
    }

    for (auto& name : t->name) {
        char* typeName = stringToChar(name);
        // 调用 FindEntry 函数在符号表中查找类型名
        bool present = FindEntry(typeName, false, Entry);
        if (present) {
            // 检查标识符的类型是否为 typeKind
            if ((*Entry)->attrIR.kind == typeKind) {
                // 返回类型内部表示
                return (*Entry)->attrIR.idtype;
            }
            else {
                // 如果不是类型标识符，报非类型标识符错误
                cout << "语义错误: 第" << t->lineno << "行，非类型标识符错误: '" << typeName << "'  -nameType" << endl;
                fprintf(outfile, "语义错误: 第%d行，非类型标识符错误: '%s'  -nameType\n", t->lineno, typeName);
                semanticError++;
                return nullptr;
            }
        }
    }//for

    // 如果遍历完所有 name 仍未找到类型名，报无声明错误
    cout << "语义错误: 第" << t->lineno << "行，自定义类型标识符未声明 -nameType" << endl;
    fprintf(outfile, "语义错误: 第%d行，自定义类型标识符未声明 -nameType\n", t->lineno);
    semanticError++;
    return nullptr;
}

//5.数组类型内部表示处理函数
TypeIR* arrayType(TreeNode* t) {
    // 检查数组上界是否小于下界
    if (t->attr.ArrayAttr.up < t->attr.ArrayAttr.low) {
        cout << "语义错误: 第" << t->lineno << "行，数组上界小于下界 -arrayType" << endl;
        fprintf(outfile, "语义错误: 第%d行，数组上界小于下界 -arrayType\n", t->lineno);
        semanticError++;
    }

    TypeIR* arrayType = new TypeIR();
    arrayType->kind = arrayTy;
    arrayType->More.ArrayAttr.low = t->attr.ArrayAttr.low;
    arrayType->More.ArrayAttr.up = t->attr.ArrayAttr.up;

    arrayType->More.ArrayAttr.indexTy = intPtr;
    arrayType->More.ArrayAttr.elemTy = TypeProcess(t, t->attr.ArrayAttr.childtype);

    // 计算数组长度
    int arraySize = t->attr.ArrayAttr.up - t->attr.ArrayAttr.low;
    arrayType->size = arraySize;

    // 返回数组的内部表示指针
    return arrayType;
}


//6.处理记录类型的内部表示函数
TypeIR* recordType(TreeNode* t) {
    // 新建记录类型节点
    TypeIR* recordType = new TypeIR();
    recordType->kind = recordTy;
    recordType->size = 0; // 初始大小为 0
    FieldChain* lastField = nullptr; // 用于链接域链

    // 遍历记录体中的域变量
    TreeNode* currentField = t->child[0]; // 假设第一个子节点是记录体的开始
    //int recordoffset = 0;   //记录字段的偏移量
    while (currentField != nullptr) {
        // 处理一个节点中有多个 ID 的情况
        for (auto& Name : currentField->name) {
            //类型转换
            char* idName = stringToChar(Name);
            //  if(lastField !=nullptr){
              // 申请新的域类型单元结构
            FieldChain* newField = new FieldChain();
            //newField->idname = idName; //char   char*???
            strcpy_s(newField->idname, idName);
            // 处理域类型
            newField->unitType = TypeProcess(currentField, currentField->kind.dec);
            if (newField->unitType == nullptr) {
                //error
                cout << "语义错误: 第" << t->lineno << "行，无效的字段类型: '" << idName << "'  -arrayType" << endl;
                fprintf(outfile, "语义错误: 第%d行，无效的字段类型: '%s'  -arrayType\n", t->lineno, idName);
                semanticError++;
                return nullptr;
            }

            // 计算偏移量    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            newField->offset = recordType->size;
            recordType->size += newField->unitType->size;

            // 链接到域链
            if (lastField == nullptr) {
                recordType->More.body = newField; // 第一个域
            }
            else {
                lastField->next = newField; // 链接到上一个域
            }
            lastField = newField;
        }

        // 移动到下一个兄弟节点
        currentField = currentField->sibling;
    }

    // 返回记录类型的内部表示指针
    return recordType;
}

//7.类型声明部分分析处理函数
void TypeDecPart(TreeNode* t) {
    // 初始化 present 为 0，属性类型标记为 typeKind
    bool present = false;

    // 遍历类型声明部分
    while (t != nullptr) {
        SymbolTable** Entry = new SymbolTable*;

        if (!t->name.empty()) {
            for (auto& name : t->name) {
                char* typeName = stringToChar(name);
                // 调用登记属性函数 enter
                present = FindEntry(typeName, false, Entry);   //检查本层
                if (present) {
                    // 如果 present = 1，表示重复声明，报错
                    cout << "语义错误: 第" << t->lineno << "行，重复声明: '" << typeName << "'  -TypeDecPart" << endl;
                    fprintf(outfile, "语义错误: 第%d行，重复声明: '%s'  -TypeDecPart\n", t->lineno, typeName);
                    semanticError++;
                }
                else {
                    AttributeIR* AttribT = new AttributeIR;
                    AttribT->kind = typeKind;
                    //flagtree = true;//类型为TRUE
                    AttribT->idtype = TypeProcess(t, t->kind.dec);
                    //flagtree = false;
                    Enter(typeName, AttribT, Entry, t);
                }
            }
        }

        // 取下一声明
        t = t->sibling;
    }
    return;
}

//8.变量声明部分分析处理函数
void VarDecList(TreeNode* t) {
    while (t != nullptr) {
        // 检查当前节点的类型是否为变量声明
        if (t->nodekind == DecK) {
            SymbolTable** Entry = new SymbolTable*;

            if (!t->name.empty()) {
                for (auto& name : t->name) {
                    char* typeName = stringToChar(name);
                    // 调用类型处理函数 TypeProcess，构造变量类型的内部表示
                    TypeIR* varType = TypeProcess(t, t->kind.dec);
                    bool flag = FindEntry(typeName, false, Entry);//判断变量标识符是否存在
                    if (flag)
                    {
                        // 如果 present = 1，表示重复声明，报错
                        cout << "语义错误: 第" << t->lineno << "行，变量标识符重复声明: '" << typeName << "'  -VarDecList" << endl;
                        fprintf(outfile, "语义错误: 第%d行，变量标识符重复声明: '%s'  -VarDecList\n", t->lineno, typeName);
                        semanticError++;
                    }
                    else {
                        AttributeIR* AttribV = new AttributeIR;
                        AttribV->kind = varKind;
                        AttribV->idtype = varType;
                        AttribV->More.VarAttr.access = Dir;//直接和间接怎么判断?????
                        AttribV->More.VarAttr.level = NowTableLevel;
                        if (AttribV->idtype != nullptr)
                            // AttribV->More.VarAttr.off = AttribV->More.VarAttr.off + AttribV->idtype->size;//计算偏移
                            AttribV->More.VarAttr.off = off + AttribV->idtype->size;
                        //off += AttribV->idtype->size; // 更新全局offset!!!!!!!!!!!!!!!!!!!!!!???????????????
                        Enter(typeName, AttribV, Entry, t);//加入符号表和栈
                    }
                }
            }
        }
        t = t->sibling;
    }
    return;
}

//9.过程声明部分分析处理函数
void ProcDecPart(TreeNode* t) {
    if (t == nullptr) {
        return;
    }
    else {
        SymbolTable* Entry = HeadProcess(t);
    }

    //处理形参，并将形参表给函数的符号表内的param项
    if (t != nullptr) {
        LoopProgram(t->child[1]);    //自会循环
    }

    // 处理下一个过程
    if (t == nullptr || t->child[2] == NULL) {
        return;
    }
    else {
        Body(t->child[2]->child[0]); //遍历语句
    }
    return;
}

//10.过程声明头分析函数
SymbolTable* HeadProcess(TreeNode* t) {
    if (t == nullptr || t->name.empty()) {
        cout << "语义错误: 过程头节点无效 -HeadProcess" << endl;
        fprintf(outfile, "语义错误: 过程头节点无效 -HeadProcess\n");
        semanticError++;
        return nullptr;
    }
    else {
        SymbolTable** Entry = new SymbolTable*;
        char* typeName = stringToChar(t->name[0]);
        // 设置过程标识符的属性
        AttributeIR* AttribP = new AttributeIR();
        AttribP->kind = procKind;
        AttribP->More.ProcAttr.level = NowTableLevel;
        //参数表
        ParamTable* paramt = new ParamTable();
        AttribP->More.ProcAttr.param = paramt;

        // 登记过程标识符到符号表
        Enter(typeName, AttribP, Entry, t);//加入符号表和栈

        //下面写处理过程
        CreateTable();
        //处理形参，并将形参表给函数的符号表内的param项
        (*Entry)->attrIR.More.ProcAttr.param = ParaDecList(t);//处理形参   //这里不是孩子para里面需要孩子

        return (*Entry);
    }
}

// 11.形参分析处理函数
ParamTable * ParaDecList(TreeNode * t) {
    ParamTable* head = nullptr; // 形参符号表链的头指针
    ParamTable* tail = nullptr; // 形参符号表链的尾指针

    if (t->child[0] != nullptr) {
        TreeNode* p = t->child[0];

        // 遍历形参声明部分
        while (p != nullptr) {
            // 遍历当前节点中的所有形参名
            for (int i = 0; i < p->idnum; ++i) {  //这样的话之前可以判断了
                std::string paramName = p->name[i]; // 获取形参名
                char* typeName = stringToChar(paramName);

                SymbolTable** Entry = new SymbolTable*;
                bool flag = FindEntry(typeName, false, Entry);
                if (flag) {
                    //error
                    cout << "语义错误: 第" << t->lineno << "行，形参标识符重复声明: '" << typeName << "'  -ParaDecList" << endl;
                    fprintf(outfile, "语义错误: 第%d行，形参标识符重复声明: '%s'  -ParaDecList\n", t->lineno, typeName);
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

                    // 将新形参符号表项添加到形参符号表链中
                    if (head == nullptr) {
                        head = newParam; // 如果是第一个形参，设置为头指针
                    }
                    else {
                        tail->next = newParam; // 否则链接到尾部
                    }
                    newParam->entry = (*Entry);
                    newParam->next = NULL;
                    tail = newParam; // 更新尾指针
                }
            }
            // 取下一个形参声明
            p = p->sibling;
        }
    }
    // 返回形参符号表链的头指针
    return head;
}

//12.执行体部分分析处理函数
void Body(TreeNode* t) {
    if (t == nullptr) return;
    while (t != nullptr) {
        statement(t);
        t = t->sibling;
    }
}

//13.语句序列分析处理函数
void statement(TreeNode* t) {
    if (t == nullptr) {
        return; // 如果节点为空，直接返回
    }

    // 根据语句类型调用相应的处理函数
    switch (t->kind.stmt) {
    case IfK:
        // 处理 if 语句
        ifstatement(t);
        break;
    case WhileK:
        // 处理 while 语句
        whilestatement(t);
        break;
    case AssignK:
        // 处理赋值语句
        assignstatement(t);
        break;
    case ReadK:
        // 处理读语句
        readstatement(t);
        break;
    case WriteK:
        // 处理写语句
        writestatement(t);
        break;
    case CallK:
        // 处理调用语句
        callstatement(t);
        break;
    case ReturnK:
        // 处理返回语句
        returnstatement(t);
        break;
    default:
        // 未知语句类型
        cout << "语义错误: 第" << t->lineno << "行，未知语句类型 -statement" << endl;
        fprintf(outfile, "语义错误: 第%d行，未知语句类型 -statement\n", t->lineno);
        semanticError++;
        break;
    }
    return;
    //递归处理兄弟节点（语句序列中的下一条语句）
    //statement(t->sibling);
}

//14.表达式分析处理函数
TypeIR* Expr(TreeNode* t, AccessKind* Ekind) {
    if (t == nullptr) {
        return nullptr; // 如果节点为空，直接返回
    }
    TypeIR* Eptr = nullptr; // 表达式类型内部表示的指针

    //?????????????????要不要进行检查find????????????????????????????

    // 表达式为常量设置访问方式为直接访问(Dir)返回整型类型指针(intPtr)
    if (t->kind.exp == ConstK) {
        (*Ekind) = Dir;
        ///////////////////////////////////////////////////////////////////////////
        if (t->attr.ExpAttr.ischar == false) {
            // 处理整型类型
            Eptr = intPtr;
        }
        else if (t->attr.ExpAttr.ischar == true) {
            // 处理字符类型
            Eptr = charPtr;
        }
        return Eptr;
    }
    // 表达式为变量在符号表中查找变量名 处理不同变量类型：
    else if (t->kind.exp == VariK) {

        SymbolTable** Entry = new SymbolTable*;

        char* typeName = stringToChar(t->name[0]);
        // 调用 FindEntry 函数在符号表中查找类型名
        bool flag = FindEntry(typeName, true, Entry);//判断标识符是否以声明
        if (!flag)
        {
            cout << "语义错误: 第" << t->lineno << "行，未声明变量: '" << typeName << "' -Expr" << endl;
            fprintf(outfile, "语义错误: 第%d行，未声明变量: '%s' -Expr\n", t->lineno, typeName);
            semanticError++;
        }

        else {//////////////////////////////////////////////////////////////////////////////////
            // 检查是否有子节点（如 a[i] 或 a.i）
            if (t->child[0] != nullptr) {
                //数组类型返回元素的内部表示的kind
                //检查数组索引表达式的类型   返回数组元素的类型
                // 情况1：数组访问 a[...]
                if ((*Entry)->attrIR.idtype->kind == arrayTy) {
                    // 检查子节点是否是表达式（如 a[1] 或 a[i]）
                    if (t->attr.ExpAttr.varkind == ArrayMembV) {
                        Expr(t->child[0], Ekind);
                        if (Expr(t->child[0], Ekind) != nullptr) {
                            if (Expr(t->child[0], Ekind)->kind == arrayTy)
                                return Expr(t->child[0], Ekind);
                        }

                        return (*Entry)->attrIR.idtype->More.ArrayAttr.elemTy; // 返回数组元素类型
                    }
                    // 情况2：非法点操作符 a.i
                    else {
                        cout << "语义错误: 第" << t->lineno << "行，数组 '" << t->name[0]
                            << "' 不能使用 '.' 访问字段" << endl;
                        fprintf(outfile, "语义错误: 第%d行，数组 '%s' 不能使用 '.' 访问字段\n",
                            t->lineno, t->name[0].c_str());
                        semanticError++;
                        return nullptr;
                    }

                }


                //记录  在记录类型中查找字段名 返回字段的类型
                else if ((*Entry)->attrIR.idtype->kind == recordTy) {
                    FieldChain* aa = (*Entry)->attrIR.idtype->More.body;
                    while (aa != nullptr)
                    {
                        if (aa->idname == t->child[0]->name[0])
                            return aa->unitType;
                    }
                    //error
                    //cout << "语义错误: 记录中没有此元素:" << typeName << "-第%d行，-Expr\n" << t->lineno << endl;
                    // printf("语义错误：第%d行，-Expr\n", t->lineno);
                    cout << "语义错误: 第" << t->lineno << "行，记录中没有此元素: '" << typeName << "' -Expr" << endl;
                    fprintf(outfile, "语义错误: 第%d行，记录中没有此元素: '%s' -Expr\n", t->lineno, typeName);
                    semanticError++;
                }
            }
            else {

                // 处理 Var = id 的情形（简单变量）
               // SymbolTable* entry = FindEntry(t->name[0]); // 在符号表中查找标识符

                if (Entry != nullptr && (*Entry)->attrIR.kind == varKind) {
                    *Ekind = InDir; // 设置为间接访问
                }
                else {
                    cout << "语义错误: 第" << t->lineno << "行，未知变量: '" << typeName << "' -Expr" << endl;
                    fprintf(outfile, "语义错误: 第%d行，未知变量: '%s' -Expr\n", t->lineno, typeName);
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
            cout << "语义错误: 第" << t->lineno << "行，表达式语句类型不匹配 -Expr" << endl;
            fprintf(outfile, "语义错误: 第%d行，表达式语句类型不匹配 -Expr\n", t->lineno);
            semanticError++;
        }

    }
    return nullptr;
}

//15.数组变量的分析处理函数//////////////////////////name///////////////////////////////
//检查var:=var0[E]中var0是不是数组类型变量，E是不是和数组的下标变量类型匹配。
TypeIR* arrayVar(TreeNode* t) {
    //检查var0是不是数组类型变量
    string arrayName = t->name[0]; //数组变量名
    SymbolTable* arrayEntry = new SymbolTable();
    bool found = FindEntry(stringToChar(arrayName), true, &arrayEntry);//查找所有符号表

    if (!found) {
        cout << "语义错误: 第" << t->lineno << "行，数组变量 '" << arrayName << "' 未声明 -arrayVar" << endl;
        fprintf(outfile, "语义错误: 第%d行，数组变量 '%s' 未声明 -arrayVar\n", t->lineno, arrayName.c_str());
        semanticError++;
    }

    //检查数组变量是否为变量
    if (arrayEntry->attrIR.kind != varKind) {
        cout << "语义错误: 第" << t->lineno << "行，'" << arrayName << "' 不是变量标识符 -arrayVar" << endl;
        fprintf(outfile, "语义错误: 第%d行，'%s' 不是变量标识符 -arrayVar\n", t->lineno, arrayName.c_str());
        semanticError++;
    }

    //检查数组变量是否为数组类型
    if (arrayEntry->attrIR.idtype->kind != TypeKind::arrayTy) {
        cout << "语义错误: 第" << t->lineno << "行，'" << arrayName << "' 不是数组类型变量 -arrayVar" << endl;
        fprintf(outfile, "语义错误: 第%d行，'%s' 不是数组类型变量 -arrayVar\n", t->lineno, arrayName.c_str());
        semanticError++;
        //return nullptr;
    }

    //检查E是不是和数组的下标变量类型匹配
    //获取数组的下标表达式节点
    TreeNode* indexNode = t->child[0]; // 下标表达式是第一个子节点

    if (indexNode == nullptr) {
        cout << "语义错误: 第" << t->lineno << "行，数组变量中缺少下标表达式 -arrayVar" << endl;
        fprintf(outfile, "语义错误: 第%d行，数组变量中缺少下标表达式 -arrayVar\n", t->lineno);
        semanticError++;
        //return nullptr;
    }
    else {////////////////////////////////////////////////////////////////////////////
        AccessKind* ekind = new AccessKind;
        TypeIR* indexType = Expr(indexNode, ekind); // 分析下标表达式的类型
        if (indexNode->idnum != 0) {//下标表达式不是常量表达式
            SymbolTable* indexEntry = new SymbolTable;
            bool flag = FindEntry(stringToChar(indexNode->name[0]), true, &indexEntry);//判断标识符是否声明

            //分析下标表达式中的变量是否声明
            if (!flag) {
                cout << "语义错误: 第" << t->lineno << "行，数组下标变量" << t->child[0]->name[0] << "未声明 -arrayVar" << endl;
                fprintf(outfile, "语义错误: 第%d行，数组下标变量%s未声明 -arrayVar\n",
                    t->lineno, t->child[0]->name[0].c_str());
                semanticError++;
                //return nullptr;
            }

            //分析下标表达式中的变量是否为整型
            if (indexEntry->attrIR.idtype->kind != TypeKind::intTy) {
                cout << "语义错误: 第" << t->lineno << "行，数组下标变量" << t->child[0]->name[0] << "不是整型 -arrayVar" << endl;
                fprintf(outfile, "语义错误: 第%d行，数组下标变量%s不是整型 -arrayVar\n",
                    t->lineno, t->child[0]->name[0].c_str());
                semanticError++;
                //return nullptr;
            }

            //分析下标表达式的类型
            if (indexType == nullptr) {
                cout << "语义错误: 第" << t->lineno << "行，无法分析下标表达式的类型 -arrayVar" << endl;
                fprintf(outfile, "语义错误: 第%d行，无法分析下标表达式的类型 -arrayVar\n", t->lineno);
                semanticError++;
                //return nullptr;
            }

            // 检查下标表达式类型是否与数组下标类型匹配
            if (indexType == nullptr) {
                cout << "语义错误: 第" << t->lineno << "行，下标表达式为空 -arrayVar" << endl;
                fprintf(outfile, "语义错误: 第%d行，下标表达式为空 -arrayVar\n", t->lineno);
                semanticError++;
            }
            else {
                if (indexType->kind != TypeKind::intTy) {
                    cout << "语义错误: 第" << t->lineno << "行，下标表达式不是整型 -arrayVar" << endl;
                    fprintf(outfile, "语义错误: 第%d行，下标表达式不是整型 -arrayVar\n", t->lineno);
                    semanticError++;
                    //return nullptr;
                }
            }
        }
        else {//数组下标是常量表达式，判断是否越界
            if (indexType == nullptr) {
                cout << "语义错误: 第" << t->lineno << "行，下标表达式为空 -arrayVar" << endl;
                fprintf(outfile, "语义错误: 第%d行，下标表达式为空 -arrayVar\n", t->lineno);
                semanticError++;
            }
            else {
                if (indexType->kind != TypeKind::intTy) {
                    cout << "语义错误: 第" << t->lineno << "行，下标表达式不是整型 -arrayVar" << endl;
                    fprintf(outfile, "语义错误: 第%d行，下标表达式不是整型 -arrayVar\n", t->lineno);
                    semanticError++;
                    //return nullptr;
                }
                else {/////////////////////////////////////////////////////////////////////////////
                    int arrayup = arrayEntry->attrIR.idtype->More.ArrayAttr.up - 1;
                    int arraylow = arrayEntry->attrIR.idtype->More.ArrayAttr.low - 1;
                    if (indexNode->attr.ExpAttr.val<arraylow || indexNode->attr.ExpAttr.val>arrayup) {
                        cout << "语义错误: 第" << t->lineno << "行，数组下标越界 -arrayVar" << endl;
                        fprintf(outfile, "语义错误: 第%d行，数组下标越界 -arrayVar\n", t->lineno);
                        semanticError++;
                    }
                }
            }
        }
    }
    // 返回指向数组元素类型内部表示的指针
    return arrayEntry->attrIR.idtype;
}
//16.记录变量中域变量的分析处理函数
TypeIR* recordVar(TreeNode* t) {
    //获取记录变量名
    if (t->name.empty()) {
        cout << "语义错误: 第" << t->lineno << "行，记录变量中缺少变量名及域名 -recordVar" << endl;
        fprintf(outfile, "语义错误: 第%d行，记录变量中缺少变量名及域名 -recordVar\n", t->lineno);
        semanticError++;
        return nullptr;
    }

    string varName = t->name[0];
    SymbolTable* varEntry = new SymbolTable;

    bool found = FindEntry(stringToChar(varName), true, &varEntry);//在所有符号表中查找标识符是否声明

    //在符号表中查找记录变量
    if (!found) {
        cout << "语义错误: 第" << t->lineno << "行，记录变量 '" << varName << "' 未声明 -recordVar" << endl;
        fprintf(outfile, "语义错误: 第%d行，记录变量 '%s' 未声明 -recordVar\n", t->lineno, varName.c_str());
        semanticError++;
    }

    //检查记录变量是否为变量
    if (varEntry->attrIR.kind != varKind) {
        cout << "语义错误: 第" << t->lineno << "行，'" << varName << "' 不是变量 -recordVar" << endl;
        fprintf(outfile, "语义错误: 第%d行，'%s' 不是变量 -recordVar\n", t->lineno, varName.c_str());
        semanticError++;
    }

    //检查记录变量是否为记录类型
    if (varEntry->attrIR.idtype->kind != TypeKind::recordTy) {
        cout << "语义错误: 第" << t->lineno << "行，'" << varName << "' 不是记录类型变量 -recordVar" << endl;
        fprintf(outfile, "语义错误: 第%d行，'%s' 不是记录类型变量 -recordVar\n", t->lineno, varName.c_str());
        semanticError++;
    }

    //获取记录类型的域表
    FieldChain* fieldChain = varEntry->attrIR.idtype->More.body;

    if (fieldChain == nullptr) {
        cout << "语义错误: 第" << t->lineno << "行，记录类型 '" << varName << "' 的域表为空 -recordVar" << endl;
        fprintf(outfile, "语义错误: 第%d行，记录类型 '%s' 的域表为空 -recordVar\n", t->lineno, varName.c_str());
        semanticError++;
    }

    TreeNode* fieldNode = t->child[0];

    while (fieldNode != nullptr) {
        if (fieldNode->idnum != 0) {//判断当前节点是否有域名
            SymbolTable* Entry1 = new SymbolTable;
            bool flag = FindEntry(stringToChar(fieldNode->name[0]), true, &Entry1);//在符号表中查找该域名是否已声明
            if (!flag) {
                cout << "语义错误: 第" << t->lineno << "行，域变量 '" << fieldNode->name[0] << "' 未声明 -recordVar" << endl;
                fprintf(outfile, "语义错误: 第%d行，域变量 '%s' 未声明 -recordVar\n",
                    t->lineno, fieldNode->name[0].c_str());
                semanticError++;
            }
            else {
                //在域表中查找域名
                FieldChain* foundDomain = new FieldChain;
                bool fieldFound = FindField(stringToChar(fieldNode->name[0]), fieldChain, &foundDomain);//在域表中查找域名
                if (!fieldFound) {//没找到
                    cout << "语义错误: 第" << t->lineno << "行，未在域表中找到域变量 '" << fieldNode->name[0] << "' -recordVar" << endl;
                    fprintf(outfile, "语义错误: 第%d行，未在域表中找到域变量 '%s' -recordVar\n",
                        t->lineno, fieldNode->name[0].c_str());
                    semanticError++;
                    break;
                }
            }
        }
        //else {
        //    cout << "语义错误: 第" << t->lineno << "行，当前节点域表中没有域名 -recordVar" << endl;
        //    semanticError++;
        //}
        fieldNode = fieldNode->child[0];
    }

    //返回指向记录元素类型内部表示的指针
    return varEntry->attrIR.idtype;
}
//17.赋值语句分析函数
void assignstatement(TreeNode* t) {
    //获取赋值语句的左值和右值节点
    TreeNode* leftChild = t->child[0]; //左值是第一个子节点
    TreeNode* rightChild = t->child[1]; //右值是第二个子节点

    if (leftChild == nullptr || rightChild == nullptr) {
        cout << "语义错误: 第" << t->lineno << "行，赋值语句缺少左值或右值 -assignstatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，赋值语句缺少左值或右值 -assignstatement\n", t->lineno);
        semanticError++;
    }

    //检查左值是否为标识符
    if (leftChild->nodekind != ExpK || leftChild->kind.exp != VariK) {
        cout << "语义错误: 第" << t->lineno << "行，赋值语句的左值不是标识符 -assignstatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，赋值语句的左值不是标识符 -assignstatement\n", t->lineno);
        semanticError++;
    }
    else {
        //在符号表中查找左值标识符
        SymbolTable* leftEntry = new SymbolTable;
        bool found = FindEntry(stringToChar(leftChild->name[0]), true, &leftEntry);

        if (!found) {
            cout << "语义错误: 第" << t->lineno << "行，标识符 '" << leftChild->name[0] << "' 未声明 -assignstatement" << endl;
            fprintf(outfile, "语义错误: 第%d行，标识符 '%s' 未声明 -assignstatement\n",
                t->lineno, leftChild->name[0].c_str());
            semanticError++;
        }
        else {
            //获取左值的类型
            TypeIR* leftType = leftEntry->attrIR.idtype;
            TypeIR* leftKind = leftType;

            //检查左端标识符是否为变量
            if (leftEntry->attrIR.kind != varKind) {
                cout << "语义错误: 第" << t->lineno << "行，赋值语句的左端标识符不是变量 -assignstatement" << endl;
                fprintf(outfile, "语义错误: 第%d行，赋值语句的左端标识符不是变量 -assignstatement\n", t->lineno);
                semanticError++;
            }

            if (leftChild->child[0] != nullptr) {
                if (leftType->kind == arrayTy) {//判断是不是数组类型
                    AccessKind* ekind = new AccessKind;
                    leftKind = Expr(leftChild->child[0], ekind);
                    TreeNode* EXP = leftChild->child[0];
                    if (EXP->kind.exp == ConstK) {
                        arrayVar(leftChild);
                    }
                }
                else if (leftType->kind == recordTy) {//判断是不是记录类型
                    FieldChain* leftField = leftType->More.body;
                    bool flag = true;
                    while (leftField != nullptr) {
                        if (leftField->idname == leftChild->child[0]->name[0]) {//判断域中是否存在该元素
                            leftKind = leftField->unitType;
                            flag = false;
                            break;
                        }
                        leftField = leftField->next;
                    }
                    if (flag) {
                        cout << "语义错误: 第" << t->lineno << "行，记录类型的域中没有赋值语句左端元素 -assignstatement" << endl;
                        fprintf(outfile, "语义错误: 第%d行，记录类型的域中没有赋值语句左端元素 -assignstatement\n", t->lineno);
                        semanticError++;
                    }
                    recordVar(leftChild);
                }
            }

            //处理右值表达式
            AccessKind Ekind;
            TypeIR* rightType = Expr(rightChild, &Ekind); //分析右值表达式的类型
            if (rightType == nullptr) {
                cout << "语义错误: 第" << t->lineno << "行，无法分析右值表达式的类型 -assignstatement" << endl;
                fprintf(outfile, "语义错误: 第%d行，无法分析右值表达式的类型 -assignstatement\n", t->lineno);
                semanticError++;
            }
            else {
                if (rightChild == nullptr) {
                    cout << "语义错误: 第" << t->lineno << "行，右值表达式为空 -assignstatement" << endl;
                    fprintf(outfile, "语义错误: 第%d行，右值表达式为空 -assignstatement\n", t->lineno);
                    semanticError++;
                }
                else {
                    if (rightChild->idnum != 0) {
                        // 当前已注释的代码
                    }
                    else {
                        // 空代码块
                    }
                }
            }
            //检查赋值号两端的类型是否相容
            if (leftKind != rightType) {
                cout << "语义错误: 第" << t->lineno << "行，赋值号两端的类型不匹配 -assignstatement" << endl;
                fprintf(outfile, "语义错误: 第%d行，赋值号两端的类型不匹配 -assignstatement\n", t->lineno);
                semanticError++;
            }
        }
    }
}

//18.过程调用语句分析处理函数
void callstatement(TreeNode* t) {
    //获取过程名
    if (t->name.empty()) {
        /*cout << "语义错误: 第" << t->lineno << "行，过程调用语句中缺少过程名 -callstatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，过程调用语句中缺少过程名 -callstatement\n", t->lineno);*/
        semanticError++;
    }

    string procName = t->name[0];
    SymbolTable* procEntry = new SymbolTable;
    bool found = FindEntry(stringToChar(procName), true, &procEntry);//判断过程标识符是否声明

    if (!found) {
        cout << "语义错误: 第" << t->lineno << "行，过程 '" << procName << "' 未声明 -callstatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，过程 '%s' 未声明 -callstatement\n", t->lineno, procName.c_str());
        semanticError++;
    }
    else {
        //检查标识符是否为过程名
        if (procEntry->attrIR.kind != procKind) {
            cout << "语义错误: 第" << t->lineno << "行，标识符'" << procName << "' 不是过程名 -callstatement" << endl;
            fprintf(outfile, "语义错误: 第%d行，标识符'%s' 不是过程名 -callstatement\n", t->lineno, procName.c_str());
            semanticError++;
        }
        else {
            //获取形参表
            ParamTable* paramTable = procEntry->attrIR.More.ProcAttr.param;
            if (paramTable == nullptr) {
                cout << "语义错误: 第" << t->lineno << "行，过程 '" << procName << "' 没有形参表 -callstatement" << endl;
                fprintf(outfile, "语义错误: 第%d行，过程 '%s' 没有形参表 -callstatement\n", t->lineno, procName.c_str());
                semanticError++;
            }

            //获取实参列表
            TreeNode* actualParamNode = t->child[0]; //实参列表是第一个子节点
            if (actualParamNode == nullptr) {
                cout << "语义错误: 第" << t->lineno << "行，过程调用语句中缺少实参列表 -callstatement" << endl;
                fprintf(outfile, "语义错误: 第%d行，过程调用语句中缺少实参列表 -callstatement\n", t->lineno);
                semanticError++;
            }

            //遍历形参表和实参列表，检查形实参对应关系
            ParamTable* currentParam = paramTable;
            TreeNode* currentActualParam = actualParamNode;
            while (currentParam != nullptr && currentActualParam != nullptr) {
                //获取形参的类型
                TypeIR* paramType = currentParam->entry->attrIR.idtype;
                if (paramType == nullptr) {
                    cout << "语义错误: 第" << t->lineno << "行，形参类型为空 -callstatement" << endl;
                    fprintf(outfile, "语义错误: 第%d行，形参类型为空 -callstatement\n", t->lineno);
                    semanticError++;
                }

                //分析实参的类型
                AccessKind ekind;
                TypeIR* actualParamType = Expr(currentActualParam, &ekind);
                if (actualParamType == nullptr) {
                    cout << "语义错误: 第" << t->lineno << "行，无法分析实参类型 -callstatement" << endl;
                    fprintf(outfile, "语义错误: 第%d行，无法分析实参类型 -callstatement\n", t->lineno);
                    semanticError++;
                }

                if (paramType != actualParamType) {
                    if (paramType == nullptr) {
                        cout << "语义错误: 第" << t->lineno << "行，形参类型为空 -callstatement" << endl;
                        fprintf(outfile, "语义错误: 第%d行，形参类型为空 -callstatement\n", t->lineno);
                        semanticError++;
                    }
                    else if (actualParamType == nullptr) {
                        cout << "语义错误: 第" << t->lineno << "行，实参类型为空 -callstatement" << endl;
                        fprintf(outfile, "语义错误: 第%d行，实参类型为空 -callstatement\n", t->lineno);
                        semanticError++;
                    }
                    else {
                        if (paramType->kind == arrayTy && actualParamType->kind == arrayTy) {//如果两者类型均为数组类型
                            if (paramType->size == actualParamType->size && paramType->More.ArrayAttr.elemTy == actualParamType->More.ArrayAttr.elemTy)
                                ;
                            else {
                                cout << "语义错误: 第" << t->lineno << "行，形参和实参不匹配 -callstatement" << endl;
                                fprintf(outfile, "语义错误: 第%d行，形参和实参不匹配 -callstatement\n", t->lineno);
                                semanticError++;
                            }
                        }
                        else {
                            cout << "语义错误: 第" << t->lineno << "行，形参和实参不匹配 -callstatement" << endl;
                            fprintf(outfile, "语义错误: 第%d行，形参和实参不匹配 -callstatement\n", t->lineno);
                            semanticError++;
                        }
                    }
                }

                //检查形参和实参类型是否匹配
                if (paramType == nullptr) {
                    cout << "语义错误: 第" << t->lineno << "行，形参类型为空 -callstatement" << endl;
                    fprintf(outfile, "语义错误: 第%d行，形参类型为空 -callstatement\n", t->lineno);
                    semanticError++;
                }
                else if (actualParamType == nullptr) {
                    cout << "语义错误: 第" << t->lineno << "行，实参类型为空 -callstatement" << endl;
                    fprintf(outfile, "语义错误: 第%d行，实参类型为空 -callstatement\n", t->lineno);
                    semanticError++;
                }
                else {
                    if (paramType->kind != actualParamType->kind) {
                        cout << "语义错误: 第" << t->lineno << "行，形参和实参类型不匹配 -callstatement" << endl;
                        fprintf(outfile, "语义错误: 第%d行，形参和实参类型不匹配 -callstatement\n", t->lineno);
                        semanticError++;
                    }
                }

                //移动到下一个形参和实参
                currentParam = currentParam->next;
                currentActualParam = currentActualParam->sibling;
            }

            //检查形参和实参数量是否一致
            if (currentParam != nullptr || currentActualParam != nullptr) {
                cout << "语义错误: 第" << t->lineno << "行，形参和实参数量不匹配 -callstatement" << endl;
                fprintf(outfile, "语义错误: 第%d行，形参和实参数量不匹配 -callstatement\n", t->lineno);
                semanticError++;
            }
            return;
        }
    }
}

//19.条件语句分析处理函数
void ifstatement(TreeNode* t) {
    //获取条件表达式节点
    TreeNode* conditionNode = t->child[0]; //条件表达式是第一个子节点
    if (conditionNode == nullptr) {
        cout << "语义错误: 第" << t->lineno << "行，条件语句中的条件表达式为空 -ifstatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，条件语句中的条件表达式为空 -ifstatement\n", t->lineno);
        semanticError++;
    }
    else {
        //分析条件表达式的类型
        AccessKind ekind;
        TypeIR* conditionType = Expr(conditionNode, &ekind); //调用表达式分析函数

        if (conditionType == nullptr) {
            cout << "语义错误: 第" << t->lineno << "行，无法分析条件表达式的类型 -ifstatement" << endl;
            fprintf(outfile, "语义错误: 第%d行，无法分析条件表达式的类型 -ifstatement\n", t->lineno);
            semanticError++;
        }

        SymbolTable* Entry = new SymbolTable;

        if (conditionNode->idnum != 0) {
            for (int i = 0; i < conditionNode->name.size(); i++) {
                bool found = FindEntry(stringToChar(conditionNode->name[i]), true, &Entry);
                if (!found) {
                    cout << "语义错误: 第" << t->lineno << "行，变量 '" << conditionNode->name[i] << "' 未声明 -ifstatement" << endl;
                    fprintf(outfile, "语义错误: 第%d行，变量 '%s' 未声明 -ifstatement\n",
                        t->lineno, conditionNode->name[i].c_str());
                    semanticError++;
                }
                else {
                    if (Entry->attrIR.idtype->kind != TypeKind::intTy && Entry->attrIR.idtype->kind != TypeKind::boolTy) {
                        cout << "语义错误: " << "第" << t->lineno << "行，条件语句中的条件表达式不是布尔类型 -ifstatement" << endl;
                        fprintf(outfile, "语义错误: 第%d行，条件语句中的条件表达式不是布尔类型 -ifstatement\n", t->lineno);
                        semanticError++;
                    }
                }
            }
        }
        else {
            if (conditionNode->nodekind != NodeKind::ExpK) {
                cout << "语义错误: " << "第" << t->lineno << "行，条件语句中的条件表达式不是布尔类型 -ifstatement" << endl;
                fprintf(outfile, "语义错误: 第%d行，条件语句中的条件表达式不是布尔类型 -ifstatement\n", t->lineno);
                semanticError++;
            }
        }

        //处理 then 语句序列部分
        TreeNode* thenStmt = t->child[1]; //then 语句序列是第二个子节点
        if (thenStmt == nullptr) {
            cout << "语义错误: 第" << t->lineno << "行，条件语句中的 then 语句序列为空 -ifstatement" << endl;
            fprintf(outfile, "语义错误: 第%d行，条件语句中的 then 语句序列为空 -ifstatement\n", t->lineno);
            semanticError++;
        }
        else {
            while (thenStmt != nullptr) {
                //递归处理 then 语句序列中的每个语句
                statement(thenStmt);
                thenStmt = thenStmt->sibling;
            }
        }

        //处理 else 语句序列部分（如果有）
        TreeNode* elseStmt = t->child[2]; //else 语句序列是第三个子节点
        if (elseStmt == nullptr) {
            cout << "语义错误: 第" << t->lineno << "行，条件语句中的 else 语句序列为空 -ifstatement" << endl;
            fprintf(outfile, "语义错误: 第%d行，条件语句中的 else 语句序列为空 -ifstatement\n", t->lineno);
            semanticError++;
        }
        else {
            while (elseStmt != nullptr) {
                //递归处理 else 语句序列中的每个语句
                statement(elseStmt);
                elseStmt = elseStmt->sibling;
            }
        }
    }
    return;
}

//20.循环语句分析处理函数
void whilestatement(TreeNode* t) {
    //获取条件表达式节点
    TreeNode* conditionNode = t->child[0];
    if (conditionNode == nullptr) {
        cout << "语义错误: " << "第" << t->lineno << "行，循环语句中的条件表达式为空 -whilestatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，循环语句中的条件表达式为空 -whilestatement\n", t->lineno);
        semanticError++;
    }

    //分析条件表达式的类型
    AccessKind ekind;
    TypeIR* conditionType = Expr(conditionNode, &ekind); //调用表达式分析函数

    if (conditionType == nullptr) {
        cout << "语义错误: " << "第" << t->lineno << "行，无法分析循环语句中的条件表达式类型 -whilestatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，无法分析循环语句中的条件表达式类型 -whilestatement\n", t->lineno);
        semanticError++;
    }

    if (conditionNode != nullptr) {
        SymbolTable* Entry = new SymbolTable;

        if (conditionNode->idnum != 0) {
            for (int i = 0; i < conditionNode->name.size(); i++) {
                bool found = FindEntry(stringToChar(conditionNode->name[i]), true, &Entry);
                if (!found) {
                    cout << "语义错误: 第" << t->lineno << "行，变量 '" << conditionNode->name[i] << "' 未声明 -whilestatement" << endl;
                    fprintf(outfile, "语义错误: 第%d行，变量 '%s' 未声明 -whilestatement\n",
                        t->lineno, conditionNode->name[i].c_str());
                    semanticError++;
                }
                else {
                    if (Entry->attrIR.idtype->kind != TypeKind::intTy && Entry->attrIR.idtype->kind != TypeKind::boolTy) {
                        cout << "语义错误: " << "第" << t->lineno << "行，循环语句中的条件表达式不是布尔类型 -whilestatement" << endl;
                        fprintf(outfile, "语义错误: 第%d行，循环语句中的条件表达式不是布尔类型 -whilestatement\n", t->lineno);
                        semanticError++;
                    }
                }
            }
        }
        else {
            if (conditionNode->nodekind != NodeKind::ExpK) {
                cout << "语义错误: " << "第" << t->lineno << "行，循环语句中的条件表达式不是布尔类型 -whilestatement" << endl;
                fprintf(outfile, "语义错误: 第%d行，循环语句中的条件表达式不是布尔类型 -whilestatement\n", t->lineno);
                semanticError++;
            }
        }

        //获取循环体语句序列节点
        TreeNode* bodyNode = t->child[1]; //循环体是第二个子节点
        if (bodyNode == nullptr) {
            cout << "语义错误: " << "第" << t->lineno << "行，循环语句中的循环体为空 -whilestatement" << endl;
            fprintf(outfile, "语义错误: 第%d行，循环语句中的循环体为空 -whilestatement\n", t->lineno);
            semanticError++;
        }
        else {
            //处理循环体中的语句序列
            while (bodyNode != nullptr) {
                statement(bodyNode); //调用 statement 函数处理语句序列
                bodyNode = bodyNode->sibling;
            }
        }
    }

    return;
}

//21.读语句分析处理函数
void readstatement(TreeNode* t) {
    if (t->name.empty()) {
        cout << "语义错误: 第" << t->lineno << "行，节点缺少名称" << endl;
        fprintf(outfile, "语义错误: 第%d行，节点缺少名称\n", t->lineno);
        semanticError++;
        return;
    }

    string varName = t->name[0];
    SymbolTable* entry = new SymbolTable;
    bool found = FindEntry(stringToChar(varName), true, &entry);//false为查当前符号表，true为查全部符号表

    if (!found) {
        cout << "语义错误: " << "第" << t->lineno << "行，读语句标识符 '" << varName << "' 未声明 -readstatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，读语句标识符 '%s' 未声明 -readstatement\n",
            t->lineno, varName.c_str());
        semanticError++;
    }
    else {
        // 检查标识符是否为变量
        if (entry->attrIR.kind != varKind) {
            cout << "语义错误: " << "第" << t->lineno << "行，'" << varName << "' 不是变量标识符 readstatement" << endl;
            fprintf(outfile, "语义错误: 第%d行，'%s' 不是变量标识符 readstatement\n",
                t->lineno, varName.c_str());
            semanticError++;
        }
    }
    return;
}

//22.返回语句分析处理函数
void returnstatement(TreeNode* t) {
    return;
}

//23.写语句分析处理函数
void writestatement(TreeNode* t) {
    //获取写语句中的表达式
    TreeNode* exprNode = t->child[0]; //表达式是写语句的第一个子节点
    if (exprNode == nullptr) {
        cout << "语义错误: " << "第" << t->lineno << "行，写语句中的表达式为空 -writestatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，写语句中的表达式为空 -writestatement\n", t->lineno);
        semanticError++;
    }

    //分析表达式的类型
    AccessKind* ekind = new AccessKind;
    TypeIR* exprType = Expr(exprNode, ekind); //调用表达式分析函数

    if (exprType == nullptr) {
        cout << "语义错误: 第" << t->lineno << "行，无法分析写语句中的表达式类型 -writestatement" << endl;
        fprintf(outfile, "语义错误: 第%d行，无法分析写语句中的表达式类型 -writestatement\n", t->lineno);
        semanticError++;
    }

    return;
}