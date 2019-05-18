#include "RegexNotationConversion.h"

#include <algorithm>
#include <set>

//TODO(uridvir): Finish function, add parentheses and other support
//Converts Lex regex notation to Kleene notation
std::string regexNotationConversion(std::string regex){
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
        if(kleeneReserved.count(regex[j]) == 1){
          if(regex[j] == '\\' && j + 1 < regex.length() && regex[j + 1] == '0'){
            content += "\\0";
            continue;
          }
          content += '\\';
        }
        if(regex[j] == '"'){
          i = j;
          break;
        }
        content += regex[j];
      }
      result += regexNotationConversion("(" + content + ")" + regex.substr(i + 1));
      break;
    }
    if(regex[i] == '.' && !escape){
      result += regexNotationConversion("[" + allCharacters + "]" + regex.substr(i + 1));
      break;
    }
    if(regex[i] == '[' && !escape){
      std::string content;
      if(i + 1 < regex.length() && regex[i + 1] == '^'){
        auto unblocked = allCharactersSet;
        for(int j = i + 2; j < regex.length(); j++){
          if(regex[j] == '\\' && !escape){
            escape = true;
            continue;
          }
          if(regex[j] == ']' && !escape){
            i = j;
            break;
          }
          unblocked.erase(regex[j]);
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
          if(kleeneReserved.count(regex[j]) == 1 || otherReserved.count(regex[j]) == 1){
            content += '\\';
          }
          if(regex[j] == ']' && !escape){
            i = j;
            break;
          }
          content += regex[j];
          if(j + 1 < regex.length()){
            content += '|';
          }
          escape = false;
        }
      }
    result += regexNotationConversion("(" + content + ")" + regex.substr(i + 1));
    break;
    }
  }
  return result;
}
