#include <algorithm>  // std::find
#include <memory>  // std::shared_ptr
#include <stack>
#include <stdexcept>  // std::invalid_argument
#include <stdio.h>
#include <string>
#include <utility>  // std::pair
#include <vector>


// #define NODE_SIZE 95  // Allow for 95 printable ASCII characters - from space (32) to tilde (126)

#define SMALLEST_CHAR 0
#define LARGEST_CHAR 127
#define NODE_SIZE (LARGEST_CHAR - SMALLEST_CHAR + 1)


class SequentialNode {

    friend class SequentialTrie;

    private:
        std::shared_ptr<SequentialNode> children_[NODE_SIZE];
        std::shared_ptr<SequentialNode> parent_;
        bool isEnd_;
        int numChildren_;
        int selfIndex_;

    public:
        SequentialNode();
};

class SequentialTrie {

    private:
        std::shared_ptr<SequentialNode> root_;
        int size_;
        int getIndexOfChar(char c);
        char getCharForIndex(int idx);
        void possiblyDeleteNode(std::shared_ptr<SequentialNode> node);

    public:
        SequentialTrie();

        // Basic operations
        void insert(std::string word);
        void insert(std::vector<std::string>* words);

        bool contains(std::string word);
        std::vector<bool> contains(std::vector<std::string>* words);

        void remove(std::string word);
        void remove(std::vector<std::string>* words);
        
        int size();

        // Advanced operations
        std::vector<std::string> getStringsWithPrefix(std::string prefix);
        std::vector<std::string> getAllStringsSorted();
        
};
