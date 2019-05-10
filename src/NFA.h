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
  std::map<int, std::string> tokens;
  NFA() = default;
  explicit NFA(std::vector<std::tuple<std::string, std::string> > tokenRegexList);

  private:
  int construct(const Node* root, int index);
};

#endif
