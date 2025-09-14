#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "total.h"
#include "vs.h"
using namespace std;

//将节点类型转换为字符串的辅助函数
std::string nodeKindToString(NodeKind kind) {
    switch (kind) {
    case ProK: return "Program";
    case PheadK: return "ProgramHead";
    case TypeK: return "TypeDec";
    case VarK: return "VarDec";
    case ProcDecK: return "ProcDec";
    case StmLK: return "StmList";
    case DecK: return "Dec";
    case StmtK: return "Stmt";
    case ExpK: return "Exp";
    default: return "Unknown";
    }
}

static int nodeId = 0;

//递归生成节点和边的普通函数
int generateNodeFunction(TreeNode* node, std::ofstream& dotFile) {
    if (!node) return -1;
    int currentId = nodeId++;
    //创建节点标签
    std::stringstream label;
    label << nodeKindToString(node->nodekind);
    // 添加标识符名称(如果有)
    if (node->idnum > 0 && !node->name.empty()) {
        label << "\\n";
        for (int i = 0; i < node->idnum && i < node->name.size(); i++) {
            label << node->name[i];
            if (i < node->idnum - 1) label << ", ";
        }
    }
    if (!node->type_name.empty()) {
        label << "\\nType: " << node->type_name;
    }
    label << "\\nLine: " << node->lineno;
    //添加节点到DOT文件
    dotFile << "  node" << currentId << " [label=\"" << label.str() << "\"];\n";
    //递归处理子节点
    for (int i = 0; i < MAXCHILDREN; i++) {
        if (node->child[i]) {
            int childId = generateNodeFunction(node->child[i], dotFile);
            if (childId != -1) {
                dotFile << "  node" << currentId << " -> node" << childId;
                dotFile << " [label=\"child" << i << "\"];\n";
            }
        }
    }
    //处理兄弟节点
    if (node->sibling) {
        int siblingId = generateNodeFunction(node->sibling, dotFile);
        if (siblingId != -1) {
            dotFile << "  node" << currentId << " -> node" << siblingId;
            dotFile << " [label=\"sibling\", style=dashed, color=red];\n";
        }
    }
    return currentId;
}
void generateDOT(TreeNode* root, const std::string& filename) {
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    dotFile << "digraph AST {\n";
    dotFile << "  node [shape=box, style=\"filled,solid\", fillcolor=white, color=black];\n";
    nodeId = 0;
    // 从根节点开始生成DOT描述
    generateNodeFunction(root, dotFile);
    dotFile << "}\n";
    dotFile.close();
    std::cout << "DOT file generated: " << filename << std::endl;
    std::string pngFilename = filename.substr(0, filename.find_last_of('.')) + ".png";
    std::string command = "dot -Tpng " + filename + " -o " + pngFilename;
    // 执行命令并检查结果
    int result = system(command.c_str());
    if (result == 0) {
        std::cout << "Image generated: " << pngFilename << std::endl;
        std::string openCommand = "start " + pngFilename;
        system(openCommand.c_str());
    }
    else {
        std::cerr << "Error generating image. Make sure Graphviz is installed and in your PATH." << std::endl;
    }
}