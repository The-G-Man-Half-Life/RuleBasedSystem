# Rule-Based Language System
 
> A compiler-inspired implementation of a rule-based language with lexical analysis, recursive-descent parsing, AST construction, fixed-point interpretation, and static analysis.
 
---
 
## Authors
- Mateo Montoya Ospina
-  Juan Pablo Lopez Lidueña
 
---
 
## Table of Contents
 
- [Overview](#overview)
- [Environment](#environment)
- [Project Structure](#project-structure)
- [Building the Project](#building-the-project)
- [Running the System](#running-the-system)
- [Input Format](#input-format)
- [Output Format](#output-format)
- [Test Cases](#test-cases)
- [Language Reference](#language-reference)
- [Architecture](#architecture)
---
 
## Overview
 
This system implements a complete pipeline for a rule-based declarative language, following the same conceptual path as a compiler:
 
```
Source File ──► Lexer ──► Parser ──► AST ──► Interpreter ──► Output
                                               │
                                               └──► Static Analyzer ──► Analysis Messages
```
 
Rules take the form `if <condition> then <action>`. The interpreter evaluates all rules iteratively until no new facts are derived (fixed-point semantics). The static analyzer detects conflicts, redundancies, and potentially inactive rules.
 
---
 
## Environment
 
| Component | Version / Details |
|---|---|
| **Operating System** | Windows 11 (build and development) |
| **Language** | C++17 |
| **Compiler** | MSVC (via Visual Studio Build Tools) / MinGW-w64 GCC 13+ |
| **Build System** | CMake 3.10+ |
| **Build Tool** | Ninja |
| **Standard Library** | C++17 STL (`std::filesystem`, `std::unique_ptr`, etc.) |
 
> The project is cross-platform. It compiles and runs on Linux and macOS with GCC or Clang (C++17 support required). The build directory includes a pre-compiled Windows binary (`rule_system.exe`).
 
---
 
## Project Structure
 
```
RuleBasedSystem/
├── include/                  # Public header files
│   ├── Token.hpp             # Token type definitions
│   ├── Lexer.hpp             # Lexer interface
│   ├── AST.hpp               # Abstract Syntax Tree node types
│   ├── Parser.hpp            # Recursive-descent parser interface
│   ├── Environment.hpp       # Execution environment (facts + variables)
│   ├── Interpreter.hpp       # Fixed-point interpreter interface
│   └── StaticAnalyzer.hpp    # Static analysis module interface
├── src/                      # Implementation files
│   ├── main.cpp              # Entry point and interactive menu
│   ├── Lexer.cpp             # Tokenizer implementation
│   ├── Parser.cpp            # Parser implementation
│   ├── Environment.cpp       # Environment implementation
│   ├── Interpreter.cpp       # Interpreter implementation
│   └── StaticAnalyzer.cpp    # Static analyzer implementation
├── data/                     # Test case input files (.txt)
│   ├── case1_rules.txt
│   ├── case2_rules.txt
│   ├── case3_rules.txt
│   ├── case4_rules.txt
│   ├── case5_rules.txt
│   ├── case6_rules.txt
│   ├── case7_rules.txt
│   ├── case8_rules.txt
│   └── extraGradingCase_rules.txt
├── build/                    # CMake build output (pre-compiled binary included)
│   └── rule_system.exe       # Pre-compiled Windows x64 executable
├── CMakeLists.txt            # CMake configuration
└── README.md
```
 
---
 
## Building the Project
 
> This project uses CMake to execute the project therefore works with linux/MacOS/Windows and all systems which support CMake
### Prerequisites
 
- CMake ≥ 3.10
- A C++17-capable compiler: GCC 9+, Clang 10+, or MSVC 19.14+
- Ninja (recommended) or Make
### Steps
 
```bash
# 1. Clone or extract the project
git clone [Project]
 
# 2. Create a build directory and configure
mkdir build && cd build
 
# 3. Compile
cmake --build .
 
# 4. The executable is placed at:
#    build/rule_system      (Linux / macOS)
#    build/rule_system.exe  (Windows)
```
 
#### Alternative: using Make instead of Ninja
 
```bash
cmake -S . -B build
cmake --build build
```
 
---
 
## Running the System
 
The program provides an **interactive menu** that automatically discovers all `.txt` files in the `data/` directory.
 
### From the `build/` directory (recommended)
 
```bash
# Windows
.\rule_system.exe
 
# Linux / macOS
./rule_system
```
 
### From the project root
 
```bash
# Windows
build\rule_system.exe
 
# Linux / macOS
build/rule_system
```
 
The program will display the main menu:
 
```
===================================================
  Rule-Based Language System
===================================================
Available cases:
  [1] case1_rules.txt
  [2] case2_rules.txt
  ...
  [0] Exit
 
Select a case to run:
```
 
Select a number to run the corresponding test case. The system will print the execution output followed by any static analysis messages.
 
### Adding custom input files
 
Place any `.txt` file following the [input format](#input-format) in the `data/` directory. It will appear automatically in the menu on the next run.
 
---
 
## Input Format
 
Each input file is divided into two sections separated by a `=== STATE ===` header. The `=== RULES ===` header is optional but recommended for clarity.
 
```
=== RULES ===
rule <id>:
if <condition> then <action>
 
rule <id>:
if <condition> then <action>
 
=== STATE ===
<variable> = <integer>
<fact>
```
 
### Rules
 
Each rule must follow this exact syntax:
 
```
rule <id>:
if <condition> then <action>
```
 
- Rules are separated by one or more blank lines.
- `<id>` is an identifier: lowercase letters, digits, and underscores; must start with a letter.
- `<condition>` is one or more atomic conditions joined by `AND`.
- `<action>` is a single identifier (the fact to activate).
### Conditions
 
| Form | Meaning |
|---|---|
| `id > value` | Variable comparison (greater than) |
| `id < value` | Variable comparison (less than) |
| `id = value` | Variable comparison (equal to) |
| `id` | Fact check (true if `id` is an active fact) |
| `cond AND cond` | Conjunction of two conditions |
 
### Initial State
 
Placed after `=== STATE ===`. Each element on its own line:
 
```
temp = 35       # variable assignment
humidity = 40   # variable assignment
alert           # active fact
```
 
- **Variable assignment:** `<id> = <integer>` — binds an identifier to an integer value.
- **Active fact:** `<id>` alone on a line — marks the identifier as an initially active fact.
### Constraints
 
- Keywords `rule`, `if`, `then`, `AND` are **case-sensitive**.
- Identifiers consist of lowercase letters, digits, and underscores; must start with a letter.
- Integer values are base-10 and non-negative.
- Extra whitespace and blank lines are ignored.
---
 
## Output Format
 
### Execution Output
 
Facts activated during the fixed-point evaluation are printed in **lexicographic (alphabetical) order**, one per line.
 
```
alert
fan_on
```
 
If no facts are activated:
 
```
(no output)
```
 
### Static Analysis Messages
 
Printed after the execution output, one message per line:
 
| Situation | Format |
|---|---|
| Multiple rules activate the same fact | `Action <id> generated by r1, r2, ...` |
| Two or more rules are structurally identical | `Redundant rules: r1, r2` |
| A rule whose condition can never be satisfied | `Potentially inactive rule: <id>` |
 
---
 
## Test Cases
 
All cases are included in the `data/` directory and are selectable from the interactive menu.
 
### Case 1 — Simple comparison rule
 
```
rule r1:
if temp > 30 then alert
 
STATE: temp = 35
```
 
**Output:** `alert`
 
---
 
### Case 2 — Chained rules (fact propagation)
 
```
rule r1:
if temp > 30 then alert
 
rule r2:
if alert then fan_on
 
STATE: temp = 35
```
 
**Output:**
```
alert
fan_on
```
 
---
 
### Case 3 — Condition not satisfied
 
```
rule r1:
if temp > 30 then alert
 
STATE: temp = 20
```
 
**Output:** `(no output)`
 
---
 
### Case 4 — Conjunction (AND)
 
```
rule r1:
if temp > 30 AND humidity < 50 then alert
 
STATE: temp = 35, humidity = 40
```
 
**Output:** `alert`
 
---
 
### Case 5 — AND condition partially fails
 
```
rule r1:
if temp > 30 AND humidity < 50 then alert
 
STATE: temp = 35, humidity = 60
```
 
**Output:** `(no output)`
 
---
 
### Case 6 — Conflict detection
 
```
rule r1:
if temp > 30 then fan_on
 
rule r2:
if humidity < 50 then fan_on
 
STATE: temp = 35, humidity = 40
```
 
**Output:**
```
fan_on
```
**Analysis:**
```
Action fan_on generated by r1, r2
```
 
---
 
### Case 7 — Redundancy detection
 
```
rule r1:
if temp > 30 then alert
 
rule r2:
if temp > 30 then alert
```
 
**Analysis:**
```
Redundant rules: r1, r2
```
 
---
 
### Case 8 — Potentially inactive rule
 
```
rule r1:
if temp > 30 then alert
 
rule r2:
if alert then fan_on
 
rule r3:
if humidity < 20 then cooling
 
STATE: temp = 25, humidity = 50
```
 
**Analysis:**
```
Potentially inactive rule: r3
```
 
---
 
### Extra Grading Case — Long fact chain
 
```
rule r1: if a then b
rule r2: if b then c
rule r3: if c then d
rule r4: if d then e
 
STATE: a
```
 
**Output:**
```
b
c
d
e
```
 
---
 
## Language Reference
 
### Grammar (LL(1) — left-recursion eliminated)
 
```
Program     → RuleList
RuleList    → Rule RuleList | ε
Rule        → rule id : if Cond then Action
Cond        → Atom CondRest
CondRest    → AND Atom CondRest | ε
Atom        → id AtomRest
AtomRest    → RelOp value | ε
RelOp       → > | < | =
Action      → id
```
 
> The grammar in the project specification is not LL(1) as written due to left recursion in `Cond`. The implementation applies left-recursion elimination and left-factoring to produce a proper LL(1) grammar, implemented as a hand-written recursive-descent parser.
 
### Token Types
 
| Token | Description |
|---|---|
| `rule` | Rule declaration keyword |
| `if` | Condition keyword |
| `then` | Action keyword |
| `AND` | Logical conjunction |
| `>` `<`  | Relational operators |
| `=` | variable operator |
| `:` | Rule separator |
| `id` | Identifier (lowercase, may include digits/underscores) |
| `INTEGER` | Non-negative base-10 integer literal |
 
---
 
## Architecture
 
The system is organized into five independent modules:
 
| Module | Files | Responsibility |
|---|---|---|
| **Lexer** | `Lexer.hpp` / `Lexer.cpp` | Converts raw source text into a flat token stream |
| **Parser** | `Parser.hpp` / `Parser.cpp` | Consumes tokens and builds a typed AST using recursive descent |
| **AST** | `AST.hpp` | Defines node types: `ProgramNode`, `RuleNode`, `ConditionNode` subtypes, `ActionNode` |
| **Interpreter** | `Interpreter.hpp` / `Interpreter.cpp` + `Environment.hpp` / `Environment.cpp` | Evaluates the AST over an `Environment` (variable map + active fact set) using fixed-point iteration |
| **Static Analyzer** | `StaticAnalyzer.hpp` / `StaticAnalyzer.cpp` | Inspects the AST for conflicts, redundant rules, and potentially inactive rules |
 
### AST Node Hierarchy
 
```
ASTNode
├── ProgramNode          — list of RuleNode
├── RuleNode             — id + ConditionNode + ActionNode
├── ActionNode           — fact identifier
└── ConditionNode
    ├── FactConditionNode    — single identifier (fact check)
    ├── ComparisonNode       — id, RelOp, integer value
    └── AndConditionNode     — left ConditionNode + right ConditionNode
```
 
### Execution Model (Fixed Point)
 
```
1. Initialize Environment with variable assignments and active facts from state
2. repeat:
     applicable ← { rule ∈ rules | evaluate(rule.condition, env) = true }
     newFacts   ← { rule.action.factId | rule ∈ applicable }
     env.facts  ← env.facts ∪ newFacts
   until env.facts did not change
3. Print sorted active facts
4. Run static analysis and print messages
```
 
Evaluation is deterministic and order-independent: all applicable rules in a given iteration are collected before any facts are added, ensuring the fixed point is reached regardless of rule ordering.
