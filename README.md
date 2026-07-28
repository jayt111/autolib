# Autolib: A Modern C++ library for omega automata

## About

I wrote this [Omega automata](<https://en.wikipedia.org/wiki/%CE%A9-automaton>) library for my final masters project.

This library is meant as a starting off point for anyone wanting to use omega automata without having to delve into the details.

It has support for:

- Constructing deterministic and non-deterministic:
  - Büchi automata
  - Rabin automata
  - Streett automata
  - Parity automata
- Emptiness checking for:
  - Rabin
  - Parity
  - Büchi
- Rabin to parity translations
- Büchi automaton product
- Synchronous product between a Kripke structure and a guarded Büchi automaton

## Building

This project uses CMake as its build system. It should work on any operating system but build instructions may vary.

### Linux

To build the library run the following in the top level directory:

```
cd library/
mkdir build
cmake ..
cmake --build .
```

This will build:

- The library
- tests
- example programs

## Running Tests

After building run:
`ctest`
to run the tests

## Examples

Example programs are located in the `examples/` directory
To run the example LTL model checker:
`./examples/ltl_mc/ltl_mc`

## Visualising DOT files

To render DOT files either:

`dot -Tpng <dot_file> -o <file_name>.png`

or use an online tool to render e.g. [GraphvizOnline](https://dreampuf.github.io/GraphvizOnline/)
