TODO/Agenda for Xi Compiler Project
===================================

## Big Picture

1. Lexer (currently working on)
2. ?
3. ?
4. ?
5. ?

## Current work

The next step is to start work on the lexer generator itself, and to iron out any bugs from there. The code I suspect is
most buggy is the Lex to Kleene regex translation code I just wrote.

Progress:
--------

* Mockup
  1. Lexer (complete, but without lookahead yet)
  2. Regex to syntax tree (complete)
  3. Syntax trees and token labels to super NFA (complete)
  4. Convert super-NFA into DFA (complete)
  5. Minimize DFA states

* Real
  1. Lex to Kleene translation (mostly complete)
  2. Regex to syntax tree (complete)
  3. Syntax trees and token labels to super NFA (complete)
  4. Convert super-NFA into DFA (complete)
  5. Lexer generator
  6. DFA minimization, kind of optional
