#include "DFA.h"

DFA::DFA(NFA nfa){
  this->nfa = &nfa;
  construct();
}

//Performs subset construction to convert from NFA to DFA
void DFA::construct(){
  std::set<std::set<int> > unmarked = {e_closure(std::set<int>({0}))};
  std::set<std::set<int> > Dstates;
  std::map<std::set<int>, int> stateIndexMap;
  int i = 0;
  int j = 0;
  while(!unmarked.empty()){
    std::set<int> states = unmarked.extract(unmarked.begin()).value();
    stateIndexMap.emplace(states, i);
    table.expand(1);
    Dstates.emplace(states);
    for(char c : constants::alphabetSet){
      std::set<int> newStates = e_closure(move(states, c));
      if(newStates.empty()){
        continue;
      }
      if(Dstates.count(newStates) == 0){
        unmarked.emplace(newStates);
        j++;
        stateIndexMap.emplace(newStates, j);
        table[i][c] = j;
      }
      else {
        table[i][c] = stateIndexMap[newStates];
      }
    }
    j++;
    i = j;
  }
  for(const auto& states : Dstates){
    int index = stateIndexMap[states];
    for(int k : nfa->lookaheadStates){
      if(states.count(k) == 1){
        lookaheadStates.emplace(index);
        break;
      }
    }
    for(int k : nfa->acceptStates){
      if(states.count(k) == 1){
        acceptStates.emplace(index);
        tokens.emplace(index, nfa->tokens.at(k));
        if(nfa->lookaheadMap.count(k) == 1){
          int lookaheadNFA = nfa->lookaheadMap[k];
          for(auto map : stateIndexMap){
            if(map.first.count(lookaheadNFA) == 1){
              lookaheadMap.emplace(index, map.second);
              break;
            }
          }
        }
        break;
      }
    }
  }
}

//Returns the set of states that can be reached through e-transitions from the set of states "states"
std::set<int> DFA::e_closure(std::set<int> states){
  std::set<int> result = states;
  while(!states.empty()){
    int t = states.extract(states.begin()).value();
    for(int state : nfa->table[t]['\0']){
      result.emplace(state);
      states.emplace(state);
    }
  }
  return result;
}

//Returns the set of states that can be reached from "states" through transitions on "symbol"
std::set<int> DFA::move(std::set<int> states, char symbol){
  std::set<int> result;
  for(int state : states){
    for(int newState : nfa->table[state][symbol]){
      result.emplace(newState);
    }
  }
  return result;
}
