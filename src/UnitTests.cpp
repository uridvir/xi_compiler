#include "DFA.h"
//#include "LexerReader.h"
#include "NFA.h"
#include "SyntaxTree.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>

namespace SyntaxTreeTest {

  std::tuple<std::string, int> printNode(const std::unique_ptr<Node>& root, int nodeLabel){
    std::string output;
    int nodeLabelFinal;
    switch(root->type){
      case NodeType::_empty:
        return std::make_tuple("n" + std::to_string(nodeLabel) + " = empty\n", nodeLabel);
      case NodeType::_union:
        {
          int nodeLabelLeft = nodeLabel + 1;
          auto leftPrinted = printNode(root->left, nodeLabelLeft);
          std::string leftOutput = std::get<0>(leftPrinted);
          int nodeLabelRight = std::get<1>(leftPrinted) + 1;

          auto rightPrinted = printNode(root->right, nodeLabelRight);
          std::string rightOutput = std::get<0>(rightPrinted);
          nodeLabelFinal = std::get<1>(rightPrinted);

          output = "n" + std::to_string(nodeLabel) + " = n" + std::to_string(nodeLabelLeft) + " | n"
            + std::to_string(nodeLabelRight) + "\n" + leftOutput + rightOutput;
          break;
        }
      case NodeType::_concatenation:
        {
          int nodeLabelLeft = nodeLabel + 1;
          auto leftPrinted = printNode(root->left, nodeLabelLeft);
          std::string leftOutput = std::get<0>(leftPrinted);
          int nodeLabelRight = std::get<1>(leftPrinted) + 1;

          auto rightPrinted = printNode(root->right, nodeLabelRight);
          std::string rightOutput = std::get<0>(rightPrinted);
          nodeLabelFinal = std::get<1>(rightPrinted);

          output = "n" + std::to_string(nodeLabel) + " = n" + std::to_string(nodeLabelLeft) + " n"
            + std::to_string(nodeLabelRight) + "\n" + leftOutput + rightOutput;
          break;
        }
      case NodeType::_star:
        {
          int nodeLabelCenter = nodeLabel + 1;
          auto centerPrinted = printNode(root->center, nodeLabelCenter);
          std::string centerOutput = std::get<0>(centerPrinted);
          nodeLabelFinal = std::get<1>(centerPrinted);

          output = "n" + std::to_string(nodeLabel) + " = n" + std::to_string(nodeLabelCenter) + " *\n" + centerOutput;
          break;
        }
      case NodeType::_leaf:
        nodeLabelFinal = nodeLabel;
        output = "n" + std::to_string(nodeLabel) + " = " + root->leafType + "\n";
        break;
      case NodeType::_lookahead:
        nodeLabelFinal = nodeLabel;
        output = "n" + std::to_string(nodeLabel) + " = lookahead\n";
        break;
    }
    return std::make_tuple(output, nodeLabelFinal);
  }

  std::string printTree(SyntaxTree tree){
    return std::get<0>(printNode(std::unique_ptr<Node>(tree.root), 1));
  }

  void test(const std::string& regex){
    std::cout << regex << "\n\n";
    SyntaxTree tree(regex);
    std::cout << printTree(tree) << "------------------------------\n\n";
  }

} //end of namespace SyntaxTreeTest

namespace NFATest {

  void test(NFA nfa){
    std::cout << "\n\n";
    std::cout << "Transitions:\n\n";
    int i = 0;
    for(auto state : nfa.transitions){
      for(auto transition : state){
        std::cout << i << " " << transition.second.value_or('e') << "-> " << transition.first << "\n";
      }
      i++;
    }
    std::cout << "\n\nAccept states: ";
    for(int state : nfa.acceptStates){
      std::cout << state << " ";
    }
    std::cout << "\n\nLookahead states: ";
    for(int state : nfa.lookaheadStates){
      std::cout << state << " ";
    }
    std::cout << "\n\nTokens:" << "\n";
    for(auto pair : nfa.tokens){
      std::cout << "\"" << pair.second << "\" at " << pair.first << "\n";
    }
    std::cout << "------------------------------\n\n";
  }

} //end of namespace NFATest

namespace DFATest {

  void test(DFA dfa){
    std::cout << "Transitions:\n\n";
    int i = 0;
    for(auto state : dfa.transitions){
      for(auto transition : state){
        std::cout << i << " " << transition.first << "-> " << transition.second << "\n";
      }
      i++;
    }
    std::cout << "\n\nAccept states: ";
    for(int state : dfa.acceptStates){
      std::cout << state << " ";
    }
    std::cout << "\n\nLookahead states: ";
    for(int state : dfa.lookaheadStates){
      std::cout << state << " ";
    }
    std::cout << "\n\nTokens:" << "\n";
    for(auto pair : dfa.tokens){
      std::cout << "\"" << pair.second << "\" at " << pair.first << "\n";
    }
    std::cout << "------------------------------\n\n";
  }

} //end of namespace DFATest

namespace FakeLexerTest {

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

  void test(DFA* dfa, const std::string& input){
    FakeLexerTest::dfa = dfa;
    initialize();

    const int bufferSize = 4096;
    std::vector<char> buffer1(bufferSize, 0);
    std::vector<char> buffer2(bufferSize, 0);
    std::stringstream fin(input);

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
    std::cout << "------------------------------\n\n";
  }

} //end of namespace FakeLexerTest

/*
namespace RegexConversionCharacterClassTest {
  void test(const std::string& regex){
    std::cout << "Input: " + regex + "\n";
    std::cout << "Output: " + characterClassProcess(regex) + "\n";
    std::cout << "------------------------------\n\n";
  }
} //end of namespace RegexConversionCharacterClassTest

namespace RegexConversionTest {
  void test(const std::string& regex){

    std::cout << "Input: " + regex + "\n";
    std::cout << "Output: " + regexNotationConversion(regex) + "\n";
    std::cout << "------------------------------\n\n";
  }
} //end of namespace RegexConversionTest
*/

int main(){
  std::cout << "SyntaxTree:\n\n";
  SyntaxTreeTest::test("(a|b)*abb");
  SyntaxTreeTest::test(R"((\(|\|)*\(\|\|)");
  SyntaxTreeTest::test(R"((a|b)\*)");
  SyntaxTreeTest::test("(a|b)*abb/a");
  SyntaxTreeTest::test("(a)");
  SyntaxTreeTest::test("(pop|blam)goesthe(weasel|squirrel)");
  SyntaxTreeTest::test(R"((a|\0)b)");

  std::vector<std::tuple<std::string, std::string> > tokenRegexList1 =
    {
      //std::make_tuple("a", "a"),
      //std::make_tuple("abb", "abb"),
      //std::make_tuple("abbb/ab", "abbb/ab"),
      //std::make_tuple("(a|b)*abb", "(a|b)*abb"),
      std::make_tuple("(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*"
          "(.(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*|\\0)"
          "(E((+|-)|\\0)(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*|\\0)", "number")
    };

  std::cout << "NFA:\n\n";
  NFA nfa(tokenRegexList1);
  NFATest::test(nfa);

  std::cout << "DFA:\n\n";
  DFA dfa(nfa);
  DFATest::test(dfa);

  std::cout << "Fake lexer:\n\n";
  std::vector<std::tuple<std::string, std::string> > tokenRegexList2 =
    {
      std::make_tuple(R"(if/\(\)then)", "if"),
      std::make_tuple("then", "then"),
      std::make_tuple(R"(\(\))", "parentheses"),
      std::make_tuple("(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_)(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_)*", "id")
    };
  std::string input = "if()then";
  NFA _nfa(tokenRegexList2);
  DFA _dfa(_nfa);
  FakeLexerTest::test(&_dfa, input);

  /*
  std::cout << "RegexNotationConversion characterClassProcess:\n\n";
  RegexConversionCharacterClassTest::test("[A-Za-z]([A-Za-z]|[0-9])*");
  RegexConversionCharacterClassTest::test("[abcde]");
  RegexConversionCharacterClassTest::test("[+-]");
  RegexConversionCharacterClassTest::test("[+\\-]");
  RegexConversionCharacterClassTest::test("\\.a+");
  RegexConversionCharacterClassTest::test("[0-9]+(\\.[0-9]+)?(E[+-]?[0-9]+)?");

  std::cout << "RegexNotationConversion regexNotationConversion:\n\n";
  RegexConversionTest::test("(a)+");
  RegexConversionTest::test("(a)?");
  RegexConversionTest::test("[a-e]{1,5}");
  RegexConversionTest::test("[0-9]+(\\.[0-9]+)?(E[+-]?[0-9]+)?");
  */
}
