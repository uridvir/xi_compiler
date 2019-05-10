TODO/Agenda for Xi Compiler Project
===================================

## Big Picture

1. Lexer (currently working on)
2. ?
3. ?
4. ?
5. ?

## Current work

Next step is to rewrite the lexer to be able to handle the lookahead operator. After that I need to write code that can
translate between Lex's regex syntax and Kleene notation.

Progress:
--------

* Mockup
  1. Lexer (complete, but without lookahead yet)
  2. Regex to syntax tree (complete)
  3. Syntax trees and token labels to super NFA (complete)
  4. Convert super-NFA into DFA (complete)
  5. Minimize DFA states

* Real
  1. Get regex and token labels from file
  2. Regex to syntax tree (complete)
  3. Syntax trees and token labels to super NFA (complete)
  4. Convert super-NFA into DFA (complete)
  5. Minimize DFA states
  6. Generate lexer

Notes:
------

The mockup lexer can accept all characters, but it _cannot_ interpret the lookahead operator. The rest of the mockup
only accepts {a, b}. When fetching the token labels and regex from the definition file, currently the mockup can only
understand basic Kleene notation for the regex.  A change that should be made in the real version of the syntax tree
generation code is to add convenient shortcuts like, for example, being able to type [0-9] rather than
0|1|2|3|4|5|6|7|8|9. This is not important for the mockup, but will make actually writing a language definition that the
code will understand much easier. The lexer itself will be a C++ source file created by the lexical analyzer generator.
