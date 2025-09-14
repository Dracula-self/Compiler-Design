#pragma once
#include "total.h"

#include"LexicalAnalysis.h"

using namespace std;
static string type;//类型
static string token;//具体标识
static bool has_unread = false;
static bool if_c = false;
static string temp_name;

//GYF----------------------------------------
int MatchToken(LexType p);
struct TreeNode* parse(TokenNode* head);
struct TreeNode* program(void);
TreeNode* ProgramHead();
TreeNode* DeclarePart();
TreeNode* TypeDeclaration(void);
TreeNode* TypeDec(void);
TreeNode* TypeDecList(void);
TreeNode* TypeDecMore(void);
void TypeId(TreeNode* t);
void TypeDef(TreeNode* t);
//MR-----------------------------------
void BaseType(TreeNode* t);
void StructureType(TreeNode* t);
void ArrayType(TreeNode* t);
void RecType(TreeNode* t);
TreeNode* FieldDecList();
TreeNode* FieldDecMore();
void IdList(TreeNode* t);
void IdMore(TreeNode* t);
TreeNode* VarDec();
TreeNode* VarDeclaration();
TreeNode* VarDecList();
TreeNode* VarDecMore();
void VarIdList(TreeNode* t);
void VarIdMore(TreeNode* t);
TreeNode* ProcDec();
TreeNode* ProcDeclaration();
void ParamList(TreeNode* t);
TreeNode* ParamDecList();
TreeNode* ParamMore();
TreeNode* Param();
void FormList(TreeNode* t);
void FidMore(TreeNode* t);
//XYN-------------------------------

TreeNode* ProcDecPart(void);
TreeNode* procBody(void);
TreeNode* ProgramBody(void);
TreeNode* StmList(void);
TreeNode* StmMore(void);
TreeNode* Stm(void);
TreeNode* AssCall(void);
TreeNode* AssignmentRest(void);
TreeNode* ConditionalStm(void);
TreeNode* LoopStm(void);
TreeNode* InputStm(void);
TreeNode* OutputStm(void);
TreeNode* ReturnStm(void);
TreeNode* CallStmRest(void);
TreeNode* ActParamList(void);
TreeNode* ActParamMore(void);
TreeNode* Exp(void);
TreeNode* Simple_exp(void);
TreeNode* term(void);
TreeNode* factor(void);
TreeNode* variable(void);
void variMore(TreeNode* t);
TreeNode* fieldvar(void);
void fieldvarMore(TreeNode* t);
