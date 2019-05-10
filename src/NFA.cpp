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
    int acceptState = construct(tree.root, index);
    acceptStates.emplace(acceptState);
    tokens.emplace(acceptState, std::get<0>(pair));
    index = acceptState + 1;
  }
}

//Constructs the NFA transition table for "root" starting at the given index
int NFA::construct(const Node* root, int index){
  int result = -1;
  switch(root->type){
    case NodeType::_lookahead:
      lookaheadStates.emplace(index);
    case NodeType::_empty:
      transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1, std::nullopt}}));
      transitions.emplace_back(std::map<int, std::optional<char> >());
      result = index + 1;
      break;
    case NodeType::_leaf:
      transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1,
            std::optional<char>(root->leafType)}}));
      transitions.emplace_back(std::map<int, std::optional<char> >());
      result = index + 1;
      break;
    case NodeType::_union:
      {
        transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1, std::nullopt}}));
        int acceptStateLeft = construct(root->left.get(), index + 1);
        transitions[index].emplace(acceptStateLeft + 1, std::nullopt);
        int acceptStateRight = construct(root->right.get(), acceptStateLeft + 1);
        int acceptState = acceptStateRight + 1;
        transitions.emplace_back(std::map<int, std::optional<char> >());
        transitions[acceptStateLeft].emplace(acceptState, std::nullopt);
        transitions[acceptStateRight].emplace(acceptState, std::nullopt);
        result = acceptStateRight + 1;
        break;
      }
    case NodeType::_concatenation:
      {
        int acceptStateLeft = construct(root->left.get(), index);
        transitions.pop_back();
        int acceptState = construct(root->right.get(), acceptStateLeft);
        result = acceptState;
        break;
      }
    case NodeType::_star:
      {
        transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1, std::nullopt}}));
        int acceptStateChild = construct(root->center.get(), index + 1);
        int acceptState = acceptStateChild + 1;
        transitions.emplace_back(std::map<int, std::optional<char> >());
        transitions[index].emplace(acceptState, std::nullopt);
        transitions[acceptStateChild].emplace(index + 1, std::nullopt);
        transitions[acceptStateChild].emplace(acceptState, std::nullopt);
        result = acceptState;
        break;
      }
  }
  return result;
}
