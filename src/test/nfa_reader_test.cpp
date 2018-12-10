#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <tuple>
#include <string>
#include <memory>

enum NodeType {_none, _union, _concatenation, _star, _leafA, _leafB};

class Node {
  public:
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> center;
  std::unique_ptr<Node> right;
  bool empty;
  NodeType type;

  Node(){
    this->left = nullptr;
    this->center = nullptr;
    this->right = nullptr;
    this->empty = true;
    this->type = NodeType::_none;
  }

  explicit Node(NodeType type){
    this->left = nullptr;
    this->center = nullptr;
    this->right = nullptr;
    this->empty = false;
    this->type = type;
  }

  Node(std::unique_ptr<Node> _center, NodeType type){
    this->left = nullptr;
    this->center = std::move(_center);
    this->right = nullptr;
    this->empty = false;
    this->type = type;
  }

  Node(std::unique_ptr<Node> _left, std::unique_ptr<Node> _right, NodeType type){
    this->left = std::move(_left);
    this->center = nullptr;
    this->right = std::move(_right);
    this->empty = false;
    this->type = type;
  }
};

class ParseTree {
  public:
  std::unique_ptr<Node> root;

  std::unique_ptr<Node> evaluate(std::string regex){
    auto result = std::make_unique<Node>();
    for(int i = 0; i < regex.length(); i++){
      if(regex[i] == 'a'){
        auto branch = std::make_unique<Node>();
        if(i + 1 < regex.length()){
          if(regex[i + 1] == '*'){
            branch = std::make_unique<Node>(std::make_unique<Node>(NodeType::_leafA), NodeType::_star);
            i++;
          }
        }
        else {
          branch = std::make_unique<Node>(NodeType::_leafA);
        }

        if(result->empty){
          result = std::move(branch);
        }
        else {
          result = std::make_unique<Node>(std::move(result), std::move(branch), NodeType::_concatenation);
        }
      }
      else if(regex[i] == 'b'){
        auto branch = std::make_unique<Node>();
        if(i + 1 < regex.length()){
          if(regex[i + 1] == '*'){
            branch = std::make_unique<Node>(std::make_unique<Node>(NodeType::_leafB), NodeType::_star);
            i++;
          }
        }
        else {
          branch = std::make_unique<Node>(NodeType::_leafB);
        }

        if(result->empty){
          result = std::move(branch);
        }
        else {
          result = std::make_unique<Node>(std::move(result), std::move(branch), NodeType::_concatenation);
        }
      }
      else if(regex[i] == '|'){
        std::string remainder = regex.substr(i + 1, regex.length());
        result = std::make_unique<Node>(std::move(result), evaluate(remainder), NodeType::_union);
      }
      else if(regex[i] == '('){
        int depth = 1;
        std::string contents;
        int endpos;
        for(int j = i + 1; j < regex.length(); j++){
          if(regex[j] == '('){
            depth++;
          }
          if(regex[j] == ')'){
            depth--;
            if(depth == 0){
              contents = regex.substr(i + 1, (j - 1) - (i + 1));
              endpos = j;
              break;
            }
          }
        }
        auto branch = std::make_unique<Node>();
        if(endpos + 1 < regex.length()){
          if(regex[endpos + 1] == '*'){
            branch = std::make_unique<Node>(evaluate(contents), NodeType::_star);
            i = endpos + 1;
          }
        }
        else {
          branch = evaluate(contents);
          i = endpos;
        }

        if(result->empty){
          result = std::move(branch);
        }
        else {
          result = std::make_unique<Node>(std::move(result), std::move(branch), NodeType::_concatenation);
        }
      }
    }
    return result;
  }

  explicit ParseTree(std::string regex){
    root = evaluate(std::move(regex));
  }
};

class NFA {
  std::vector<std::map<char, int> > transitions;
  std::set<int> acceptStates;
};

std::tuple<std::string, int> printNode(const std::unique_ptr<Node>& root, int nodeLabel){
  std::string output;
  int nodeLabelFinal;
  switch(root->type){
    case NodeType::_none:
      std::cout << "Node has no type! Returning null." << std::endl;
      return std::make_tuple("", 0);
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
    case NodeType::_leafA:
      nodeLabelFinal = nodeLabel;
      output = "n" + std::to_string(nodeLabel) + " = a\n";
      break;
    case NodeType::_leafB:
      nodeLabelFinal = nodeLabel;
      output = "n" + std::to_string(nodeLabel) + " = b\n";
      break;
  }
  return std::make_tuple(output, nodeLabelFinal);
}

std::string printTree(ParseTree tree){
  return std::get<0>(printNode(tree.root, 1));
}

int main(int argc, char* argv[]){
  /*
  //Reads file into string
  std::ifstream fin("test1.txt");
  auto size = fin.tellg();
  std::string regex(size, '\0');
  fin.read(&regex[0], size);

  ParseTree tree(regex);
  */

  auto r1 = std::make_unique<Node>(NodeType::_leafA);
  auto r2 = std::make_unique<Node>(NodeType::_leafB);
  auto r3 = std::make_unique<Node>(std::move(r1), std::move(r2), NodeType::_union);
  auto r5 = std::make_unique<Node>(std::move(r3), NodeType::_star);
  auto r6 = std::make_unique<Node>(NodeType::_leafA);
  auto r7 = std::make_unique<Node>(std::move(r5), std::move(r6), NodeType::_concatenation);
  auto r8 = std::make_unique<Node>(NodeType::_leafB);
  auto r9 = std::make_unique<Node>(std::move(r7), std::move(r8), NodeType::_concatenation);
  auto r10 = std::make_unique<Node>(NodeType::_leafB);
  auto r11 = std::make_unique<Node>(std::move(r9), std::move(r10), NodeType::_concatenation);

  std::string output = std::get<0>(printNode(r11, 1));
  std::cout << output;
}
