#ifndef LEXER_READER_H
#define LEXER_READER_H

#include <map>
#include <set>
#include <string>

class LexerReader {
  private:
    const std::string allCharacters = R"( !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`)"
          R"(abcdefghijklmnopqrstuvwxyz{|}~)";
    const std::set<char> allCharactersSet = std::set<char>(allCharacters.begin(), allCharacters.end());

    std::string characterClassProcess(std::string regex);

  public:
    //Text to be inserted in lexer source file verbatim
    std::string frontAuxiliary;
    std::string backAuxiliary;

    std::map<std::string, std::string> regexActionMap;

    explicit LexerReader(const std::string& filename);

    //Converts to Kleene notation
    std::string regexNotationConversion(std::string regex, std::map<std::string, std::string> definitions);
};

#endif
