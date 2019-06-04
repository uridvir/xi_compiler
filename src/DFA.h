#ifndef DFA_H
#define DFA_H

#include "constants.h"
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
    std::map<int, int> lookaheadMap; //maps accept states to lookaheads
    std::map<int, std::string> tokens; //maps accept states to token names
    explicit DFA(NFA nfa);

  private:
    NFA* nfa;
    void construct();
    std::set<int> e_closure(std::set<int> states);
    std::set<int> move(std::set<int> states, char symbol);
};

#endif
