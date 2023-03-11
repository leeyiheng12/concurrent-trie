#pragma once

#include <algorithm>  // std::find
#include <memory>  // std::shared_ptr, std::enable_shared_from_this
#include <mutex>
#include <omp.h>
#include <pthread.h>
#include <stack>
#include <stdexcept>  // std::invalid_argument
#include <stdio.h>
#include <string>
#include <thread>
#include <utility>  // std::pair
#include <vector>

#include "utils/readers_writers.h"


// #define NODE_SIZE 95  // Allow for 95 printable ASCII characters - from space (32) to tilde (126)

#define SMALLEST_CHAR 0
#define LARGEST_CHAR 127
#define NODE_SIZE (LARGEST_CHAR - SMALLEST_CHAR + 1)

class ConcurrentTrie;

struct InsertAsyncArgs {
    std::vector<std::string>* words;
    int size;
    ConcurrentTrie* trie;
};


class ConcurrentNode {

    friend class ConcurrentTrie;

    private:
        std::shared_ptr<ConcurrentNode> children_[NODE_SIZE];
        std::shared_ptr<ConcurrentNode> parent_;
        bool isEnd_;
        int numChildren_;
        int selfIndex_;
        omp_lock_t nodeLock_;

    public:
        ConcurrentNode();
};

class ConcurrentTrie : public std::enable_shared_from_this<ConcurrentTrie> {

    private:
        std::shared_ptr<ConcurrentNode> root_;

        int size_;  // For keeping track of the number of strings in the trie
        omp_lock_t sizeLock_;  // For updating size_ in a thread-safe manner
        
        // For inserting and removing
        std::shared_ptr<FairReadersWriters> rwLock_;

        // For async inserting and removing
        std::shared_ptr<FairReadersWriters> asyncWriteLock_;

        // Methods to help with basic operations
        int getIndexOfChar(char c);
        char getCharForIndex(int idx);
        void possiblyDeleteNode(std::shared_ptr<ConcurrentNode> node);
        
        // To support async insert and remove - called in another thread by insertAsync and removeAsync
        static void insertAsyncHelper(std::shared_ptr<ConcurrentTrie> trie, std::vector<std::string>* words);
        static void removeAsyncHelper(std::shared_ptr<ConcurrentTrie> trie, std::vector<std::string>* words);

        // Helper methods for getting all strings in a sorted order given a particular node
        std::vector<std::string> getAllStringsSortedHelper(std::shared_ptr<ConcurrentNode> node, std::string prefix);

    public:
        ConcurrentTrie();
        std::shared_ptr<ConcurrentTrie> createSharedPtr();
        
        void setNumThreads(int numThreads);
        void setMaxThreads();

        // Basic operations
        void insert(std::string word);
        void insert(std::vector<std::string>* words);
        void insertAsync(std::vector<std::string>* words);

        bool contains(std::string word);
        std::vector<bool> contains(std::vector<std::string>* words);

        void remove(std::string word);
        void remove(std::vector<std::string>* words);
        void removeAsync(std::vector<std::string>* words);

        int size();

        // Advanced operations
        std::vector<std::string> getStringsWithPrefix(std::string prefix);
        std::vector<std::string> getAllStringsSorted();

};
