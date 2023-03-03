#include "SequentialTrie.h"

Node::Node() {
    for (int i = 0; i < NODE_SIZE; i++) {
        children_[i] = NULL;
    }
    parent_ = NULL;
    isEnd_ = false;
    numChildren_ = 0;
}

SequentialTrie::SequentialTrie() {
    root_ = std::make_shared<Node>();
    size_ = 0;
}

int SequentialTrie::getIndexOfChar(char c) {
    int idx = c - ' ';
    if (idx < 0 || idx >= NODE_SIZE) {
        throw std::invalid_argument("Invalid character");
    }
    return idx;
}

char SequentialTrie::getCharForIndex(int idx) {
    if (idx < 0 || idx >= NODE_SIZE) {
        throw std::invalid_argument("Invalid index");
    }
    return idx + ' ';
}
 
// Inserts a word into the SequentialTrie.
// Returns true if the word is already in the SequentialTrie.
bool SequentialTrie::insert(std::string word) {

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

// Inserts multiple words into the SequentialTrie.
// Returns true if all words are already in the SequentialTrie.
bool SequentialTrie::insert(std::vector<std::string> words) {

    bool allPresent = true;
    for (int i = 0; i < words.size(); i++) {
        if (!insert(words[i])) {
            allPresent = false;
        }
    }
    return allPresent;
}
 
// Returns true if word is present in the SequentialTrie.
bool SequentialTrie::search(std::string word) {

    std::shared_ptr<Node> cur = root_;
    int index;
 
    for (int i = 0; i < word.length(); i++) {
        index = getIndexOfChar(word[i]);
        if (!cur->children_[index]) {
            return false;  // If word was in the SequentialTrie, this would not have been NULL.
        }
        cur = cur->children_[index];
    }
 
    return cur->isEnd_;
}

// Returns true if word was in the SequentialTrie and was successfully removed.
bool SequentialTrie::erase(std::string word) {

    // There are a few possibilities:
    // 1. The word is not in the SequentialTrie. Return false in this scenario.
    // 2. The word is present. Unmark the end of the word (set as false).
    // If the word is not a prefix of another word, we delete the node.
    // If there exists another word that is a prefix of this word, we do not delete the word
    // otherwise we keep going up the SequentialTrie, deleting nodes until we reach a node that has more than one child.

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

// This method is called on the last node when a word is removed from the SequentialTrie.
// If the word is not a prefix of another word, we delete the node.
// Until we find a prefix of this word that exists in the set, we keep going up the SequentialTrie, deleting nodes.
// For instance, if our set has "beta" and "be", and we remove "beta",
// we want to remove the "a" node, and go up and remove the "t" node as well.
void SequentialTrie::possiblyDeleteNode(std::shared_ptr<Node> node) {

    if (node == root_) {  // We don't want to delete the root
        return;
    }

    if (node->numChildren_ > 0) {  // Word is a prefix of another word
        return;
    }

    if (node->isEnd_) {  // Word exists in the SequentialTrie
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


int SequentialTrie::size() {
    return size_;
}


// Given a prefix, return all words in the SequentialTrie that strictly starts with that prefix.
std::vector<std::string> SequentialTrie::getWordsWithPrefix(std::string prefix) {

    std::vector<std::string> words;

    // Find the node that corresponds to the prefix
    std::shared_ptr<Node> cur = root_;
    int index;
    for (int i = 0; i < prefix.length(); i++) {
        index = getIndexOfChar(prefix[i]);
        if (!cur->children_[index]) {
            return words;  // Return empty vector
        }
        cur = cur->children_[index];
    }

    // Now we have the node that corresponds to the prefix.
    std::string word = prefix;

    // Initialise a stack of (node, word)
    std::stack<std::pair<std::shared_ptr<Node>, std::string>> stack;
    stack.push(std::make_pair(cur, word));

    std::pair<std::shared_ptr<Node>, std::string> curPairToEvaluate;
    std::shared_ptr<Node> curNodeToEvaluate;
    std::string curWordToEvaluate;
    while (!stack.empty()) {
        curPairToEvaluate = stack.top();
        stack.pop();
        curNodeToEvaluate = curPairToEvaluate.first;
        curWordToEvaluate = curPairToEvaluate.second;

        if (curNodeToEvaluate->isEnd_) {
            words.push_back(curWordToEvaluate);  // We have a word
        }

        for (int i = NODE_SIZE - 1; i >= 0; i--) {  // Iterate through all children
            if (curNodeToEvaluate->children_[i]) {
                curWordToEvaluate.push_back(getCharForIndex(i));
                stack.push(std::make_pair(curNodeToEvaluate->children_[i], curWordToEvaluate));
                curWordToEvaluate.pop_back();
            }
        }
    }
    return words;
}


// Returns all words in the SequentialTrie, sorted alphabetically.
std::vector<std::string> SequentialTrie::getAllWordsSorted() {
    // In this sequential implementation, this returns the same thing as getWordsWithPrefix("")

    std::vector<std::string> words;

    std::string word = "";

    // Initialise a stack of (node, word)
    std::stack<std::pair<std::shared_ptr<Node>, std::string>> stack;
    stack.push(std::make_pair(root_, word));

    std::pair<std::shared_ptr<Node>, std::string> curPairToEvaluate;
    std::shared_ptr<Node> curNodeToEvaluate;
    std::string curWordToEvaluate;
    while (!stack.empty()) {
        curPairToEvaluate = stack.top();
        stack.pop();
        curNodeToEvaluate = curPairToEvaluate.first;
        curWordToEvaluate = curPairToEvaluate.second;

        if (curNodeToEvaluate->isEnd_) {
            words.push_back(curWordToEvaluate);  // We have a word
        }

        for (int i = NODE_SIZE - 1; i >= 0; i--) {  // Iterate through all children
            if (curNodeToEvaluate->children_[i]) {
                curWordToEvaluate.push_back(getCharForIndex(i));
                stack.push(std::make_pair(curNodeToEvaluate->children_[i], curWordToEvaluate));
                curWordToEvaluate.pop_back();
            }
        }
    }
    return words;

}


// Given a word, return the lexicographically next word in the SequentialTrie.
std::string SequentialTrie::findLexicographicalNext(std::string word) {
    return "";
}

// Returns all words in the SequentialTrie that are lexicographically greater than the leftBound.
// For instance, if the SequentialTrie has "be", "bet", "beta", and "bey", and the leftBound is "bet",
// this method returns "beta" and "bey".
std::vector<std::string> SequentialTrie::getWordsLexicographicallyGreaterThan(std::string leftBound) {
    std::vector<std::string> words;
    std::string word;
    // getWordsLexicographicallyGreaterThanHelper(root_, word, leftBound, &words);
    return words;
}


// Simple test that tests insertion
void testBasicInsert() {

    SequentialTrie SequentialTrie;
    SequentialTrie.insert("be");
    SequentialTrie.insert("bet");
    SequentialTrie.insert("beta");

    IS_TRUE(SequentialTrie.search("be"));
    IS_TRUE(SequentialTrie.search("bet"));
    IS_TRUE(SequentialTrie.search("beta"));

}

// Tests basic deletion - deletion of a word that is a prefix of another word and has a prefix in the SequentialTrie
void testBasicDelete() {

    SequentialTrie SequentialTrie;
    SequentialTrie.insert("be");
    SequentialTrie.insert("bet");
    SequentialTrie.insert("beta");

    // Delete "bet" - which returns true if the word was in the SequentialTrie and was successfully removed
    IS_TRUE(SequentialTrie.erase("bet"));

    // Check that "bet" is no longer in the SequentialTrie
    IS_FALSE(SequentialTrie.search("bet"));

    // Check that "be" and "beta" are still in the SequentialTrie
    IS_TRUE(SequentialTrie.size() == 2);
    IS_TRUE(SequentialTrie.search("be"));
    IS_TRUE(SequentialTrie.search("beta"));

}


// Tests deletion of a word that is a prefix of another word, but does not have a prefix in the SequentialTrie
void testBasicDelete2() {

    SequentialTrie SequentialTrie;
    SequentialTrie.insert("bet");
    SequentialTrie.insert("beta");

    // Delete "bet" - which returns true if the word was in the SequentialTrie and was successfully removed
    IS_TRUE(SequentialTrie.erase("bet"));

    // Check that "bet" is no longer in the SequentialTrie
    IS_FALSE(SequentialTrie.search("bet"));

    // Check that "beta" is still in the SequentialTrie
    IS_TRUE(SequentialTrie.size() == 1);
    IS_TRUE(SequentialTrie.search("beta"));

    // Check that "b" and "be" is not in the SequentialTrie
    IS_FALSE(SequentialTrie.search("b"));
    IS_FALSE(SequentialTrie.search("be"));

}

// Tests deletion of a word that is not a prefix of another word, but has a prefix in the SequentialTrie
void testBasicDelete3() {

    SequentialTrie SequentialTrie;
    SequentialTrie.insert("be");
    SequentialTrie.insert("bet");

    // Delete "bet" - which returns true if the word was in the SequentialTrie and was successfully removed
    IS_TRUE(SequentialTrie.erase("bet"));

    // Check that "bet" is no longer in the SequentialTrie
    IS_FALSE(SequentialTrie.search("bet"));

    // Check that "beta" is still in the SequentialTrie
    IS_TRUE(SequentialTrie.size() == 1);
    IS_TRUE(SequentialTrie.search("be"));

}


// Tests deletion of a word that is not a prefix of another word, and does not have a prefix in the SequentialTrie
void testBasicDelete4() {

    SequentialTrie SequentialTrie;
    SequentialTrie.insert("a");
    SequentialTrie.insert("bet");
    SequentialTrie.insert("c");

    // Delete "bet" - which returns true if the word was in the SequentialTrie and was successfully removed
    IS_TRUE(SequentialTrie.erase("bet"));

    // Check that "bet" is no longer in the SequentialTrie
    IS_FALSE(SequentialTrie.search("bet"));

    // Check that "b" and "be" is not in the SequentialTrie
    IS_TRUE(SequentialTrie.size() == 2);
    IS_FALSE(SequentialTrie.search("b"));
    IS_FALSE(SequentialTrie.search("be"));

}


void testGetWordsWithPrefix() {

    SequentialTrie SequentialTrie;
    SequentialTrie.insert("a");
    SequentialTrie.insert("be");
    SequentialTrie.insert("bet");
    SequentialTrie.insert("beta");
    SequentialTrie.insert("c");

    std::vector<std::string> words = SequentialTrie.getWordsWithPrefix("b");

    IS_TRUE(words.size() == 3);
    IS_TRUE(std::find(words.begin(), words.end(), "be") != words.end());
    IS_TRUE(std::find(words.begin(), words.end(), "bet") != words.end());
    IS_TRUE(std::find(words.begin(), words.end(), "beta") != words.end());
    
}

void testGetWordsSorted() {

    std::vector<std::string> stringsToTest = std::vector<std::string>({"a", "be", "bet", "beta", "c"});

    SequentialTrie SequentialTrie;
    for (std::string str : stringsToTest) {
        SequentialTrie.insert(str);
    }

    std::vector<std::string> words = SequentialTrie.getAllWordsSorted();
    IS_TRUE(words == stringsToTest);

}

void basicTests() {
    testBasicInsert();
    testBasicDelete();
    testBasicDelete2();
    testBasicDelete3();
    testBasicDelete4();

    testGetWordsWithPrefix();
    testGetWordsSorted();
}
 
// Driver
int main(int argc, char const* argv[]) {
    
    basicTests();
    return 0;

}
