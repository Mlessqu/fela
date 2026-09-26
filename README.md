Fela is name for the programming language I am trying to make.
By doing this I hope to learn standard compiler architecture etc.
This repo is meant to document that journey.

Milestones:
Write initial language spec using EBNF - done 
write micro subset of C using ENBF -done
Research and write lexer skeleton - done
write lexer - done
boring unit tests for lexer - done  by AI 
Research and write parser skeleton. - done
write parser - done
boring unit tests for parser - done  by AI
Research how clang implements sema and AST.-done
Research how to implement symbol table and ast nodes - done
define some initial ast tree data structure - done

writer parser v2 - done (first time I only wrote skeleton and defined descent order with basicaly empty stubs oops!)
write semantic checker stubs - done
I have just realised at some point I got confused and started doing semantic checks in the parser, where I meant it to be 2 pass compiler OOPS!


Current goals: 
Modify parser to produce just the AST tree.
Modify semantic checker to walk that tree.
