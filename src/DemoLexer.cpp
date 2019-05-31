#include "DFA.h"
#include "LexerReader.h"
#include "NFA.h"
#include "SyntaxTree.h"

#include <fstream>
#include <iostream>
#include <string.h>

DFA* dfa;
int state;
int offset;
std::optional<int> lastAcceptState;
std::optional<int> lastLookaheadState;
int lastAcceptOffset;
int lastLookaheadOffset;

std::tuple<std::optional<std::string>, int> feed(char c){
  //std::cout << "ate '" << c << "', state = " << state << std::endl; //debug
  offset++;
  auto token = std::optional<std::string>();
  if(dfa->transitions[state].count(c) == 1){
    state = dfa->transitions[state].at(c);
    if(dfa->acceptStates.count(state) == 1){
      lastAcceptState = state;
      lastAcceptOffset = offset;
    }
    if(dfa->lookaheadStates.count(state) == 1){
      lastLookaheadState = state;
      lastLookaheadOffset = offset;
    }
  }
  else if(lastAcceptState.has_value()){
    int lastOffset;
    if(dfa->lookaheadMap.count(lastAcceptState.value()) == 1){
      if(dfa->lookaheadMap[lastAcceptState.value()] == lastLookaheadState){
        lastOffset = lastLookaheadOffset;
      }
      else { //debug
        std::cout << "Accept state points to wrong lookahead!\n";
        lastOffset = -1;
      }
    }
    else {
      lastOffset = lastAcceptOffset;
    }
    return std::make_tuple(dfa->tokens.at(lastAcceptState.value()), lastOffset);
  }
  return std::make_tuple(token, offset);
}

void initialize(){
  state = 0;
  offset = 0;
  lastAcceptState = std::nullopt;
  lastLookaheadState = std::nullopt;
}

void runLexer(const std::string& definitionFile, const std::string& sourceFile){
  LexerReader reader(definitionFile);
  NFA nfa(reader.regexNameList);
  *dfa = DFA(nfa);

  const int bufferSize = 4096;
  std::vector<char> buffer1(bufferSize, 0);
  std::vector<char> buffer2(bufferSize, 0);
  std::ifstream fin(sourceFile);

  int lexemeBegin = 0;
  bool onFirstBuffer = true;
  bool lexemeBeginsFirstBuffer = true;

  while(true){
    std::vector<char>& buffer = onFirstBuffer ? buffer1 : buffer2;
    fin.read(&buffer[0], bufferSize);
    //printBuffer(buffer1, "buffer1"); //debug
    //printBuffer(buffer2, "buffer2"); //debug
    //std::cout << "on buffer" << (onFirstBuffer ? "1" : "2") << std::endl; //debug
    int forward;
    if(fin.gcount() != 0){
      for(forward = 0; forward < bufferSize; forward++){
        auto result = feed(buffer[forward]);
        auto token = std::get<0>(result);
        if(token.has_value()){
          int offset = std::get<1>(result);
          if(onFirstBuffer == lexemeBeginsFirstBuffer){
            lexemeBegin += offset;
          }
          else {
            lexemeBegin = (lexemeBegin + offset) % bufferSize;
            lexemeBeginsFirstBuffer = !lexemeBeginsFirstBuffer;
          }
          forward = lexemeBegin - 1;
          //std::cout << "lexeme begins on buffer" << (lexemeBeginsFirstBuffer ? "1" : "2") << std::endl; //debug
          //std::cout << "forward = " << forward << std::endl; //debug
          //std::cout << "lexemeBegin = " << lexemeBegin << std::endl; //debug
          initialize();
          std::cout << token.value() + " token\n";
        }
        else if(buffer[forward] == 0){
          break;
        }
      }
    onFirstBuffer = !onFirstBuffer;
    }
    else {
      break;
    }
  }
}

int main(int argc, char* argv[]){
  std::string definitionFile;
  std::string sourceFile;
  std::string errorMessage = "Type \"" + std::string(argv[0]) + " --help\" for correct usage.";
  
  if(argc == 1){
    std::cout << errorMessage << std::endl;
    return 0;
  }

  int i = 1;
  while(i < argc){
    if((strcmp(argv[i], "-definition") == 0 || strcmp(argv[i], "-def") == 0) && i + 1 < argc){
      definitionFile = argv[i + 1];
      i += 2;
    }
    else if((strcmp(argv[i], "-source") == 0 || strcmp(argv[i], "-src") == 0) && i + 1 < argc){
      sourceFile = argv[i + 1];
      i += 2;
    }
    else if(strcmp(argv[i], "--help") == 0){
      std::cout << std::endl;
      std::cout << "Uri's Ma'avar Compiler Project" << std::endl;
      std::cout << std::endl;
      std::cout << "Usage:" << std::endl;
      std::cout << argv[0] << " -definition [filename] -source [file]" << std::endl;
      std::cout << std::endl;
      std::cout << "-definition     Sets the definition file for the lexer (shortcut is -def)" << std::endl;
      std::cout << "-source         Sets the source file which the lexer shall lex (shortcut is -src)" << std::endl;
      std::cout << "--help          Summons this help menu" << std::endl;
      i += 1;
    }
    else {
      std::cout << errorMessage << std::endl;
      return 0;
    }
  }
  
  runLexer(definitionFile, sourceFile);
  return 0;
}
