# Fela

Fela is name for the programming language I am trying to make.
By doing this I hope to learn standard compiler architecture etc.
This repo is meant to document that journey.

## Overview

**Fela** is a micro C-subset programming language and an end-to-end compiler built from scratch in C++20 using LLVM:
**Lexer -> Parser -> AST -> Semantic Checker -> LLVM IR CodeGen -> TargetMachine Object File (.o) -> Linker Executable**.

It follows standard C syntax: curly braces `{}` for scopes, semicolons `;` terminating statements, C-style function declarations/definitions, and familiar keywords (`if`, `else`, `while`, `return`). If it looks like valid C (without pointers, structs, or `#include`), it compiles on Fela.

## Usage

```bash
fela [options] <source_file>
```

### Options
- `<source_file>`: Input source file path (or `-p, --path <file>`)
- `-o, --out <name>`: Output base name (default: derived from input file name)
- `-c`: Compile to object file only (`.o`), do not link executable
- `-S, --emit-llvm`: Emit LLVM IR file only (`.ll`)
- `-d, --ast-dump`: Print AST after parsing
- `-h, --help`: Display help message

### Built-in Functions
- `print(x)`: Prints an `int` or `bool` followed by a newline to stdout.

## Milestones

- [x] Write initial language spec using EBNF - done
- [x] write micro subset of C using ENBF -done
- [x] Research and write lexer skeleton - done
- [x] write lexer - done
- [x] boring unit tests for lexer - done by AI
- [x] Research and write parser skeleton. - done
- [x] write parser - done
- [x] boring unit tests for parser - done by AI
- [x] Research how clang implements sema and AST.-done
- [x] Research how to implement symbol table and ast nodes - done
- [x] define some initial ast tree data structure - done
- [x] writer parser v2 - done (first time I only wrote skeleton and defined descent order with basicaly empty stubs oops!)
- [x] write semantic checker stubs - done
- [x] I have just realised at some point I got confused and started doing semantic checks in the parser, where I meant it to be 2 pass compiler OOPS!
- [x] Modify parser to produce just the AST tree -done!
- [x] 1.Walk ast tree and just dump it into stream somewhere (for debugging) -done!
- [x] Modify semantic checker to walk AST tree. -done
- [x] Front end officially done yay
- [x] learn llvm ir basics done
- [x] llvm IR representation (CodeGen) done!
- [x] Support global and local variables in IR - done!
- [x] Backend object file emission (`.o`) via LLVM `TargetMachine` - done!
- [x] System runtime linking (`cc`) into executable binary - done!
- [x] Built-in `print(x)` function for `int` and `bool` - done!
- [x] Standard compiler CLI (positional source argument, `-o`, `-c`, `-S`, `-h`) - done!

## Current Goals

- Add automated end-to-end test suite compiling and running sample Fela programs
- Fix remaining edge cases in Semantic Checker (return type verification, binary op checks)
- Hook up LLVM optimization pipeline (`mem2reg` / `-O2`)

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
