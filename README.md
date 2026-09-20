Fela is name for the programming language I am trying to make.
By doing this I hope to learn standard compiler architecture etc.
This repo is meant to document that journey.

Milestones:
Write initial language spec using EBNF - done 
write micro subset of C using ENBF -done
Research and write lexer skeleton - done
write lexer - done


boring unit tests for lexer - done  by AI (mostly)
Research and write parser skeleton. - done


So clang does it with parser and sema. 
Parser - checks grammard
AST building is delegated to sema
Current goals: 
Code parser that can check grammar.