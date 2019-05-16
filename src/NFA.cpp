#include "NFA.h"

//Takes list of regexes, constructs them, and then links them to state 0 with e-transitions
NFA::NFA(std::vector<std::tuple<std::string, std::string> > tokenRegexList){
  transitions.resize(1);
  acceptStates = {};
  lookaheadStates = {};
  tokens = {};
  int index = 1;
  for (auto pair : tokenRegexList){
    transitions[0].emplace(index, std::nullopt);
    SyntaxTree tree(std::get<0>(pair));
    auto branch = construct(tree.root, index);
    int acceptState = std::get<0>(branch);
    if(std::get<1>(branch).has_value()){
      lookaheadMap.emplace(acceptState, std::get<1>(branch).value());
      lookaheadStates.emplace(std::get<1>(branch).value());
    }
    acceptStates.emplace(acceptState);
    tokens.emplace(acceptState, std::get<0>(pair));
    index = acceptState + 1;
  }
}

//Constructs the NFA transition table for "root" starting at the given index
//Returns a tuple with the new index and the location of any lookahead operator
std::tuple<int, std::optional<int> > NFA::construct(const Node* root, int index){
  auto result = std::make_tuple(-1, std::optional<int>());
  switch(root->type){
    case NodeType::_lookahead:
      std::get<1>(result) = index;
    case NodeType::_empty:
      transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1, std::nullopt}}));
      transitions.emplace_back(std::map<int, std::optional<char> >());
      std::get<0>(result) = index + 1;
      break;
    case NodeType::_leaf:
      transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1,
            std::optional<char>(root->leafType)}}));
      transitions.emplace_back(std::map<int, std::optional<char> >());
      std::get<0>(result) = index + 1;
      break;
    case NodeType::_union:
      {
        transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1, std::nullopt}}));
        auto leftBranch = construct(root->left.get(), index + 1);
        int acceptStateLeft = std::get<0>(leftBranch);
        if(std::get<1>(leftBranch).has_value()){
          std::get<1>(result) = std::get<1>(leftBranch);
        }
        transitions[index].emplace(acceptStateLeft + 1, std::nullopt);
        auto rightBranch = construct(root->right.get(), acceptStateLeft + 1);
        int acceptStateRight = std::get<0>(rightBranch);
        if(std::get<1>(rightBranch).has_value()){
          std::get<1>(result) = std::get<1>(rightBranch);
        }
        int acceptState = acceptStateRight + 1;
        transitions.emplace_back(std::map<int, std::optional<char> >());
        transitions[acceptStateLeft].emplace(acceptState, std::nullopt);
        transitions[acceptStateRight].emplace(acceptState, std::nullopt);
        std::get<0>(result) = acceptStateRight + 1;
        break;
      }
    case NodeType::_concatenation:
      {
        auto leftBranch = construct(root->left.get(), index);
        int acceptStateLeft = std::get<0>(leftBranch);
        if(std::get<1>(leftBranch).has_value()){
          std::get<1>(result) = std::get<1>(leftBranch);
        }
        transitions.pop_back();
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
        transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1, std::nullopt}}));
        auto branch = construct(root->center.get(), index + 1);
        int acceptStateChild = std::get<0>(branch);
        if(std::get<1>(branch).has_value()){
          std::get<1>(result) = std::get<1>(branch);
        }
        int acceptState = acceptStateChild + 1;
        transitions.emplace_back(std::map<int, std::optional<char> >());
        transitions[index].emplace(acceptState, std::nullopt);
        transitions[acceptStateChild].emplace(index + 1, std::nullopt);
        transitions[acceptStateChild].emplace(acceptState, std::nullopt);
        std::get<0>(result) = acceptState;
        break;
      }
  }
  return result;
}
