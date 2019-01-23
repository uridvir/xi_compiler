TODO/Agenda for Xi Compiler Project
===================================

## Big Picture

1. Lexer (currently working on)
2. ?
3. ?
4. ?
5. ?

## Current work

Currently, I am writing a basic lexical analyzer generator, and an accompanying lexer.

Progress:
--------

* Mockup
  1. Lexer (complete)
  2. Regex to syntax tree (complete)
  3. Syntax tree and token labels to NFA
  4. Merge NFAs into super-NFA using e-transitions
  5. Convert super-NFA into DFA
  6. Minimize DFA states

* Real
  1. Get regex and token labels from file
  2. Regex to syntax tree
  3. Syntax tree and token labels to NFA
  4. Merge NFAs into super-NFA using e-transitions
  5. Convert super-NFA into DFA
  6. Minimize DFA states
  7. Generate lexer

Notes:
------

The mockup lexer can accept all characters, but it _cannot_ interpret the lookahead operator. The rest of the mockup
only accepts {a, b}. When fetching the token labels and regex from the definition file, currently the mockup can only
understand basic Kleene notation for the regex. A change that should be made in the real version of the syntax tree
generation code is to add convenient shortcuts like, for example, being able to type [0-9] rather than
0|1|2|3|4|5|6|7|8|9. This is not important for the mockup, but will make actually writing a language definition that
the code will understand much easier. The lexer itself will be a C++ source file created by the lexical analyzer
generator.
