#include "ConcurrentTrie.h"

Node::Node() {
    for (int i = 0; i < NODE_SIZE; i++) {
        children_[i] = NULL;
    }
    parent_ = NULL;
    isEnd_ = false;
    numChildren_ = 0;
}

Trie::Trie() {
    root_ = std::make_shared<Node>();
    size_ = 0;
}

int Trie::getIndexOfChar(char c) {
    int idx = c - ' ';
    if (idx < 0 || idx >= NODE_SIZE) {
        throw std::invalid_argument("Invalid character");
    }
    return idx;
}
 
// Inserts a string into the trie.
// Returns true if the string is already in the trie.
bool Trie::insert(std::string word) {

    std::shared_ptr<Node> cur = root_;
    int index;
 
    for (int i = 0; i < word.length(); i++) {

        index = getIndexOfChar(word[i]);

        if (!cur->children_[index]) {

            // Create new node and set other attributes
            std::shared_ptr<Node> newNode = std::make_shared<Node>();
            newNode->parent_ = cur;
            newNode->selfIndex_ = index;
            cur->children_[index] = newNode;

            // Increment number of children of cur
            cur->numChildren_++;
        }
        cur = cur->children_[index];

    }
 
    if (cur->isEnd_) {
        return true;
    }

    cur->isEnd_ = true;
    size_++;
    return false;
}
 
// Returns true if word is present in the trie.
bool Trie::search(std::string word) {

    std::shared_ptr<Node> cur = root_;
    int index;
 
    for (int i = 0; i < word.length(); i++) {
        index = getIndexOfChar(word[i]);
        if (!cur->children_[index]) {
            return false;  // If word was in the trie, this would not have been NULL.
        }
        cur = cur->children_[index];
    }
 
    return cur->isEnd_;
}

// Returns true if word was in the trie and was successfully removed.
bool Trie::erase(std::string word) {

    // There are a few possibilities:
    // 1. The word is not in the trie. Return false in this scenario.
    // 2. The word is present. Unmark the end of the word (set as false).
    // If the word is not a prefix of another word, we delete the node.
    // If there exists another word that is a prefix of this word, we do not delete the word
    // otherwise we keep going up the trie, deleting nodes until we reach a node that has more than one child.

    std::shared_ptr<Node> cur = root_;
    int index;
 
    for (int i = 0; i < word.length(); i++) {
        index = getIndexOfChar(word[i]);
        if (!cur->children_[index]) {
            return false;  // Scenario 1
        }
        cur = cur->children_[index];
    }
 
    if (!cur->isEnd_) {
        return false;  // Scenario 1
    }

    cur->isEnd_ = false;
    size_--;
    possiblyDeleteNode(cur);
    return true;
}

// This method is called on the last node when a word is removed from the trie.
// If the word is not a prefix of another word, we delete the node.
// Until we find a prefix of this word that exists in the set, we keep going up the trie, deleting nodes.
// For instance, if our set has "beta" and "be", and we remove "beta",
// we want to remove the "a" node, and go up and remove the "t" node as well.

void Trie::possiblyDeleteNode(std::shared_ptr<Node> node) {

    if (node == root_) {  // We don't want to delete the root
        return;
    }

    if (node->numChildren_ > 0) {  // Word is a prefix of another word
        return;
    }

    if (node->isEnd_) {  // Word exists in the trie
        return;
    }

    // Node->numChildren == 0 && node->isEnd == false
    
    // Delete self
    std::shared_ptr<Node> parent = node->parent_;
    
    // This removes the reference to the current node from the parent
    parent->children_[node->selfIndex_] = NULL;

    // Decrement number of children of parent
    parent->numChildren_--;

    possiblyDeleteNode(parent);
}

int Trie::size() {
    return size_;
}


// Simple test that tests insertion
void testBasicInsert() {

    Trie trie;
    trie.insert("be");
    trie.insert("bet");
    trie.insert("beta");

    IS_TRUE(trie.search("be"));
    IS_TRUE(trie.search("bet"));
    IS_TRUE(trie.search("beta"));

}

// Tests basic deletion - deletion of a word that is a prefix of another word and has a prefix in the trie
void testBasicDelete() {

    Trie trie;
    trie.insert("be");
    trie.insert("bet");
    trie.insert("beta");

    // Delete "bet" - which returns true if the word was in the trie and was successfully removed
    IS_TRUE(trie.erase("bet"));

    // Check that "bet" is no longer in the trie
    IS_FALSE(trie.search("bet"));

    // Check that "be" and "beta" are still in the trie
    IS_TRUE(trie.size() == 2);
    IS_TRUE(trie.search("be"));
    IS_TRUE(trie.search("beta"));

}


// Tests deletion of a word that is a prefix of another word, but does not have a prefix in the trie
void testBasicDelete2() {

    Trie trie;
    trie.insert("bet");
    trie.insert("beta");

    // Delete "bet" - which returns true if the word was in the trie and was successfully removed
    IS_TRUE(trie.erase("bet"));

    // Check that "bet" is no longer in the trie
    IS_FALSE(trie.search("bet"));

    // Check that "beta" is still in the trie
    IS_TRUE(trie.size() == 1);
    IS_TRUE(trie.search("beta"));

    // Check that "b" and "be" is not in the trie
    IS_FALSE(trie.search("b"));
    IS_FALSE(trie.search("be"));

}

// Tests deletion of a word that is not a prefix of another word, but has a prefix in the trie
void testBasicDelete3() {

    Trie trie;
    trie.insert("be");
    trie.insert("bet");

    // Delete "bet" - which returns true if the word was in the trie and was successfully removed
    IS_TRUE(trie.erase("bet"));

    // Check that "bet" is no longer in the trie
    IS_FALSE(trie.search("bet"));

    // Check that "beta" is still in the trie
    IS_TRUE(trie.size() == 1);
    IS_TRUE(trie.search("be"));

}


// Tests deletion of a word that is not a prefix of another word, and does not have a prefix in the trie
void testBasicDelete4() {

    Trie trie;
    trie.insert("a");
    trie.insert("bet");
    trie.insert("c");

    // Delete "bet" - which returns true if the word was in the trie and was successfully removed
    IS_TRUE(trie.erase("bet"));

    // Check that "bet" is no longer in the trie
    IS_FALSE(trie.search("bet"));

    // Check that "b" and "be" is not in the trie
    IS_TRUE(trie.size() == 2);
    IS_FALSE(trie.search("b"));
    IS_FALSE(trie.search("be"));

}

void basicTests() {
    testBasicInsert();
    testBasicDelete();
    testBasicDelete2();
    testBasicDelete3();
    testBasicDelete4();
}
 
// Driver
int main(int argc, char const* argv[]) {
    
    return 0;

}
