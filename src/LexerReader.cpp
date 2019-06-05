#include "LexerReader.h"

#include <algorithm>
#include <fstream>
#include <set>

LexerReader::LexerReader(const std::string& filename){
  std::map<std::string, std::string> definitions;
  enum {_definitions, _translation} mode = _definitions;
  std::ifstream file(filename);
  bool onNewLine = true;
  bool encounteredMarker = false;
  char c;
  std::string first;
  std::string second;
  enum {_firstSpace, _firstWord, _secondSpace, _secondWord} linePlace = _firstSpace;
  while(!file.eof()){
    file.get(c);
    if(onNewLine){
      first = "";
      second = "";
      linePlace = _firstSpace;
      if(c == '%'){
        encounteredMarker = true;
        onNewLine = false;
        continue;
      }
      if(c == '\n'){
        continue;
      }
    }
    switch(linePlace){
      case _firstSpace:
        if(std::isspace(c) == 0){
          linePlace = _firstWord;
        }
        break;
      case _firstWord:
        if(std::isspace(c) != 0){
          linePlace = _secondSpace;
        }
        break;
      case _secondSpace:
        if(std::isspace(c) == 0){
          linePlace = _secondWord;
        }
        break;
      case _secondWord:
        break;
    }
    switch(mode){
      case _definitions:
        if(encounteredMarker && c == '%'){
          mode = _translation;
        }
        else if(linePlace == _firstWord){
          first += c;
        }
        else if(linePlace == _secondWord){
          if(c == '\n'){
            while(std::isspace(second.back()) != 0){
              second.erase(second.end());
            }
            definitions.emplace(first, regexNotationConversion(second, definitions));
          }
          else {
            second += c;
          }
        }
        break;
      case _translation:
        if(linePlace == _firstWord){
          first += c;
        }
        else if(linePlace == _secondWord){
          if(c == '\n'){
            while(std::isspace(second.back()) != 0){
              second.erase(second.end());
            }
            regexNameList.emplace_back(std::make_tuple(regexNotationConversion(second, definitions), first));
          }
          else {
            second += c;
          }
        }
        break;
    }
    onNewLine = (c == '\n');
    encounteredMarker = false;
  }
}

//TODO(uridvir): Add support for ^ and $ operators
//Deals with "s", ., [s], [^s] and {definition} operators
std::string LexerReader::characterClassProcess(std::string regex, std::map<std::string, std::string>& definitions){
  std::string result;
  bool escape = false;
  auto kleeneReserved = std::set<char>({'(', ')', '|', '\\', '/', '*'});
  auto otherReserved = std::set<char>({'"', '.', '^', '$', '[', ']', '+', '?', '{', '}'});
  for(int i = 0; i < regex.length(); i++){
    if(regex[i] == '\\' && !escape){
      escape = true;
      continue;
    }
    if(regex[i] == '"' && !escape){
      std::string content;
      for(int j = i + 1; j < regex.length(); j++){
        if(regex[j] == '"'){
          i = j;
          break;
        }
        if(kleeneReserved.count(regex[j]) == 1 || otherReserved.count(regex[j]) == 1){
          if(regex[j] == '\\' && j + 1 < regex.length()){
            if(regex[j + 1] == 't'){
              content += '\t';
              continue;
            }
            if(regex[j + 1] == 'n'){
              content += '\n';
              continue;
            }
          }
          content += '\\';
        }
        content += regex[j];
      }
      result += "(" + content + ")";
    }
    else if(regex[i] == '.' && !escape){
      result += characterClassProcess("[" + constants::alphabetString + "]", definitions);
    }
    else if(regex[i] == '[' && !escape){
      std::string content;
      if(i + 1 < regex.length() && regex[i + 1] == '^'){
        auto unblocked = constants::alphabetSet;
        for(int j = i + 2; j < regex.length(); j++){
          if(regex[j] == '\\' && !escape){
            escape = true;
            continue;
          }
          if(regex[j] == ']' && !escape){
            i = j;
            break;
          }
          if(regex[j] == 't' && escape){
            unblocked.erase('\t');
          }
          else if(regex[j] == 'n' && escape){
            unblocked.erase('\n');
          }
          else {
            unblocked.erase(regex[j]);
          }
          if(j + 2 < regex.length() && regex[j + 1] == '-'){
            for(char c = regex[j] + 1; c <= regex[j + 2]; c++){
              unblocked.erase(c);
            }
            j += 2;
          }
          escape = false;
        }
        int j = 0;
        for(char c : unblocked){
          if(kleeneReserved.count(c) == 1 || otherReserved.count(c) == 1){
            content += '\\';
          }
          content += c;
          if(j + 1 < unblocked.size()){
            content += '|';
          }
          j++;
        }
      }
      else {
        for(int j = i + 1; j < regex.length(); j++){
          if(regex[j] == '\\'){
            escape = true;
            continue;
          }
          if(regex[j] == ']' && !escape){
            i = j;
            break;
          }
          if(regex[j] == 't' && escape){
            content += '\t';
          }
          else if(regex[j] == 'n' && escape){
            content += '\n';
          }
          else {
            content += regex[j];
          }
          if(j + 1 < regex.length() && regex[j + 1] != ']'){
            content += '|';
          }
          if(j + 2 < regex.length() && regex[j + 1] == '-' && regex[j + 2] != ']'){
            for(char c = regex[j] + 1; c <= regex[j + 2]; c++){
              if(kleeneReserved.count(c) == 1 || otherReserved.count(c) == 1){
                content += '\\';
              }
              content += c;
              if(c + 1 <= regex[j + 2] || (j + 3 < regex.length() && regex[j + 3] != ']')){
                content += '|';
              }
            }
            j += 2;
          }
          escape = false;
        }
      }
      result += "(" + content + ")";
    }
    else if(regex[i] == '{' && !escape){
      std::string name;
      for(int j = i + 1; j < regex.length(); j++){
        if(regex[j] == '}'){
          i = j;
          break;
        }
        name += regex[j];
      }
      result += "(" + definitions[name] + ")";
    }
    else if(regex[i] == 't' && escape){
      result += '\t';
    }
    else if(regex[i] == 'n' && escape){
      result += '\n';
    }
    else {
      if(escape){
        result += '\\';
      }
      result += regex[i];
    }
    escape = false;
  }
  return result;
}

//Calls characterClassProcess and then deals with +, ? and {m, n} operators
std::string LexerReader::regexNotationConversion(std::string regex, std::map<std::string, std::string>& definitions){
  regex = characterClassProcess(std::move(regex), definitions);
  std::string result;
  bool escape = false;
  for(int i = 0; i < regex.length(); i++){
    if(regex[i] == '\\' && !escape){
      escape = true;
      result += '\\';
      continue;
    }
    if(regex[i] == '(' && !escape){
      int depth = 1;
      std::string contents;
      for(int j = i + 1; j < regex.length(); j++){
        if(regex[j] == '\\' && !escape){
          escape = true;
          continue;
        }
        if(regex[j] == '(' && !escape){
          depth++;
        }
        if(regex[j] == ')' && !escape){
          depth--;
          if(depth == 0){
            contents = regexNotationConversion(regex.substr(i + 1, j - i - 1), definitions);
            i = j;
            break;
          }
        }
        escape = false;
      }
      if(i + 1 < regex.length() && regex[i + 1] == '+'){
        result.append("(" + contents + ")(").append(contents + ")*");
        i++;
      }
      else if(i + 1 < regex.length() && regex[i + 1] == '?'){
        result += "((" + contents + ")|\\0)";
        i++;
      }
      else if(i + 2 < regex.length() && regex[i + 1] == '{'
          && std::set<char>({'1','2','3','4','5','6','7','8','9','0'}).count(regex[i + 2]) == 1){

        std::string first;
        std::string second;
        bool beforeComma = true;
        for(int j = i + 2; j < regex.length(); j++){
          if(regex[j] == ','){
            beforeComma = false;
            continue;
          }
          if(regex[j] == '}'){
            i = j;
            break;
          }
          if(beforeComma){
            first += regex[j];
          }
          else {
            second += regex[j];
          }
        }
        int a = std::stoi(first);
        int b = std::stoi(second);
        result += '(';
        for(int j = a; j <= b; j++){
          for(int k = 0; k < j; k++){
            result += "(" + contents + ")";
          }
          if(j != b){
            result += '|';
          }
        }
        result += ')';
      }
      else {
        result += "(" + contents + ")";
      }
    }
    else {
      result += regex[i];
    }
    escape = false;
  }
  return result;
}
