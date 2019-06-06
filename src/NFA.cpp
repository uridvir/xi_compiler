#include "NFA.h"

//Takes list of regexes, constructs them, and then creates e-transitions from state 0 to all the resulting mini-NFAs
NFA::NFA(std::vector<std::tuple<std::string, std::string> > tokenRegexList){
  table.resize(1);
  acceptStates = {};
  lookaheadStates = {};
  tokens = {};
  int index = 1;
  for (auto pair : tokenRegexList){
    table[0]['\0'].emplace(index);
    SyntaxTree tree(std::get<0>(pair));
    auto branch = construct(tree.root, index);
    int acceptState = std::get<0>(branch);
    if(std::get<1>(branch).has_value()){
      lookaheadMap.emplace(acceptState, std::get<1>(branch).value());
      lookaheadStates.emplace(std::get<1>(branch).value());
    }
    acceptStates.emplace(acceptState);
    tokens.emplace(acceptState, std::get<1>(pair));
    index = acceptState + 1;
  }
}

/*
 * construct implements the McNaughton-Yamada-Thompson algorithm to convert root into an NFA. root is a pointer to a
 * Node object which represents the syntax tree for a regular expression. construct assigns the start state of the NFA
 * to the value of index, and returns a tuple with the accept state of the NFA and the location of the lookahead
 * operator, if it found one.
 */
std::tuple<int, std::optional<int> > NFA::construct(const Node* root, int index){
  auto result = std::make_tuple(-1, std::optional<int>());
  switch(root->type){
    case NodeType::_lookahead:
      std::get<1>(result) = index;
    case NodeType::_empty:
      table.expand(2);
      table[index]['\0'] = {index + 1};
      std::get<0>(result) = index + 1;
      break;
    case NodeType::_leaf:
      table.expand(2);
      table[index][root->leafType] = {index + 1};
      std::get<0>(result) = index + 1;
      break;
    case NodeType::_union:
      {
        table.expand(1);
        table[index]['\0'] = {index + 1};
        auto leftBranch = construct(root->left.get(), index + 1);
        int acceptStateLeft = std::get<0>(leftBranch);
        if(std::get<1>(leftBranch).has_value()){
          std::get<1>(result) = std::get<1>(leftBranch);
        }
        table[index]['\0'].emplace(acceptStateLeft + 1);
        auto rightBranch = construct(root->right.get(), acceptStateLeft + 1);
        int acceptStateRight = std::get<0>(rightBranch);
        if(std::get<1>(rightBranch).has_value()){
          std::get<1>(result) = std::get<1>(rightBranch);
        }
        table.expand(1);
        int acceptState = acceptStateRight + 1;
        table[acceptStateLeft]['\0'].emplace(acceptState);
        table[acceptStateRight]['\0'].emplace(acceptState);
        std::get<0>(result) = acceptState;
        break;
      }
    case NodeType::_concatenation:
      {
        auto leftBranch = construct(root->left.get(), index);
        int acceptStateLeft = std::get<0>(leftBranch);
        if(std::get<1>(leftBranch).has_value()){
          std::get<1>(result) = std::get<1>(leftBranch);
        }
        auto rightBranch = construct(root->right.get(), acceptStateLeft);
        int acceptState = std::get<0>(rightBranch);
        if(std::get<1>(rightBranch).has_value()){
          std::get<1>(result) = std::get<1>(rightBranch);
        }
        std::get<0>(result) = acceptState;
        break;
      }
    case NodeType::_star:
      {
        table.expand(1);
        auto branch = construct(root->center.get(), index + 1);
        int acceptStateChild = std::get<0>(branch);
        if(std::get<1>(branch).has_value()){
          std::get<1>(result) = std::get<1>(branch);
        }
        table.expand(1);
        int acceptState = acceptStateChild + 1;
        table[index]['\0'] = {index + 1, acceptState};
        table[acceptStateChild]['\0'] = {index + 1, acceptState};
        std::get<0>(result) = acceptState;
        break;
      }
  }
  return result;
}
