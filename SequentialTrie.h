#include <algorithm>  // std::find
#include <memory>  // std::shared_ptr
#include <stack>
#include <stdexcept>  // std::invalid_argument
#include <stdio.h>
#include <string>
#include <utility>  // std::pair
#include <vector>

#define IS_TRUE(x) { if (!(x)) printf("%s failed on line %d\n", __FUNCTION__, __LINE__); }
#define IS_FALSE(x) { if ((x)) printf("%s failed on line %d\n", __FUNCTION__, __LINE__); }
#define NODE_SIZE 95  // Allow for 95 printable ASCII characters - from space (32) to tilde (126)


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
