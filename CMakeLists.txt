# Stablishing the minimal Cmake Version required
cmake_minimum_required(VERSION 3.10)

# Giving name to the project
project(RuleBasedSystem LANGUAGES CXX)

# Requeesting C++17 for using certain features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tell the compilar where to find the header files (.hpp)
include_directories(include)

# Group source files(.cpp) in a veriable
set(SOURCES
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/Interpreter.cpp
    src/StaticAnalyzer.cpp
    src/Environment.cpp
)

# Create the executable known as "rule_system" using the source files
add_executable(rule_system ${SOURCES})

# Messages in case something goes wrong
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(rule_system PRIVATE -Wall -Wextra -Wpedantic)
endif()