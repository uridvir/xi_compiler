#ifndef LEXER_READER_H
#define LEXER_READER_H

#include "constants.h"

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

class LexerReader {
  private:
    std::string characterClassProcess(std::string regex, std::map<std::string, std::string>& definitions);

  public:
    std::vector<std::tuple<std::string, std::string> > regexNameList;

    explicit LexerReader(const std::string& filename);

    //Converts to Kleene notation
    std::string regexNotationConversion(std::string regex, std::map<std::string, std::string>& definitions);
};

#endif
