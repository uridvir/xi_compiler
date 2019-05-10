#include "DFA.h"

DFA::DFA(NFA nfa){
  transitions = {};
  acceptStates = {};
  lookaheadStates = {};
  tokens = {};
  this->nfa = &nfa;
  construct();
}

//Performs subset construction to convert from NFA to DFA
void DFA::construct(){
  std::set<std::set<int> > unmarked = {e_closure(std::set<int>({0}))};
  std::set<std::set<int> > Dstates;
  std::map<std::set<int>, std::map<char, std::set<int> > > Dtran;
  while(!unmarked.empty()){
    std::set<int> states = *unmarked.begin();
    unmarked.erase(unmarked.begin());
    Dstates.emplace(states);
    Dtran.emplace(states, std::map<char, std::set<int> >());
    for(char c : getAlphabet(states)){
      std::set<int> newStates = e_closure(move(states, c));
      if(newStates.empty()){
        continue;
      }
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
    for(int j : nfa->lookaheadStates){
      if(pair.first.count(j) == 1){
        lookaheadStates.emplace(i);
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

//Returns the set of states that can be reached through e-transitions from "states"
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
      if(!transition.second.has_value() && result.count(transition.first) == 0){
        result.emplace(transition.first);
        stack.emplace_back(transition.first);
      }
    }
  }
  return result;
}

//Returns the set of letters which the given set of states has transitions on
std::set<char> DFA::getAlphabet(std::set<int> states){
  std::set<char> result;
  for(int state : states){
    for(auto transition : nfa->transitions[state]){
      if(transition.second.has_value()){
        result.emplace(transition.second.value());
      }
    }
  }
  return result;
}

//Returns the set of states that can be reached from "states" through transitions on "symbol"
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
