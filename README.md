# Generic Markov Chain Engine in C

A reusable Markov Chain framework implemented in C using `void*`, function pointers, linked lists, dynamic arrays, and manual memory management.

The project includes two applications built on top of the same generic Markov Chain engine:

1. A text/tweet generator that learns word transitions from a text corpus.
2. A Snakes & Ladders random-walk simulator.

## Features

- Generic Markov Chain implementation using `void*`
- Function pointers for printing, comparison, copying, freeing, and terminal-state detection
- Frequency-based probabilistic transitions
- Linked-list-based database of states
- Dynamic frequency lists using `malloc` and `realloc`
- Manual memory management with full cleanup using `free`
- Two different applications using the same framework

## Project Structure

```text
generic-markov-chain-c/
├── markov_chain.c
├── markov_chain.h
├── linked_list.c
├── linked_list.h
├── tweets_generator.c
├── snakes_and_ladders.c
├── Makefile
├── README.md
└── .gitignore
```

## Technologies and Concepts

- Generic programming with `void*`
- Function pointers
- Structs
- Linked lists
- Dynamic arrays
- File parsing
- Randomized algorithms
- Manual memory management

## Build

Compile both applications:

```bash
make
```

This creates two executables:

```text
tweets_generator
snakes_and_ladders
```

Remove compiled files:

```bash
make clean
```

## Run the Text Generator

```bash
./tweets_generator <seed> <number_of_tweets> <corpus_file_path> [words_to_read]
```

Arguments:

```text
seed              Random seed
number_of_tweets  Number of tweets/sentences to generate
corpus_file_path  Path to the input text corpus
words_to_read     Optional number of words to read from the corpus
```

Example:

```bash
./tweets_generator 42 5 corpus.txt 1000
```

Example output:

```text
Tweet 1: stay hungry stay foolish.
Tweet 2: this is a generated sentence.
Tweet 3: the model creates random text from learned transitions.
```

## Run the Snakes & Ladders Simulator

```bash
./snakes_and_ladders <seed> <number_of_paths>
```

Arguments:

```text
seed             Random seed
number_of_paths  Number of random walks to generate
```

Example:

```bash
./snakes_and_ladders 42 3
```

Example output:

```text
Random Walk 1: [1] -> [4] -> [8] -ladder to-> [30] -> [35] -snake to-> [11] -> [17] -> [23] -ladder to-> [76] -> [82] -> [88] -> [94] -> [100]
Random Walk 2: [1] -> [6] -> [10] -> [13] -snake to-> [4] -> [9] -> [15] -ladder to-> [47] -> [52] -> [58] -> [64] -> [70] -> [76] -> [81] -snake to-> [43] -> [49] -> [55] -> [61] -snake to-> [14] -> [20] -ladder to-> [39] -> [44] -> [50] -> [56] -> [62] -> [68] -> [74] -> [80] -> [86] -> [92] -> [98] -> [100]
```

## Main Components

### Markov Chain Engine

Implemented in:

```text
markov_chain.c
markov_chain.h
```

The engine stores generic data as `void*` and receives type-specific behavior through function pointers:

- `print_func`
- `comp_func`
- `copy_func`
- `free_data`
- `is_last`

This allows the same Markov Chain logic to work with both strings and board cells.

### Text Generator

Implemented in:

```text
tweets_generator.c
```

This application reads words from a text file, builds a Markov Chain based on word transition frequencies, and generates random text sequences.

### Snakes & Ladders Simulator

Implemented in:

```text
snakes_and_ladders.c
```

This application models a Snakes & Ladders board as a Markov Chain and generates random walks from the first cell to the final cell.

## Notes

This project was implemented as part of a C/C++ programming workshop.

The main focus of the project is low-level C programming, including generic data structures, function pointers, pointer-based ownership, dynamic memory allocation, and deterministic cleanup.