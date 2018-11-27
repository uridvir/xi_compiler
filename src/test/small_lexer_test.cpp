#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <optional>
#include <tuple>

enum Token {_null, _failure, _if, _then};

class DFA {
  static int state;
  static int offset;
  static std::vector<std::map<char, int> > transitions;
  static std::set<int> acceptStates;
  static std::map<int, Token> tokens;
  static std::optional<int> lastAcceptState;
  static int lastOffset;

  public:
  static std::tuple<Token, int> feed(char c){
    //std::cout << "ate '" << c << "', state = " << state << std::endl; //debug
    offset++;
    if(transitions[state].count(c) == 1){
      state = transitions[state].at(c);
      if(acceptStates.count(state) == 1){
        lastAcceptState = state;
        lastOffset = offset;
      }
      return std::make_tuple(Token::_null, offset);
    }
    else {
      if(lastAcceptState.has_value()){
        return std::make_tuple(tokens.at(lastAcceptState.value()), lastOffset);
      }
      else {
        //std::cout << "failed to match lexeme" << std::endl; //debug
        return std::make_tuple(Token::_failure, offset);
      }
    }
  }

  static void initialize(){
    state = 0;
    offset = 0;
    lastAcceptState = std::nullopt;
  }

};

std::vector<std::map<char, int> > DFA::transitions =
  {
    std::map<char, int>({{'i',1}, {'t',3}}),
    std::map<char, int>({{'f',2}}),
    std::map<char, int>({}),
    std::map<char, int>({{'h',4}}),
    std::map<char, int>({{'e',5}}),
    std::map<char, int>({{'n',6}}),
    std::map<char, int>({})
  };
std::set<int> DFA::acceptStates = {2, 6};
std::map<int, Token> DFA::tokens = {{2,Token::_if}, {6,Token::_then}};

void printBuffer(std::vector<char>& buffer, std::string name){ //debug
  std::cout << name << " = \"";
  for(char c : buffer){
    std::cout << c;
  }
  std::cout << "\"" << std::endl;
}

int main(int argc, char* argv[]){
  DFA::initialize();

  const int bufferSize = 4;
  std::vector<char> buffer1(bufferSize, 0);
  std::vector<char> buffer2(bufferSize, 0);
  std::ifstream fin("test.txt");

  int lexemeBegin = 0;
  bool onFirstBuffer = true;
  bool lexemeBeginsFirstBuffer = true;

  while(true){
    std::vector<char>& buffer = onFirstBuffer ? buffer1 : buffer2;
    fin.read(&buffer[0], bufferSize);
    //printBuffer(buffer1, "buffer1"); //debug
    //printBuffer(buffer2, "buffer2"); //debug
    //std::cout << "on buffer" << (onFirstBuffer ? "1" : "2") << std::endl; //debug
    if(fin.gcount() != 0){
      for(int forward = 0; forward < bufferSize; forward++){
        auto result = DFA::feed(buffer[forward]);
        Token token = std::get<0>(result);
        if(token != Token::_null){
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
          DFA::initialize();
          switch(token){
            case Token::_failure:
              std::cout << "terminating lexical analysis" << std::endl;
              return 0;
            case Token::_if:
              std::cout << "if token" << std::endl;
              break;
            case Token::_then:
              std::cout << "then token" << std::endl;
              break;
            default:
              std::cout << "unknown token" << std::endl;
              break;
          }
        }
      }
      onFirstBuffer = !onFirstBuffer;
    }
    else {
      break;
    }
  }
}
