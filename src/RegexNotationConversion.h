#ifndef REGEX_NOTATION_CONVERSION_H
#define REGEX_NOTATION_CONVERSION_H

#include <set>
#include <string>

const std::string allCharacters = R"( !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`)"
      + R"(abcdefghijklmnopqrstuvwxyz{|}~)";
const std::set<char> allCharactersSet = std::set<char>(allCharacters.begin(), allCharacters.end());

std::string regexNotationConversion(std::string regex);

#endif
