//#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "syntax_tree_test.cpp"

class NFA {
  public:
  std::vector<std::map<int, std::optional<char> > > transitions;
  std::set<int> acceptStates;
  std::map<int, std::string> tokens;
  NFA() = default;
  explicit NFA(std::vector<std::tuple<std::string, std::string> > tokenRegexList);

  private:
  int construct(const Node* root, int index);
};

NFA::NFA(std::vector<std::tuple<std::string, std::string> > tokenRegexList){
  transitions.resize(1);
  acceptStates = {};
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

int NFA::construct(const Node* root, int index){
  int result = -1;
  switch(root->type){
    case NodeType::_none:
      std::cout << "Node has no type!" << std::endl; //debug
      break;
    case NodeType::_leafA:
      {
        transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1, std::optional<char>('a')}}));
        transitions.emplace_back(std::map<int, std::optional<char> >());
        result = index + 1;
        break;
      }
    case NodeType::_leafB:
      {
        transitions.emplace_back(std::map<int, std::optional<char> >({{index + 1, std::optional<char>('b')}}));
        transitions.emplace_back(std::map<int, std::optional<char> >());
        result = index + 1;
        break;
      }
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

/*
int main(){
  std::vector<std::tuple<std::string, std::string> > tokenRegexList =
    {
      std::make_tuple("a", "a"),
      std::make_tuple("abb", "abb"),
      std::make_tuple("a*bb*", "a*bb*"),
      std::make_tuple("(a|b)*abb", "(a|b)*abb")
    };
  NFA superNFA(tokenRegexList);
  std::cout << "Transitions:" << std::endl;
  int i = 0;
  for(auto state : superNFA.transitions){
    for(auto transition : state){
      std::cout << i << " " << transition.second.value_or('e') << "-> " << transition.first << std::endl;
    }
    i++;
  }
  std::cout << std::endl << std::endl << "Accept states:" << std::endl;
  for(int state : superNFA.acceptStates){
    std::cout << state << " ";
  }
  std::cout << std::endl << std::endl << std::endl << "Tokens:" << std::endl;
  for(auto pair : superNFA.tokens){
    std::cout << "\"" << pair.second << "\" at " << pair.first << std::endl;
  }
}
*/
