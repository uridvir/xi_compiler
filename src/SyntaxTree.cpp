#include "SyntaxTree.h"

std::unique_ptr<Node> SyntaxTree::evaluate(std::string regex){
  auto result = std::make_unique<Node>();
  bool escape = false;
  for(int i = 0; i < regex.length(); i++){
    if(regex[i] == '\\' && !escape){
      escape = true;
      continue;
    }
    if(regex[i] == '(' && !escape){
      int depth = 1;
      std::string contents;
      int endpos = -1;
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
            contents = regex.substr(i + 1, j - i - 1);
            endpos = j;
            break;
          }
        }
        escape = false;
      }
      auto branch = std::make_unique<Node>();
      if(endpos + 1 < regex.length() && regex[endpos + 1] == '*'){
        branch = std::make_unique<Node>(evaluate(contents), NodeType::_star);
        i = endpos + 1;
      }
      else {
        branch = evaluate(contents);
        i = endpos;
      }
      if(result->type == NodeType::_empty){
        result = std::move(branch);
      }
      else {
        result = std::make_unique<Node>(std::move(result), std::move(branch), NodeType::_concatenation);
      }
      continue;
    }
    if(regex[i] == '|' && !escape){
      std::string remainder = regex.substr(i + 1, regex.length());
      result = std::make_unique<Node>(std::move(result), evaluate(remainder), NodeType::_union);
      break;
    }

    std::unique_ptr<Node> leaf;
    if(regex[i] == '0' && escape){
      leaf = std::make_unique<Node>(NodeType::_empty);
    }
    else if(regex[i] == '/' && !escape){
      leaf = std::make_unique<Node>(NodeType::_lookahead);
    }
    else{
      leaf = std::make_unique<Node>(regex[i]);
    }
    auto branch = std::make_unique<Node>();
    if(i + 1 < regex.length() && regex[i + 1] == '*'){
      branch = std::make_unique<Node>(std::move(leaf), NodeType::_star);
      i++;
    }
    else {
      branch = std::move(leaf);
    }
    if(result->type == NodeType::_empty){
      result = std::move(branch);
    }
    else {
      result = std::make_unique<Node>(std::move(result), std::move(branch), NodeType::_concatenation);
    }
    escape = false;
  }
  return result;
}
