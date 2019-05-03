#include "SyntaxTree.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <tuple>

namespace SyntaxTreeTest {

std::tuple<std::string, int> printNode(const std::unique_ptr<Node>& root, int nodeLabel){
  std::string output;
  int nodeLabelFinal;
  switch(root->type){
    case NodeType::_empty:
      return std::make_tuple("n" + std::to_string(nodeLabel) + " = empty\n", nodeLabel);
    case NodeType::_union:
      {
        int nodeLabelLeft = nodeLabel + 1;
        auto leftPrinted = printNode(root->left, nodeLabelLeft);
        std::string leftOutput = std::get<0>(leftPrinted);
        int nodeLabelRight = std::get<1>(leftPrinted) + 1;

        auto rightPrinted = printNode(root->right, nodeLabelRight);
        std::string rightOutput = std::get<0>(rightPrinted);
        nodeLabelFinal = std::get<1>(rightPrinted);

        output = "n" + std::to_string(nodeLabel) + " = n" + std::to_string(nodeLabelLeft) + " | n"
          + std::to_string(nodeLabelRight) + "\n" + leftOutput + rightOutput;
        break;
      }
    case NodeType::_concatenation:
      {
        int nodeLabelLeft = nodeLabel + 1;
        auto leftPrinted = printNode(root->left, nodeLabelLeft);
        std::string leftOutput = std::get<0>(leftPrinted);
        int nodeLabelRight = std::get<1>(leftPrinted) + 1;

        auto rightPrinted = printNode(root->right, nodeLabelRight);
        std::string rightOutput = std::get<0>(rightPrinted);
        nodeLabelFinal = std::get<1>(rightPrinted);

        output = "n" + std::to_string(nodeLabel) + " = n" + std::to_string(nodeLabelLeft) + " n"
          + std::to_string(nodeLabelRight) + "\n" + leftOutput + rightOutput;
        break;
      }
    case NodeType::_star:
      {
        int nodeLabelCenter = nodeLabel + 1;
        auto centerPrinted = printNode(root->center, nodeLabelCenter);
        std::string centerOutput = std::get<0>(centerPrinted);
        nodeLabelFinal = std::get<1>(centerPrinted);

        output = "n" + std::to_string(nodeLabel) + " = n" + std::to_string(nodeLabelCenter) + " *\n" + centerOutput;
        break;
      }
    case NodeType::_leaf:
      nodeLabelFinal = nodeLabel;
      output = "n" + std::to_string(nodeLabel) + " = " + root->leafType + "\n";
      break;
    case NodeType::_lookahead:
      nodeLabelFinal = nodeLabel;
      output = "n" + std::to_string(nodeLabel) + " = lookahead\n";
      break;
  }
  return std::make_tuple(output, nodeLabelFinal);
}

std::string printTree(SyntaxTree tree){
  return std::get<0>(printNode(std::unique_ptr<Node>(tree.root), 1));
}

void test(const std::string& regex){
  std::cout << regex << "\n\n";
  SyntaxTree tree(regex);
  std::cout << printTree(tree) << "------------------------------\n\n";
}

} //end of namespace SyntaxTreeTest

int main(){
  std::cout << "SyntaxTree:" << std::endl << std::endl;
  SyntaxTreeTest::test("(a|b)*abb");
  SyntaxTreeTest::test(R"((\(|\|)*\(\|\|)");
  SyntaxTreeTest::test(R"((a|b)\*)");
  SyntaxTreeTest::test("(a|b)*abb/a");
  SyntaxTreeTest::test("(a)");
  SyntaxTreeTest::test("(pop|blam)goesthe(weasel|squirrel)");
  SyntaxTreeTest::test(R"((a|\0)b)");
}
