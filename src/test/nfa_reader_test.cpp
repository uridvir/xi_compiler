#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <optional>
#include <tuple>
#include <string>

enum NodeType {_union, _concatenation, _star, _leafA, _leafB};

class Node {
  public:
  bool empty;
  NodeType type;

  Node(){
    this->empty = true;
  }
};

class UnionNode : public Node {
  public:
  Node* left;
  Node* right;

  UnionNode(Node* left, Node* right){
    this->left = left;
    this->right = right;
    this->empty = false;
    this->type = NodeType::_union;
  }
};

class ConcatenationNode : public Node {
  public:
  Node* left;
  Node* right;

  ConcatenationNode(Node* left, Node* right){
    this->left = left;
    this->right = right;
    this->empty = false;
    this->type = NodeType::_concatenation;
  }
};

class StarNode : public Node {
  public:
  Node* child;

  explicit StarNode(Node* child){
    this->child = child;
    this->empty = false;
    this->type = NodeType::_star;
  }
};

class LeafA : public Node {
  public:
  LeafA(){
    this->empty = false;
    this->type = NodeType::_leafA;
  }
};

class LeafB : public Node {
  public:
  LeafB(){
    this->empty = false;
    this->type = NodeType::_leafB;
  }
};

class ParseTree {
  public:
  Node* root;

  Node* evaluate(std::string regex){
    Node* result;
    {
      Node node;
      result = &node;
    }
    for(int i = 0; i < regex.length(); i++){
      if(regex[i] == 'a'){
        Node* branch;
        {
          Node node;
          branch = &node;
        }
        if(i + 1 < regex.length()){
          if(regex[i + 1] == '*'){
            Node leaf = LeafA();
            StarNode star = StarNode(&leaf);
            branch = &star;
            i++;
          }
        }
        else {
          *branch = LeafA();
        }

        if(result->empty){
          result = branch;
        }
        else {
          ConcatenationNode concatenation = ConcatenationNode(result, branch);
          result = &concatenation;
        }
      }
      else if(regex[i] == 'b'){
        Node* branch;
        {
          Node node;
          branch = &node;
        }
        if(i + 1 < regex.length()){
          if(regex[i + 1] == '*'){
            Node leaf = LeafB();
            StarNode star = StarNode(&leaf);
            branch = &star;
            i++;
          }
        }
        else {
          *branch = LeafB();
        }

        if(result->empty){
          result = branch;
        }
        else {
          ConcatenationNode concatenation = ConcatenationNode(result, branch);
          result = &concatenation;
        }
      }
      else if(regex[i] == '|'){
        std::string remainder = regex.substr(i + 1, regex.length());
        UnionNode _union = UnionNode(result, evaluate(remainder));
        result = &_union;
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
        Node* branch;
        {
          Node node;
          branch = &node;
        }
        if(endpos + 1 < regex.length()){
          if(regex[endpos + 1] == '*'){
            StarNode star = StarNode(evaluate(contents));
            branch = &star;
            i = endpos + 1;
          }
        }
        else {
          branch = evaluate(contents);
          i = endpos;
        }

        if(result->empty){
          result = branch;
        }
        else {
          ConcatenationNode concatenation = ConcatenationNode(result, branch);
          result = &concatenation;
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

std::tuple<std::string, int> printNode(Node* root, int nodeLabel){
  std::string output;
  int nodeLabelFinal;
  switch(root->type){
    case NodeType::_union:
      {
        auto _root = static_cast<UnionNode*>(root);

        int nodeLabelLeft = nodeLabel + 1;
        auto leftPrinted = printNode(_root->left, nodeLabelLeft);
        std::string leftOutput = std::get<0>(leftPrinted);
        int nodeLabelRight = std::get<1>(leftPrinted) + 1;

        auto rightPrinted = printNode(_root->right, nodeLabelRight);
        std::string rightOutput = std::get<0>(rightPrinted);
        nodeLabelFinal = std::get<1>(rightPrinted);

        output = "n" + std::to_string(nodeLabel) + " = n" + std::to_string(nodeLabelLeft) + " | n"
          + std::to_string(nodeLabelRight) + "\n" + leftOutput + rightOutput;
        break;
      }
    case NodeType::_concatenation:
      {
        auto _root = static_cast<ConcatenationNode*>(root);

        int nodeLabelLeft = nodeLabel + 1;
        auto leftPrinted = printNode(_root->left, nodeLabelLeft);
        std::string leftOutput = std::get<0>(leftPrinted);
        int nodeLabelRight = std::get<1>(leftPrinted) + 1;

        auto rightPrinted = printNode(_root->right, nodeLabelRight);
        std::string rightOutput = std::get<0>(rightPrinted);
        nodeLabelFinal = std::get<1>(rightPrinted);

        output = "n" + std::to_string(nodeLabel) + " = n" + std::to_string(nodeLabelLeft) + " n"
          + std::to_string(nodeLabelRight) + "\n" + leftOutput + rightOutput;
        break;
      }
    case NodeType::_star:
      {
        auto _root = static_cast<StarNode*>(root);

        int nodeLabelChild = nodeLabel + 1;
        auto childPrinted = printNode(_root->child, nodeLabelChild);
        std::string childOutput = std::get<0>(childPrinted);
        nodeLabelFinal = std::get<1>(childPrinted);

        output = "n" + std::to_string(nodeLabel) + " = n" + std::to_string(nodeLabelChild) + " *\n" + childOutput;
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

  LeafA* r1; 
  {
    LeafA leaf;
    r1 = &leaf;
  }

  LeafB* r2;
  {
    LeafB leaf;
    r2 = &leaf;
  }
  
  UnionNode* r3;
  {
    UnionNode _union = UnionNode(r1, r2);
    r3 = &_union;
  }

  StarNode* r5;
  {
    StarNode star = StarNode(r3);
    r5 = &star;
  }

  LeafA* r6;
  {
    LeafA leaf;
    r6 = &leaf;
  }

  ConcatenationNode* r7;
  {
    ConcatenationNode concatenation = ConcatenationNode(r5, r6);
    r7 = &concatenation;
  }

  LeafB* r8;
  {
    LeafB leaf;
    r8 = &leaf;
  }

  ConcatenationNode* r9;
  {
    ConcatenationNode concatenation = ConcatenationNode(r7, r8);
    r9 = &concatenation;
  }

  LeafB* r10;
  {
    LeafB leaf;
    r10 = &leaf;
  }

  ConcatenationNode* r11;
  {
    ConcatenationNode concatenation = ConcatenationNode(r9, r10);
    r11 = &concatenation;
  }

  std::string output = std::get<0>(printNode(r11, 1));
  std::cout << output;
}
