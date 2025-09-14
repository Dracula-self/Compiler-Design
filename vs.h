#pragma once


std::string nodeKindToString(NodeKind kind);


int generateNodeFunction(TreeNode* node, std::ofstream& dotFile);

void generateDOT(TreeNode* root, const std::string& filename);


