#include "SequentialTrie.h"

SequentialNode::SequentialNode() {
    for (int i = 0; i < NODE_SIZE; i++) {
        children_[i] = NULL;
    }
    parent_ = NULL;
    isEnd_ = false;
    numChildren_ = 0;
}

SequentialTrie::SequentialTrie() {
    root_ = std::make_shared<SequentialNode>();
    size_ = 0;
}

int SequentialTrie::getIndexOfChar(char c) {
    int idx = int(c);
    // if (idx < 0 || idx >= NODE_SIZE) {
    if (idx < SMALLEST_CHAR || idx > LARGEST_CHAR) {
        throw std::invalid_argument("Invalid character");
    }
    return idx;
}

char SequentialTrie::getCharForIndex(int idx) {
    if (idx < SMALLEST_CHAR || idx > LARGEST_CHAR) {
        throw std::invalid_argument("Invalid index");
    }
    return char(idx);
}
 
// Inserts a word into the SequentialTrie.
// Returns true if the word is already in the SequentialTrie.
void SequentialTrie::insert(std::string word) {

    std::shared_ptr<SequentialNode> cur = root_;
    int index;
 
    for (int i = 0; i < word.length(); i++) {

        index = getIndexOfChar(word[i]);

        if (!cur->children_[index]) {

            // Create new node and set other attributes
            std::shared_ptr<SequentialNode> newNode = std::make_shared<SequentialNode>();
            newNode->parent_ = cur;
            newNode->selfIndex_ = index;
            newNode->isEnd_ = false;
            newNode->numChildren_ = 0;
            for (int i = 0; i < NODE_SIZE; i++) {
                newNode->children_[i] = NULL;
            }
            
            cur->children_[index] = newNode;
            cur->numChildren_++;
        }
        cur = cur->children_[index];

    }
 
    if (cur->isEnd_) {
        return;
    }

    cur->isEnd_ = true;
    size_++;
    return;
}

// Inserts multiple words into the SequentialTrie.
// Returns a vector of booleans, where each boolean corresponds to the word at the same index in the input vector.
// The boolean is true if the word is already in the SequentialTrie.
void SequentialTrie::insert(std::vector<std::string>* words) {

    for (int i = 0; i < words->size(); i++) {
        insert((*words)[i]);
    }
}
 
// Returns true if word is present in the SequentialTrie.
bool SequentialTrie::contains(std::string word) {

    std::shared_ptr<SequentialNode> cur = root_;
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

// Checks if multiple words are present in the SequentialTrie.
// Returns a vector of booleans, where each boolean corresponds to the word at the same index in the input vector.
// The boolean is true if the word is present in the SequentialTrie.
std::vector<bool> SequentialTrie::contains(std::vector<std::string>* words) {

    std::vector<bool> results;
    for (int i = 0; i < words->size(); i++) {
        results.push_back(contains((*words)[i]));
    }
    return results;
}


// Deletes a word from the SequentialTrie.
// Returns true if word was in the SequentialTrie and was successfully removed.
void SequentialTrie::remove(std::string word) {

    // There are a few possibilities:
    // 1. The word is not in the SequentialTrie. Return false in this scenario.
    // 2. The word is present. Unmark the end of the word (set as false).
    // If the word is not a prefix of another word, we delete the node.
    // If there exists another word that is a prefix of this word, we do not delete the word
    // otherwise we keep going up the SequentialTrie, deleting nodes until we reach a node that has more than one child.

    std::shared_ptr<SequentialNode> cur = root_;
    int index;
 
    for (int i = 0; i < word.length(); i++) {
        index = getIndexOfChar(word[i]);
        if (!cur->children_[index]) {
            return;
        }
        cur = cur->children_[index];
    }
 
    if (!cur->isEnd_) {
        return;  // Scenario 1
    }

    cur->isEnd_ = false;
    size_--;
    possiblyDeleteNode(cur);
}

// Deletes multiple words from the SequentialTrie.
// Returns a vector of booleans, where each boolean corresponds to the word at the same index in the input vector.
// The boolean is true if the word was in the SequentialTrie and was successfully removed.
void SequentialTrie::remove(std::vector<std::string>* words) {

    std::vector<bool> results;
    for (int i = 0; i < words->size(); i++) {
        remove((*words)[i]);
    }
}

// This method is called on the last node when a word is removed from the SequentialTrie.
// If the word is not a prefix of another word, we delete the node.
// Until we find a prefix of this word that exists in the set, we keep going up the SequentialTrie, deleting nodes.
// For instance, if our set has "beta" and "be", and we remove "beta",
// we want to remove the "a" node, and go up and remove the "t" node as well.
void SequentialTrie::possiblyDeleteNode(std::shared_ptr<SequentialNode> node) {

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
    std::shared_ptr<SequentialNode> parent = node->parent_;
    
    // This removes the reference to the current node from the parent
    parent->children_[node->selfIndex_] = NULL;

    // Decrement number of children of parent
    parent->numChildren_--;

    possiblyDeleteNode(parent);
}


int SequentialTrie::size() {
    return size_;
}


// Given a prefix, return all strings in the SequentialTrie that strictly starts with that prefix.
std::vector<std::string> SequentialTrie::getStringsWithPrefix(std::string prefix) {

    std::vector<std::string> words;

    // Find the node that corresponds to the prefix
    std::shared_ptr<SequentialNode> cur = root_;
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
    std::stack<std::pair<std::shared_ptr<SequentialNode>, std::string>> stack;
    stack.push(std::make_pair(cur, word));

    std::pair<std::shared_ptr<SequentialNode>, std::string> curPairToEvaluate;
    std::shared_ptr<SequentialNode> curNodeToEvaluate;
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
std::vector<std::string> SequentialTrie::getAllStringsSorted() {
    // In this sequential implementation, this returns the same thing as getStringsWithPrefix("")

    std::vector<std::string> words;

    std::string word = "";

    // Initialise a stack of (node, word)
    std::stack<std::pair<std::shared_ptr<SequentialNode>, std::string>> stack;
    stack.push(std::make_pair(root_, word));

    std::pair<std::shared_ptr<SequentialNode>, std::string> curPairToEvaluate;
    std::shared_ptr<SequentialNode> curNodeToEvaluate;
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
