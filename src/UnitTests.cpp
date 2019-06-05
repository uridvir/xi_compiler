#include "DFA.h"
#include "LexerReader.h"
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

namespace CharacterClassTest {

  void test(const std::string& regex, std::map<std::string, std::string> definitions){
    std::cout << regex << "\n----->\n" << LexerReader::characterClassProcess(regex, definitions) << "\n";
    std::cout << "------------------------------\n\n";
  }

} //end of namespace CharacterClassTest

namespace RegexNotationTest {

  void test(const std::string& regex, std::map<std::string, std::string> definitions){
    std::cout << regex << "\n----->" << LexerReader::regexNotationConversion(regex, definitions);
    std::cout << "------------------------------\n\n";
  }

} //end of namespace RegexNotationTest

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

  std::cout << "characterClassProcess:\n\n";
  CharacterClassTest::test(R"(\/\/[^\n]*\n)", {});
}
