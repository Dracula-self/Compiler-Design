#pragma once
#include "total.h"

void printIndent(int level);
const char* getNodeKindStr(NodeKind kind);
const char* getDecKindStr(DecKind kind);
const char* getStmtKindStr(StmtKind kind);
const char* getExpKindStr(ExpKind kind);
const char* getVarKindStr(VarKind kind);
const char* getExpTypeStr(ExpType type);
const char* getParamTypeStr(ParamType type);
const char* getLexTypeStr(LexType type);

void printTreeToFile(TreeNode* tree, const char* filename);
void print_tree(TreeNode* tree, int level, FILE* p);
