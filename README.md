# Mini Search Engine

A small C++ command-line search engine that searches for phrases among the text files in a directory.

## Features

### Phrase Search
Searches for the exact phrase in the files; if the phrase is not found, it shows the results of all the files containing the words in the phrase.

### TF-IDF Based Results
Results are ranked and displayed according to TF-IDF (Term Frequency–Inverse Document Frequency) scoring.

### Inverted Index
Words are mapped to the files they appear in, enabling fast lookups.

### On-Disk Caching
Instead of recomputing the index on every run, the index is cached to disk after the first run. If any file in the directory is added, removed, or modified, the index is automatically recomputed. This significantly speeds up repeated searches when the underlying files haven't changed.

## Project Structure

| File | Purpose |
| --- | --- |
| main_search.cpp | Interactive search program |
| main.cpp | Runs predefined queries for testing performance |
| invertedIndex.cpp | Implementation of indexing and searching logic |
| invertedIndex.hpp | Class declarations for inverted index |
| readFiles.cpp | Reading and processing text files |
| readFiles.hpp | Class declarations for file handling |
| queries.hpp | Contains test queries for benchmarking |

There are two entry points, serving different purposes:

`main_search.cpp`: Interactive search — prompts you to type a query and returns ranked results. 
`main.cpp` : Benchmark tool — runs a fixed set of queries (defined in `queries.hpp`) to measure the algorithm's performance. 

Both share the same core logic, built from `invertedIndex.cpp` and `readFiles.cpp`.

## Prerequisites

- A C++17-compatible compiler (e.g. `g++`)

## How to Run

### 1. Interactive Search

Compile:
```bash
g++ -std=c++17 main_search.cpp invertedIndex.cpp readFiles.cpp -o search
```

Run:
```bash
./search
```

You'll be prompted to:
1. Type your search query.

The engine will return matching files ranked by TF-IDF score. If the exact phrase isn't found, it falls back to showing files containing the individual words in the query.

### 2. Performance Benchmark

Compile:
```bash
g++ -std=c++17 main.cpp invertedIndex.cpp readFiles.cpp -o benchmark
```

Run:
```bash
./benchmark
```

This runs through a predefined list of queries (edit `queries.hpp` to add or change them) and reports performance/timing results — useful for testing indexing and caching speed across repeated runs.

## Example

```bash
$ ./search
Enter search query: machine learning

Results (ranked by TF-IDF):
1. article3.txt   (score: 0.842)
2. notes.txt      (score: 0.615)
3. article1.txt   (score: 0.203)
```


## Notes

- On first run for a given folder, the engine builds and caches the inverted index to disk.
- On subsequent runs, if no files in the folder have changed, the cached index is reused, making searches much faster.
- If files are added, removed, or modified, the index is rebuilt automatically.
