#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <set>
#include <string>

namespace constants {
  const std::string alphabetString = R"( !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`)"
        R"(abcdefghijklmnopqrstuvwxyz{|}~)" "\t\n";
  const std::set<char> alphabetSet = std::set<char>(alphabetString.begin(), alphabetString.end());
} //end of namespace constants

#endif
