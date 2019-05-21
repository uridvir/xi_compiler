#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H

#include <memory>
#include <string>

enum NodeType {_empty, _union, _concatenation, _star, _leaf, _lookahead};

class Node {
  public:
  
    //Child nodes
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> center;
    std::unique_ptr<Node> right;
    
    NodeType type;
    char leafType;

    Node(){
      this->left = nullptr;
      this->center = nullptr;
      this->right = nullptr;
      this->type = NodeType::_empty;
      this->leafType = 0;
    }

    explicit Node(NodeType type){
      this->left = nullptr;
      this->center = nullptr;
      this->right = nullptr;
      this->type = type;
      this->leafType = 0;
    }

    explicit Node(char leafType){
      this->left = nullptr;
      this->center = nullptr;
      this->right = nullptr;
      this->type = NodeType::_leaf;
      this->leafType = leafType;
    }

    Node(std::unique_ptr<Node> _center, NodeType type){
      this->left = nullptr;
      this->center = std::move(_center);
      this->right = nullptr;
      this->type = type;
      this->leafType = 0;
    }

    Node(std::unique_ptr<Node> _left, std::unique_ptr<Node> _right, NodeType type){
      this->left = std::move(_left);
      this->center = nullptr;
      this->right = std::move(_right);
      this->type = type;
      this->leafType = 0;
    }
};

class SyntaxTree {
  public:
    Node* root;
    std::unique_ptr<Node> evaluate(std::string regex);
    explicit SyntaxTree(std::string regex){
      //evaluate returns a unique_ptr but root is a normal pointer
      this->root = evaluate(std::move(regex)).release();
    }
};

#endif
