#ifndef LEXER_READER_H
#define LEXER_READER_H

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

class LexerReader {
  private:
    const std::string allCharacters = R"( !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`)"
          R"(abcdefghijklmnopqrstuvwxyz{|}~)" "\t\n";
    const std::set<char> allCharactersSet = std::set<char>(allCharacters.begin(), allCharacters.end());

    std::string characterClassProcess(std::string regex);

  public:
    std::vector<std::tuple<std::string, std::string> > regexNameList;

    explicit LexerReader(const std::string& filename);

    //Converts to Kleene notation
    std::string regexNotationConversion(std::string regex, std::map<std::string, std::string> definitions);
};

#endif
