#include "RegexNotationConversion.h"

#include <algorithm>
#include <set>

//TODO(uridvir): Add support for ^ and $ operators
//Processes character class operators as the first stage
std::string characterClassProcess(std::string regex){
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
      result += "(" + content + ")";
    }
    else if(regex[i] == '.' && !escape){
      result += characterClassProcess("[" + allCharacters + "]" + regex.substr(i + 1));
      break;
    }
    else if(regex[i] == '[' && !escape){
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
          if(kleeneReserved.count(regex[j]) == 1 || otherReserved.count(regex[j]) == 1){
            content += '\\';
          }
          content += regex[j];
          if(j + 1 < regex.length() && regex[j + 1] != ']'){
            content += '|';
          }
          if(j + 2 < regex.length() && regex[j + 1] == '-'){
            for(char c = regex[j] + 1; c <= regex[j + 2]; c++){
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
    else if(!escape){
      result += regex[i];
    }
  }
  return result;
}
