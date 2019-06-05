#ifndef LEXER_READER_H
#define LEXER_READER_H

#include "constants.h"

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

class LexerReader {
  public:
    std::vector<std::tuple<std::string, std::string> > regexNameList;

    static std::string characterClassProcess(std::string regex, std::map<std::string, std::string>& definitions);
    static std::string regexNotationConversion(std::string regex, std::map<std::string, std::string>& definitions);
    explicit LexerReader(const std::string& filename);
};

#endif
