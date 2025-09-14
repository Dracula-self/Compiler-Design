#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include<iostream>
#include<algorithm>
#include "total.h"
#include "syntaxAnalysis.h"
#include "LexicalAnalysis.h"
#include "printTree.h"
using namespace std;
// ��LexTypeö��ֵת��Ϊ��Ӧ���ַ�����ʾ
// ��LexTypeö��ֵת��Ϊ��Ӧ���ַ�����ʾ
const char* LexTypeToString(LexType type) {
    switch (type) {
    case ENDFILE:   return "EOF";
    case ERROR:     return "ERROR";
    case PROGRAM:   return "PROGRAM";
    case PROCEDURE: return "PROCEDURE";
    case TYPE:      return "TYPE";
    case VAR:       return "VAR";
    case IF:        return "IF";
    case THEN:      return "THEN";
    case ELSE:      return "ELSE";
    case FI:        return "FI";
    case WHILE:     return "WHILE";
    case DO:        return "DO";
    case ENDWH:     return "ENDWH";
    case BEGIN:     return "BEGIN";
    case END:       return "END";
    case READ:      return "READ";
    case WRITE:     return "WRITE";
    case ARRAY:     return "ARRAY";
    case OF:        return "OF";
    case RECORD:    return "RECORD";
    case RETURN:    return "RETURN";
    case INTEGER:   return "INTEGER";
    case CHAR:      return "CHAR";
    case ID:        return "ID";
    case INTC:      return "INTC";
    case CHARC:     return "CHARC";
    case ASSIGN:    return ":=";
    case EQ:        return "=";
    case LT:        return "<";
    case GT:        return ">";
    case PLUS:      return "+";
    case MINUS:     return "-";
    case TIMES:     return "*";
    case OVER:      return "/";
    case LPAREN:    return "(";
    case RPAREN:    return ")";
    case DOT:       return ".";
    case COLON:     return ":";
    case SEMI:      return ";";
    case COMMA:     return ",";
    case LMIDPAREN: return "[";
    case RMIDPAREN: return "]";
    case UNDERANGE: return "..";  // �����±귶Χ����

    default:        return "δ֪����";
    }
}
// ��ȡ�ڵ����͵��ַ�����ʾ
const char* getNodeKindStr(NodeKind kind) {
    switch (kind) {
    case ProK: return "Program";
    case PheadK: return "ProgramHead";
    case TypeK: return "TypeDecl";
    case VarK: return "VarDecl";
    case ProcDecK: return "ProcDecl";
    case StmLK: return "StmtList";
    case DecK: return "Declaration";
    case StmtK: return "Statement";
    case ExpK: return "Expression";
    case nodeNull: return "Null";
    default: return "Unknown";
    }
}

// ��ȡ�������͵��ַ�����ʾ
const char* getDecKindStr(DecKind kind) {
    switch (kind) {
    case ArrayK: return "Array";
    case CharK: return "Char";
    case IntegerK: return "Integer";
    case RecordK: return "Record";
    case IdK: return "Id";
    case decNull: return "Null";
    default: return "Unknown";
    }
}

// ��ȡ������͵��ַ�����ʾ
const char* getStmtKindStr(StmtKind kind) {
    switch (kind) {
    case IfK: return "If";
    case WhileK: return "While";
    case AssignK: return "Assign";
    case ReadK: return "Read";
    case WriteK: return "Write";
    case CallK: return "Call";
    case ReturnK: return "Return";
    case stmtNull: return "Null";
    default: return "Unknown";
    }
}

// ��ȡ���ʽ���͵��ַ�����ʾ
const char* getExpKindStr(ExpKind kind) {
    switch (kind) {
    case OpK: return "Op";
    case ConstK: return "Const";
    case VariK: return "Variable";
    case expKindNull: return "Null";
    default: return "Unknown";
    }
}

// ��ȡ�������͵��ַ�����ʾ
const char* getVarKindStr(VarKind kind) {
    switch (kind) {
    case IdV: return "Id";
    case ArrayMembV: return "ArrayMember";
    case FieldMembV: return "FieldMember";
    case varNull: return "Null";
    default: return "Unknown";
    }
}

// ��ȡ���ʽ���͵��ַ�����ʾ
const char* getExpTypeStr(ExpType type) {
    switch (type) {
    case Void: return "Void";
    case Integer: return "Integer";
    case Boolean: return "Boolean";
    case expTypeNull: return "Null";
    default: return "Unknown";
    }
}

// ��ȡ�������͵��ַ�����ʾ
const char* getParamTypeStr(ParamType type) {
    switch (type) {
    case none: return "None";
    case varparamType: return "VarParam";
    case valparamType: return "ValParam";
    case paramNull: return "Null";
    default: return "Unknown";
    }
}

// ��ȡ�ʷ����͵��ַ�����ʾ
const char* getLexTypeStr(LexType type) {
    switch (type) {
    case ENDFILE:
        return "ENDFILE";
    case ERROR:
        return "ERROR";
    case PROGRAM:
        return "PROGRAM";
    case PROCEDURE:
        return "PROCEDURE";
    case TYPE:
        return "TYPE";
    case VAR:
        return "VAR";
    case IF:
        return "IF";
    case THEN:
        return "THEN";
    case ELSE:
        return "ELSE";
    case FI:
        return "FI";
    case WHILE:
        return "WHILE";
    case DO:
        return "DO";
    case ENDWH:
        return "ENDWH";
    case BEGIN:
        return "BEGIN";
    case END:
        return "END";
    case READ:
        return "READ";
    case WRITE:
        return "WRITE";
    case ARRAY:
        return "ARRAY";
    case OF:
        return "OF";
    case RECORD:
        return "RECORD";
    case RETURN:
        return "RETURN";
    case INTEGER:
        return "INTEGER";
    case CHAR:
        return "CHAR";
    case ID:
        return "ID";
    case INTC:
        return "INTC";
    case CHARC:
        return "CHARC";
    case ASSIGN:
        return "ASSIGN";
    case EQ:
        return "EQ";
    case LT:
        return "LT";
    case GT:
        return "GT";
    case PLUS:
        return "PLUS";
    case MINUS:
        return "MINUS";
    case TIMES:
        return "TIMES";
    case OVER:
        return "OVER";
    case DOT:
        return "DOT";
    case LPAREN:
        return "LPAREN";
    case RPAREN:
        return "RPAREN";
    case COLON:
        return "COLON";
    case SEMI:
        return "SEMI";
    case COMMA:
        return "COMMA";
    case LMIDPAREN:
        return "LMIDPAREN";
    case RMIDPAREN:
        return "RMIDPAREN";
    case UNDERANGE:
        return "UNDERANGE";
    default:
        return "UNKNOWN";
    }
}

// ��ӡ�������ļ�
void printIndentToFile(FILE* treetree, int level) {
    for (int i = 0; i < level; i++) {
        fprintf(treetree, "  ");
    }
}

// ��ӡ�﷨�����ļ�
void printsimpleTree(FILE* treetree, TreeNode* tree, int level) {
    if (tree == NULL) return;
    printIndentToFile(treetree, level);
    // ���ȴ�ӡ�к���Ϣ
    fprintf(treetree, "[Line: %d] ", tree->lineno);
    // ���ݽڵ����ʹ�ӡ��ͬ����Ϣ
    switch (tree->nodekind) {
    case ProK:
        fprintf(treetree, "ProK\n");
        break;
    case PheadK:
        fprintf(treetree, "PheadK  %s\n", tree->name[0].c_str());
        break;

    case TypeK:
        fprintf(treetree, "TypeK\n");
        break;

    case VarK:
        fprintf(treetree, "VarK\n");
        break;

    case ProcDecK:
        if (tree->idnum > 0)
            fprintf(treetree, "ProcDecK  %s\n", tree->name[0].c_str());
        else
            fprintf(treetree, "ProcDecK\n");
        break;

    case StmLK:
        fprintf(treetree, "StmLK\n");
        break;

    case DecK:
        fprintf(treetree, "DecK  ");

        // ��ӡ��������
        switch (tree->kind.dec) {
        case IntegerK:
            fprintf(treetree, "IntegerK");
            break;
        case CharK:
            fprintf(treetree, "CharK");
            break;
        case ArrayK:
            fprintf(treetree, "ArrayK  %d  %d  ",
                tree->attr.ArrayAttr.low,   // �����½�
                tree->attr.ArrayAttr.up);   // �����Ͻ�

            // ��ӡ����Ԫ������
            switch (tree->attr.ArrayAttr.childtype) {
            case IntegerK:
                fprintf(treetree, "IntegerK");
                break;
            case CharK:
                fprintf(treetree, "CharK");
                break;
            default:
                break;
            }
            break;
        case RecordK:
            fprintf(treetree, "RecordK");
            break;
        case IdK:
            fprintf(treetree, "IdK");
            break;
        default:
            break;
        }

        // ��ӡ������
        for (int i = 0; i < tree->idnum; i++) {
            fprintf(treetree, "  %s", tree->name[i].c_str());
        }

        // ��ӡ��������
        if (tree->attr.ProcAttr.paramt != none) {
            fprintf(treetree, "  param: ");
            if (tree->attr.ProcAttr.paramt == varparamType)
                fprintf(treetree, "var");
            else if (tree->attr.ProcAttr.paramt == valparamType)
                fprintf(treetree, "val");
        }
        fprintf(treetree, "\n");
        break;

    case StmtK:
        fprintf(treetree, "StmtK  ");

        // ��ӡ�������
        switch (tree->kind.stmt) {
        case IfK:
            fprintf(treetree, "If\n");
            break;
        case WhileK:
            fprintf(treetree, "While\n");
            break;
        case AssignK:
            fprintf(treetree, "Assign\n");
            break;
        case ReadK:
            fprintf(treetree, "Read");
            if (tree->idnum > 0)
                fprintf(treetree, "  %s", tree->name[0].c_str());
            fprintf(treetree, "\n");
            break;
        case WriteK:
            fprintf(treetree, "Write\n");
            break;
        case CallK:
            fprintf(treetree, "Call    ");
            fprintf(treetree, "%s", tree->name[0].c_str());
            fprintf(treetree, "\n");
            break;
        case ReturnK:
            fprintf(treetree, "Return\n");
            break;
        default:
            fprintf(treetree, "\n");
            break;
        }
        break;

    case ExpK:
        fprintf(treetree, "ExpK  ");

        // ��ӡ���ʽ����
        switch (tree->kind.exp) {
        case OpK:
            fprintf(treetree, "Op  ");
            // ��ӡ������
            if (tree->attr.ExpAttr.op == LT)
                fprintf(treetree, "<");
            else if (tree->attr.ExpAttr.op == EQ)
                fprintf(treetree, "=");
            else if (tree->attr.ExpAttr.op == PLUS)
                fprintf(treetree, "+");
            else if (tree->attr.ExpAttr.op == MINUS)
                fprintf(treetree, "-");
            else if (tree->attr.ExpAttr.op == TIMES)
                fprintf(treetree, "*");
            else if (tree->attr.ExpAttr.op == OVER)
                fprintf(treetree, "/");
            fprintf(treetree, "\n");
            break;

        case ConstK:
            fprintf(treetree, "Const  %d\n", tree->attr.ExpAttr.val);
            break;

        case VariK:
            fprintf(treetree, "%s", tree->name[0].c_str());
            fprintf(treetree, "  ");

            // ��ӡ��������
            if (tree->attr.ExpAttr.varkind == IdV)
                fprintf(treetree, "IdV");
            else if (tree->attr.ExpAttr.varkind == ArrayMembV)
                fprintf(treetree, "ArrayMembV");
            else if (tree->attr.ExpAttr.varkind == FieldMembV)
                fprintf(treetree, "FieldMembV");

            fprintf(treetree, "\n");
            break;

        default:
            fprintf(treetree, "\n");
            break;
        }
        break;
    default:
        fprintf(treetree, "Unknown node kind\n");
        break;
    }
    // ��ӡ�ӽڵ�
    for (int i = 0; i < MAXCHILDREN; i++) {
        if (tree->child[i] != NULL) {
            printsimpleTree(treetree, tree->child[i], level + 1);
        }
    }
    // ��ӡ�ֵܽڵ�
    if (tree->sibling != NULL) {
        printsimpleTree(treetree, tree->sibling, level);
    }
}
void printTreeToFile(TreeNode* tree, const char* filename) {
    FILE* treetree = fopen(filename, "w");
    if (treetree == NULL) {
        printf("Error opening file %s for writing\n", filename);
        return;
    }
    printsimpleTree(treetree, tree, 0);
    fclose(treetree);
    printf("Syntax tree has been written to %s\n", filename);
}



// ��ӡ���ĺ���
void print_tree(TreeNode* t, int n, FILE* treefile) {
    while (t != NULL) {
        // �������
        for (int i = 0; i < n; ++i) {
            fprintf(treefile, "   ");
        }

        // ���ݽڵ����������Ϣ
        switch (t->nodekind) {
        case ProK:
            fprintf(treefile, "ProK %d\n", t->lineno);
            break;
        case PheadK:
            if (t->idnum > 0) {
                fprintf(treefile, "PheadK %d %s\n", t->lineno, t->name[0].c_str());
            }
            else {
                fprintf(treefile, "PheadK %d \n", t->lineno);
            }
            break;
        case TypeK:
            fprintf(treefile, "TypeK %d\n", t->lineno);
            break;
        case VarK:
            fprintf(treefile, "VarK %d\n", t->lineno);
            break;
        case ProcDecK:
            if (t->idnum > 0) {
                fprintf(treefile, "ProcDecK %d %s\n", t->lineno, t->name[0].c_str());
            }
            else {
                fprintf(treefile, "ProcDecK %d \n", t->lineno);
            }
            break;
        case StmLK:
            fprintf(treefile, "StmLK %d\n", t->lineno);
            break;
        case DecK:
            fprintf(treefile, "DecK %d ", t->lineno);
            if (t->attr.ProcAttr.paramt == varparamType) {
                fprintf(treefile, "varparamType ");
            }
            else if (t->attr.ProcAttr.paramt == valparamType) {
                fprintf(treefile, "valparamType ");
            }
            switch (t->kind.dec) {
            case ArrayK:
                fprintf(treefile, "ArrayK %d %d ", t->attr.ArrayAttr.low, t->attr.ArrayAttr.up);
                switch (t->attr.ArrayAttr.childtype) {
                case ArrayK: fprintf(treefile, "ArrayK "); break;
                case CharK: fprintf(treefile, "CharK "); break;
                case IntegerK: fprintf(treefile, "IntegerK "); break;
                case RecordK: fprintf(treefile, "RecordK "); break;
                case IdK: fprintf(treefile, "IdK "); break;
                }
                for (size_t i = 0; i < t->idnum; ++i) {
                    fprintf(treefile, "%s ", t->name[i].c_str());
                }
                fprintf(treefile, "\n");
                break;
            case CharK:
                fprintf(treefile, "CharK ");
                for (size_t i = 0; i < t->idnum; ++i) {
                    fprintf(treefile, "%s ", t->name[i].c_str());
                }
                fprintf(treefile, "\n");
                break;
            case IntegerK:
                fprintf(treefile, "IntegerK ");
                for (size_t i = 0; i < t->idnum; ++i) {
                    fprintf(treefile, "%s ", t->name[i].c_str());
                }
                fprintf(treefile, "\n");
                break;
            case RecordK:
                fprintf(treefile, "RecordK ");
                for (size_t i = 0; i < t->idnum; ++i) {
                    fprintf(treefile, "%s ", t->name[i].c_str());
                }
                fprintf(treefile, "\n");
                break;
            case IdK:
                fprintf(treefile, "IdK %s ", t->type_name.c_str());
                for (size_t i = 0; i < t->idnum; ++i) {
                    fprintf(treefile, "%s ", t->name[i].c_str());
                }
                fprintf(treefile, "\n");
                break;
            default:
                fprintf(treefile, "�﷨���󣺵�%d�У�δ֪����������\n", t->lineno);
            }
            break;
        case StmtK:
            fprintf(treefile, "StmtK %d ", t->lineno);
            switch (t->kind.stmt) {
            case IfK:
                fprintf(treefile, "IfK \n");
                break;
            case WhileK:
                fprintf(treefile, "WhileK \n");
                break;
            case AssignK:
                fprintf(treefile, "AssignK \n");
                break;
            case ReadK:
                if (t->idnum > 0) {
                    fprintf(treefile, "ReadK %s\n", t->name[0].c_str());
                }
                else {
                    fprintf(treefile, "ReadK \n");
                }
                break;
            case WriteK:
                fprintf(treefile, "WriteK \n");
                break;
            case CallK:
                if (t->idnum > 0) {
                    fprintf(treefile, "CallK %s\n", t->name[0].c_str());
                }
                else {
                    fprintf(treefile, "CallK \n");
                }
                break;
            case ReturnK:
                fprintf(treefile, "ReturnK \n");
                break;
            default:
                fprintf(treefile, "�﷨���󣺵�%d�У�δ֪���������\n", t->lineno);
            }
            break;
        case ExpK:
            fprintf(treefile, "ExpK %d ", t->lineno);
            switch (t->kind.exp) {
            case OpK:
                fprintf(treefile, "OpK %s\n", LexTypeToString(t->attr.ExpAttr.op));
                break;
            case ConstK:
                if (t->attr.ExpAttr.ischar == false) {
                    fprintf(treefile, "ConstK %d\n", t->attr.ExpAttr.val);
                }
                else {
                    fprintf(treefile, "ConstK %c\n", t->attr.ExpAttr.val);
                }
                break;
            case VariK:
                switch (t->attr.ExpAttr.varkind) {
                case IdV: fprintf(treefile, "IdK IdV "); break;
                case ArrayMembV: fprintf(treefile, "IdK ArrayMembV "); break;
                case FieldMembV: fprintf(treefile, "IdK FieldMembV "); break;
                }
                if (t->idnum > 0) {
                    fprintf(treefile, "%s\n", t->name[0].c_str());
                }
                else {
                    fprintf(treefile, "\n");
                }
                break;
            default:
                fprintf(treefile, "�﷨���󣺵�%d�У�δ֪�ı��ʽ����\n", t->lineno);
            }
            break;
        default:
            fprintf(treefile, "�﷨���󣺵�%d�У�δ֪�Ľڵ�����\n", t->lineno);
        }

        // �ݹ��ӡ�ӽڵ�
        for (int i = 0; i<3; ++i) {
            if (t->child[i] != NULL) {
                print_tree(t->child[i], n + 1, treefile);
            }

        }

        // �����ֵܽڵ�
        t = t->sibling;
    }
}

