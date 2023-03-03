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


class Node {

    friend class Trie;

    private:
        std::shared_ptr<Node> children_[NODE_SIZE];
        std::shared_ptr<Node> parent_;
        bool isEnd_;
        int numChildren_;
        int selfIndex_;

    public:
        Node();
};

class Trie {

    private:
        std::shared_ptr<Node> root_;
        int size_;
        int getIndexOfChar(char c);
        char getCharForIndex(int idx);
        void possiblyDeleteNode(std::shared_ptr<Node> node);

    public:
        Trie();
        bool insert(std::string word);
        bool insert(std::vector<std::string> words);
        bool search(std::string word);
        bool erase(std::string word);
        int size();

        std::vector<std::string> getWordsWithPrefix(std::string prefix);
        std::vector<std::string> getWordsSorted();
        std::vector<std::string> getWordsLexicographicallyGreaterThan(std::string leftBound);
        std::vector<std::string> getWordsLexicographicallySmallerThan(std::string rightBound);
        std::vector<std::string> getWordsLexicographicallyBetween(std::string leftBound, std::string rightBound);
};
