# Fela

Fela is name for the programming language I am trying to make.
By doing this I hope to learn standard compiler architecture etc.
This repo is meant to document that journey.

## Overview

**Fela** is a micro C-subset language and 2-pass compiler frontend (Lexer -> Parser -> AST -> Semantic Checker).

It follows standard C syntax: curly braces `{}` for scopes, semicolons `;` terminating statements, C-style function declarations/definitions, and familiar keywords (`if`, `else`, `while`, `return`). If it looks like valid C (without pointers, structs, or `#include`), it parses as Fela.

## Usage

```bash
fela -p <source_file> [options]
fela --path <source_file> [options]
```

### Options
- `-p, --path <file>`: Source file path (required)
- `--ast-dump`: Print AST after parsing

## Milestones

- [x] Write initial language spec using EBNF - done
- [x] write micro subset of C using ENBF -done
- [x] Research and write lexer skeleton - done
- [x] write lexer - done
- [x] boring unit tests for lexer - done  by AI
- [x] Research and write parser skeleton. - done
- [x] write parser - done
- [x] boring unit tests for parser - done  by AI
- [x] Research how clang implements sema and AST.-done
- [x] Research how to implement symbol table and ast nodes - done
- [x] define some initial ast tree data structure - done
- [x] writer parser v2 - done (first time I only wrote skeleton and defined descent order with basicaly empty stubs oops!)
- [x] write semantic checker stubs - done
- [x] I have just realised at some point I got confused and started doing semantic checks in the parser, where I meant it to be 2 pass compiler OOPS!
- [x] Modify parser to produce just the AST tree -done!
- [x] 1.Walk ast tree and just dump it into stream somewhere (for debugging) -done!
- [x] Modify semantic checker to walk AST tree. -done
- [x] Front end  officialy done yay

## Current Goals

- debugging front end :D

## Language Constraints

- **Single-File Compilation (No Linker):**
  - No linker or multi-file linking. All code (functions, global declarations, and `main()`) must be in a single source file.
- **Types & Literals:**
  - `int`, `bool`.
  - `void` only for function return type (variables cannot be `void`).
  - Literals: integer numbers, `true`, `false`.
- **Identifiers:**
  - Must start with `[a-zA-Z_]`, followed by letters, digits, or `_`.
- **Entry point:**
  - Must define `int main()` with no parameters.
- **Functions:**
  - Global scope only (no nested functions).
  - Forward declarations supported (`type name(params);`).
  - Unique parameter names per function.
  - Return statements: `return;` for `void`, `return <expr>;` matching return type.
- **Variables & Scopes:**
  - Global variables allowed at file scope.
  - Optional initialization (`int x = 10;` or `int x;`).
  - Lexical scoping with shadowing in inner blocks.
- **Instructions:**
  - Assignments (`x = expr;`), declarations, blocks (`{ ... }`), and expression statements (`foo();`).
- **Control Flow:**
  - `if (...)` and `while (...)` conditions must be `bool`.
- **Operators:**
  - Arithmetic: `+`, `-`, `*`, `/` ( L and R expression must be 'int', result is 'int').
  - Relational: `<`, `>`,  (`int` -> `bool`).
  - Equal not equal `==`, `!=` ('int' or 'bool' -> 'bool)
  - Logical: `&&`, `||`, `!` (`bool` -> 'bool' only).
- **Comments:**
  - Line (`// ...`) and block (`/* ... */`).
