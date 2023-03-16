# Concurrent Trie

A [*trie*](https://en.wikipedia.org/wiki/Trie) or *prefix tree* is a data structure that allows for efficient storage and retrieval of strings.

This repository contains a concurrent trie implementation in C++, where multithreading is used to improve performance as compared to a single-threaded implementation.
- Each node in the trie contains a `mutex` to ensure that only one thread can add/remove children nodes at a time.
- Provided "bulk operation" methods take in a `vector` of strings to insert/search/remove.
- [OpenMP](https://www.openmp.org/) is used to parallelize the bulk operations.
- An *async* insert/remove method is also provided, which returns instantly and performs the operation in the background.
- Readers and writers are ensured fairness by implementing a fair solution to the Unisex Bathroom Problem, a variant
of the [Readers-Writers Problem](https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem) where multiple writers
are allowed, and readers and writers are equally prioritized.

## Provided Methods
---

### Insertion
`void insert(std::string word)` - Inserts a single string into the trie.

`void insert(std::vector<std::string>* words)` - Inserts multiple strings into the trie.

`void insertAsync(std::vector<std::string>* words)` - Inserts multiple strings into the trie asynchronously.


### Deletion
`void remove(std::string word)` - Removes a single string from the trie.

`void remove(std::vector<std::string>* words)` - Removes multiple strings from the trie.

`void removeAsync(std::vector<std::string>* words)` - Removes multiple strings from the trie asynchronously.

### Search
`bool contains(std::string word)` - Returns `true` if the trie contains the given string, `false` otherwise.

`std::vector<bool> contains(std::vector<std::string>* words)` - Returns a `vector` of booleans, where the `i`th element is `true` if the trie contains the `i`th string in the given `vector`, `false` otherwise.

### Retrieval
`std::vector<std::string> getStringsWithPrefix(std::string prefix)` - Returns all strings in the trie that start with the given prefix, in sorted order.

`std::vector<std::string> getAllStringsSorted()` - Returns all strings in the trie, in sorted order.

### Others
`int size()` - Returns the number of strings in the trie.

`void setNumThreads(int numThreads)` - Sets the number of threads to use for OpenMP parallelization.

`void setMaxThreads()` - Sets the number of threads to use for OpenMP parallelization to the maximum number of threads that OpenMP can use.

`std::shared_ptr<ConcurrentTrie> createSharedPtr()` - Returns a `shared_ptr` to the trie.

