#include <algorithm>  // std::find
#include <memory>  // std::shared_ptr
#include <mutex>
#include <omp.h>
#include <pthread.h>
#include <stack>
#include <stdexcept>  // std::invalid_argument
#include <stdio.h>
#include <string>
#include <utility>  // std::pair
#include <vector>


#define NODE_SIZE 95  // Allow for 95 printable ASCII characters - from space (32) to tilde (126)


class ConcurrentNode {

    friend class ConcurrentTrie;

    private:
        std::shared_ptr<ConcurrentNode> children_[NODE_SIZE];
        std::shared_ptr<ConcurrentNode> parent_;
        bool isEnd_;
        int numChildren_;
        int selfIndex_;
        std::mutex nodeLock_;

    public:
        ConcurrentNode();
};

class ConcurrentTrie {

    private:
        std::shared_ptr<ConcurrentNode> root_;
        int size_;
        int getIndexOfChar(char c);
        char getCharForIndex(int idx);
        void possiblyDeleteNode(std::shared_ptr<ConcurrentNode> node);

    public:
        ConcurrentTrie();

        // Basic operations
        bool insert(std::string word);
        std::vector<bool> insert(std::vector<std::string> words);
        bool search(std::string word);
        std::vector<bool> search(std::vector<std::string> words);
        bool erase(std::string word);
        std::vector<bool> erase(std::vector<std::string> words);
        int size();

        // Advanced operations
        std::vector<std::string> getWordsWithPrefix(std::string prefix);
        std::vector<std::string> getAllWordsSorted();

};
