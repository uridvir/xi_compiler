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
      result += characterClassProcess("[" + allCharacters + "]");
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
          content += regex[j];
          if(j + 1 < regex.length() && regex[j + 1] != ']'){
            content += '|';
          }
          if(j + 2 < regex.length() && regex[j + 1] == '-' && regex[j + 2] != ']'){
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
    else {
      if(escape && (kleeneReserved.count(regex[i]) == 1 || otherReserved.count(regex[i]) == 1)){
        result += '\\';
      }
      result += regex[i];
    }
    escape = false;
  }
  return result;
}

std::string regexNotationConversion(std::string regex){
  regex = characterClassProcess(std::move(regex));
  std::string result;
  bool escape = false;
  for(int i = 0; i < regex.length(); i++){
    if(regex[i] == '\\' && !escape){
      escape = true;
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
            contents = regexNotationConversion(regex.substr(i + 1, j - i - 1));
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
      //TODO(uridvir): Add support for definitions here
      else if(i + 2 < regex.length() && regex[i + 1] == '{'){
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
    }
    else {
      result += regex[i];
    }
    escape = false;
  }
  return result;
}
