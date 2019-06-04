TODO/Agenda for Xi Compiler Project
===================================

## Big Picture

1. Lexer (currently working on)
2. ?
3. ?
4. ?
5. ?

## Current work

I'm no longer going to work on an actual lexer generator, instead on a demo lexer which creates the automata from a
definition file and then lexes a source file on the fly.

Progress:
--------

* Mockup
  1. Lexer (complete, but without lookahead yet)
  2. Regex to syntax tree (complete)
  3. Syntax trees and token labels to super NFA (complete)
  4. Convert super-NFA into DFA (complete)
  5. Minimize DFA states

* Real
  1. Lex to Kleene translation (complete)
  2. Regex to syntax tree (complete)
  3. Syntax trees and token labels to super NFA (complete)
  4. Convert super-NFA into DFA (complete)
  5. Lexer generator
  6. DFA minimization, kind of optional
