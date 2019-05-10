#ifndef DFA_H
#define DFA_H

#include "NFA.h"

#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

class DFA {
  public:
  std::vector<std::map<char, int> > transitions;
  std::set<int> acceptStates;
  std::set<int> lookaheadStates;
  std::map<int, std::string> tokens;
  explicit DFA(NFA nfa);

  private:
  NFA* nfa;
  void construct();
  std::set<int> e_closure(std::set<int> states);
  std::set<char> getAlphabet(std::set<int> states);
  std::set<int> move(std::set<int> states, char symbol);
};

#endif
