#ifndef NFA_H
#define NFA_H

#include "SyntaxTree.h"

#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

class NFA {
  public:
  std::vector<std::map<int, std::optional<char> > > transitions;
  std::set<int> acceptStates;
  std::set<int> lookaheadStates;
  std::map<int, int> lookaheadMap; //maps accept states to lookahead states
  std::map<int, std::string> tokens; //maps accept states to token names
  NFA() = default;
  explicit NFA(std::vector<std::tuple<std::string, std::string> > tokenRegexList);

  private:
  std::tuple<int, std::optional<int> > construct(const Node* root, int index);
};

#endif
