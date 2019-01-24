#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "syntax_tree_to_nfa_test.cpp"

class DFA {
  public:
  std::vector<std::map<char, int> > transitions;
  std::set<int> acceptStates;
  std::map<int, std::string> tokens;
  explicit DFA(NFA nfa);

  private:
  NFA* nfa;
  void construct();
  std::set<int> e_closure(std::set<int> states);
  std::set<int> e_closure(int state);
  std::set<int> move(std::set<int> states, char symbol);
};

DFA::DFA(NFA nfa){
  transitions = {};
  acceptStates = {};
  tokens = {};
  this->nfa = &nfa;
  construct();
}

void DFA::construct(){
  std::set<std::set<int> > unmarked = {e_closure(0)};
  std::set<std::set<int> > Dstates;
  std::map<std::set<int>, std::map<char, std::set<int> > > Dtran;
  while(!unmarked.empty()){
    std::set<int> states = *unmarked.begin();
    unmarked.erase(unmarked.begin());
    Dstates.emplace(states);
    Dtran.emplace(states, std::map<char, std::set<int> >());
    for(char c : std::set<char>({'a', 'b'})){
      std::set<int> newStates = e_closure(move(states, c));
      if(Dstates.count(newStates) == 0){
        unmarked.emplace(newStates);
      }
      Dtran[states].emplace(c, newStates);
    }
  }
  std::map<std::set<int>, int> stateIndexMap;
  int i = 0;
  for(auto pair : Dtran){
    stateIndexMap.emplace(pair.first, i);
    for(int j : nfa->acceptStates){
      if(pair.first.count(j) == 1){
        acceptStates.emplace(i);
        tokens.emplace(i, nfa->tokens.at(j));
      }
    }
    i++;
  }
  for(auto pair : Dtran){
    std::map<char, int> transitionMap;
    for(auto transition : pair.second){
      transitionMap.emplace(transition.first, stateIndexMap[transition.second]);
    }
    this->transitions.emplace_back(transitionMap);
  }
}

std::set<int> DFA::e_closure(std::set<int> states){
  std::vector<int> stack;
  stack.reserve(states.size());
  for(int state : states){
    stack.emplace_back(state);
  }
  std::set<int> result = states;
  while(!stack.empty()){
    int t = stack.back();
    stack.pop_back();
    for(auto transition : nfa->transitions[t]){
      if(result.count(transition.first) == 0 && !transition.second.has_value()){
        result.emplace(transition.first);
        stack.emplace_back(transition.first);
      }
    }
  }
  return result;
}

std::set<int> DFA::e_closure(int state){
  return e_closure(std::set<int>({state}));
}

std::set<int> DFA::move(std::set<int> states, char symbol){
  std::set<int> result;
  for(int state : states){
    for(auto transition : nfa->transitions[state]){
      if(transition.second.has_value() && transition.second.value() == symbol){
        result.emplace(transition.first);
      }
    }
  }
  return result;
}

int main(){
  std::vector<std::tuple<std::string, std::string> > regexTokenList =
    {
      std::make_tuple("a", "a"),
      std::make_tuple("abb", "abb")
    };
  NFA nfa(regexTokenList);
  DFA dfa(nfa);
  std::cout << "Transitions:" << std::endl;
  int i = 0;
  for(auto state : dfa.transitions){
    for(const auto& transition : state){
      std::cout << i << " " << transition.first << "-> " << transition.second << std::endl;
    }
    i++;
  }
  std::cout << std::endl << std::endl << "Accept states:" << std::endl;
  for(int state : dfa.acceptStates){
    std::cout << state << " ";
  }
  std::cout << std::endl << std::endl << std::endl << "Tokens:" << std::endl;
  for(auto pair : dfa.tokens){
    std::cout << "\"" << pair.second << "\" at " << pair.first << std::endl;
  }
}
