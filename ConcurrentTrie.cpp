#include "ConcurrentTrie.h"

Node::Node() {
    for (int i = 0; i < NODE_SIZE; i++) {
        children_[i] = NULL;
    }
    parent_ = NULL;
    isEnd_ = false;
    numChildren_ = 0;
}

ConcurrentTrie::ConcurrentTrie() {
    root_ = std::make_shared<Node>();
    size_ = 0;
}

int ConcurrentTrie::getIndexOfChar(char c) {
    int idx = c - ' ';
    if (idx < 0 || idx >= NODE_SIZE) {
        throw std::invalid_argument("Invalid character");
    }
    return idx;
}

char ConcurrentTrie::getCharForIndex(int idx) {
    if (idx < 0 || idx >= NODE_SIZE) {
        throw std::invalid_argument("Invalid index");
    }
    return idx + ' ';
}
 
// Inserts a word into the ConcurrentTrie.
// Returns true if the word is already in the ConcurrentTrie.
bool ConcurrentTrie::insert(std::string word) {

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

// Inserts multiple words into the ConcurrentTrie.
// Returns true if all words are already in the ConcurrentTrie.
bool ConcurrentTrie::insert(std::vector<std::string> words) {

    bool allPresent = true;
    for (int i = 0; i < words.size(); i++) {
        if (!insert(words[i])) {
            allPresent = false;
        }
    }
    return allPresent;
}
 
// Returns true if word is present in the ConcurrentTrie.
bool ConcurrentTrie::search(std::string word) {

    std::shared_ptr<Node> cur = root_;
    int index;
 
    for (int i = 0; i < word.length(); i++) {
        index = getIndexOfChar(word[i]);
        if (!cur->children_[index]) {
            return false;  // If word was in the ConcurrentTrie, this would not have been NULL.
        }
        cur = cur->children_[index];
    }
 
    return cur->isEnd_;
}

// Returns true if word was in the ConcurrentTrie and was successfully removed.
bool ConcurrentTrie::erase(std::string word) {

    // There are a few possibilities:
    // 1. The word is not in the ConcurrentTrie. Return false in this scenario.
    // 2. The word is present. Unmark the end of the word (set as false).
    // If the word is not a prefix of another word, we delete the node.
    // If there exists another word that is a prefix of this word, we do not delete the word
    // otherwise we keep going up the ConcurrentTrie, deleting nodes until we reach a node that has more than one child.

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

// This method is called on the last node when a word is removed from the ConcurrentTrie.
// If the word is not a prefix of another word, we delete the node.
// Until we find a prefix of this word that exists in the set, we keep going up the ConcurrentTrie, deleting nodes.
// For instance, if our set has "beta" and "be", and we remove "beta",
// we want to remove the "a" node, and go up and remove the "t" node as well.
void ConcurrentTrie::possiblyDeleteNode(std::shared_ptr<Node> node) {

    if (node == root_) {  // We don't want to delete the root
        return;
    }

    if (node->numChildren_ > 0) {  // Word is a prefix of another word
        return;
    }

    if (node->isEnd_) {  // Word exists in the ConcurrentTrie
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


int ConcurrentTrie::size() {
    return size_;
}


// Given a prefix, return all words in the ConcurrentTrie that strictly starts with that prefix.
std::vector<std::string> ConcurrentTrie::getWordsWithPrefix(std::string prefix) {

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


// Returns all words in the ConcurrentTrie, sorted alphabetically.
std::vector<std::string> ConcurrentTrie::getAllWordsSorted() {
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


// Given a word, return the lexicographically next word in the ConcurrentTrie.
std::string ConcurrentTrie::findLexicographicalNext(std::string word) {
    return "";
}

// Returns all words in the ConcurrentTrie that are lexicographically greater than the leftBound.
// For instance, if the ConcurrentTrie has "be", "bet", "beta", and "bey", and the leftBound is "bet",
// this method returns "beta" and "bey".
std::vector<std::string> ConcurrentTrie::getWordsLexicographicallyGreaterThan(std::string leftBound) {
    std::vector<std::string> words;
    std::string word;
    // getWordsLexicographicallyGreaterThanHelper(root_, word, leftBound, &words);
    return words;
}


// Simple test that tests insertion
void testBasicInsert() {

    ConcurrentTrie ConcurrentTrie;
    ConcurrentTrie.insert("be");
    ConcurrentTrie.insert("bet");
    ConcurrentTrie.insert("beta");

    IS_TRUE(ConcurrentTrie.search("be"));
    IS_TRUE(ConcurrentTrie.search("bet"));
    IS_TRUE(ConcurrentTrie.search("beta"));

}

// Tests basic deletion - deletion of a word that is a prefix of another word and has a prefix in the ConcurrentTrie
void testBasicDelete() {

    ConcurrentTrie ConcurrentTrie;
    ConcurrentTrie.insert("be");
    ConcurrentTrie.insert("bet");
    ConcurrentTrie.insert("beta");

    // Delete "bet" - which returns true if the word was in the ConcurrentTrie and was successfully removed
    IS_TRUE(ConcurrentTrie.erase("bet"));

    // Check that "bet" is no longer in the ConcurrentTrie
    IS_FALSE(ConcurrentTrie.search("bet"));

    // Check that "be" and "beta" are still in the ConcurrentTrie
    IS_TRUE(ConcurrentTrie.size() == 2);
    IS_TRUE(ConcurrentTrie.search("be"));
    IS_TRUE(ConcurrentTrie.search("beta"));

}


// Tests deletion of a word that is a prefix of another word, but does not have a prefix in the ConcurrentTrie
void testBasicDelete2() {

    ConcurrentTrie ConcurrentTrie;
    ConcurrentTrie.insert("bet");
    ConcurrentTrie.insert("beta");

    // Delete "bet" - which returns true if the word was in the ConcurrentTrie and was successfully removed
    IS_TRUE(ConcurrentTrie.erase("bet"));

    // Check that "bet" is no longer in the ConcurrentTrie
    IS_FALSE(ConcurrentTrie.search("bet"));

    // Check that "beta" is still in the ConcurrentTrie
    IS_TRUE(ConcurrentTrie.size() == 1);
    IS_TRUE(ConcurrentTrie.search("beta"));

    // Check that "b" and "be" is not in the ConcurrentTrie
    IS_FALSE(ConcurrentTrie.search("b"));
    IS_FALSE(ConcurrentTrie.search("be"));

}

// Tests deletion of a word that is not a prefix of another word, but has a prefix in the ConcurrentTrie
void testBasicDelete3() {

    ConcurrentTrie ConcurrentTrie;
    ConcurrentTrie.insert("be");
    ConcurrentTrie.insert("bet");

    // Delete "bet" - which returns true if the word was in the ConcurrentTrie and was successfully removed
    IS_TRUE(ConcurrentTrie.erase("bet"));

    // Check that "bet" is no longer in the ConcurrentTrie
    IS_FALSE(ConcurrentTrie.search("bet"));

    // Check that "beta" is still in the ConcurrentTrie
    IS_TRUE(ConcurrentTrie.size() == 1);
    IS_TRUE(ConcurrentTrie.search("be"));

}


// Tests deletion of a word that is not a prefix of another word, and does not have a prefix in the ConcurrentTrie
void testBasicDelete4() {

    ConcurrentTrie ConcurrentTrie;
    ConcurrentTrie.insert("a");
    ConcurrentTrie.insert("bet");
    ConcurrentTrie.insert("c");

    // Delete "bet" - which returns true if the word was in the ConcurrentTrie and was successfully removed
    IS_TRUE(ConcurrentTrie.erase("bet"));

    // Check that "bet" is no longer in the ConcurrentTrie
    IS_FALSE(ConcurrentTrie.search("bet"));

    // Check that "b" and "be" is not in the ConcurrentTrie
    IS_TRUE(ConcurrentTrie.size() == 2);
    IS_FALSE(ConcurrentTrie.search("b"));
    IS_FALSE(ConcurrentTrie.search("be"));

}


void testGetWordsWithPrefix() {

    ConcurrentTrie ConcurrentTrie;
    ConcurrentTrie.insert("a");
    ConcurrentTrie.insert("be");
    ConcurrentTrie.insert("bet");
    ConcurrentTrie.insert("beta");
    ConcurrentTrie.insert("c");

    std::vector<std::string> words = ConcurrentTrie.getWordsWithPrefix("b");

    IS_TRUE(words.size() == 3);
    IS_TRUE(std::find(words.begin(), words.end(), "be") != words.end());
    IS_TRUE(std::find(words.begin(), words.end(), "bet") != words.end());
    IS_TRUE(std::find(words.begin(), words.end(), "beta") != words.end());
    
}

void testGetWordsSorted() {

    std::vector<std::string> stringsToTest = std::vector<std::string>({"a", "be", "bet", "beta", "c"});

    ConcurrentTrie ConcurrentTrie;
    for (std::string str : stringsToTest) {
        ConcurrentTrie.insert(str);
    }

    std::vector<std::string> words = ConcurrentTrie.getAllWordsSorted();
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