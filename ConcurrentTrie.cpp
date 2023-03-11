#include "ConcurrentTrie.h"


ConcurrentNode::ConcurrentNode() {
    for (int i = 0; i < NODE_SIZE; i++) {
        children_[i] = NULL;
    }
    parent_ = NULL;
    isEnd_ = false;
    numChildren_ = 0;
    omp_init_lock(&nodeLock_);
}

ConcurrentTrie::ConcurrentTrie() {
    root_ = std::make_shared<ConcurrentNode>();
    size_ = 0;
    rwLock_ = std::make_shared<FairReadersWriters>();
    asyncWriteLock_ = std::make_shared<FairReadersWriters>();
    omp_init_lock(&sizeLock_);
    
    // Best performance after testing
    omp_set_num_threads(4);
}

std::shared_ptr<ConcurrentTrie> ConcurrentTrie::createSharedPtr() {
    return shared_from_this();
}

void ConcurrentTrie::setNumThreads(int numThreads) {
    omp_set_num_threads(numThreads);
}

void ConcurrentTrie::setMaxThreads() {
    omp_set_num_threads(omp_get_max_threads());
}

int ConcurrentTrie::getIndexOfChar(char c) {
    int idx = int(c);
    if (idx < SMALLEST_CHAR || idx > LARGEST_CHAR) {
        printf("Invalid character: %c\n", c);
        throw std::invalid_argument("Invalid character");
    }
    return idx;
}

char ConcurrentTrie::getCharForIndex(int idx) {
    if (idx < SMALLEST_CHAR || idx > LARGEST_CHAR) {
        printf("Invalid index: %d\n", idx);
        throw std::invalid_argument("Invalid index");
    }
    return char(idx);
}
 
// Inserts a word into the ConcurrentTrie.
// Returns true if the word is already in the ConcurrentTrie.
void ConcurrentTrie::insert(std::string word) {

    if (word.length() == 0) {
        return;
    }

    rwLock_->startWrite();

    int index;
    std::shared_ptr<ConcurrentNode> cur = root_;

    for (int i = 0; i < word.length(); i++) {

        index = getIndexOfChar(word[i]);

        // Lock access to this node
        omp_set_lock(&cur->nodeLock_);

            if (!cur->children_[index]) {

                // Create new node and set other attributes
                std::shared_ptr<ConcurrentNode> newNode = std::make_shared<ConcurrentNode>();
                newNode->parent_ = cur;
                newNode->selfIndex_ = index;
                
                cur->children_[index] = newNode;
                cur->numChildren_++;
            }

        // Unlock access to this node
        omp_unset_lock(&cur->nodeLock_);

        cur = cur->children_[index];

    }

    if (cur->isEnd_) {
        rwLock_->endWrite();
        return;
    }

    omp_set_lock(&cur->nodeLock_);
        cur->isEnd_ = true;
    omp_unset_lock(&cur->nodeLock_);

    omp_set_lock(&sizeLock_);
        size_++;
    omp_unset_lock(&sizeLock_);
    
    rwLock_->endWrite();
}

// Inserts multiple words into the ConcurrentTrie.
void ConcurrentTrie::insert(std::vector<std::string>* words) {

    rwLock_->startWrite();

    #pragma omp parallel for
    for (int i = 0; i < words->size(); i++) {
        insert((*words)[i]);
    }

    rwLock_->endWrite();
}

void ConcurrentTrie::insertAsyncHelper(std::shared_ptr<ConcurrentTrie> trie, std::vector<std::string>* words) {
   
    #pragma omp parallel for
    for (int i = 0; i < words->size(); i++) {
        trie->insert((*words)[i]);
    }

    trie->asyncWriteLock_->endWrite();  // asyncWriteLock_->startWrite() was called by insertAsync()
}

void ConcurrentTrie::insertAsync(std::vector<std::string>* words) {
    // Calls insertAsyncHelper on a separate thread.

    asyncWriteLock_->startWrite();  // Within insertAsyncHelper, asyncWriteLock_->endWrite() will be called

    // Pass shared pointer to this ConcurrentTrie to the helper function,
    // otherwise the ConcurrentTrie will be destroyed before the helper function is done.
    std::shared_ptr<ConcurrentTrie> self = createSharedPtr();
    std::thread helperThread(ConcurrentTrie::insertAsyncHelper, self, words);
    helperThread.detach();
    return;
}

// Returns true if word is present in the ConcurrentTrie.
bool ConcurrentTrie::contains(std::string word) {

    asyncWriteLock_->startRead();  // Waits for asyncWriters to finish
    rwLock_->startRead();

    std::shared_ptr<ConcurrentNode> cur = root_;
    int index;
 
    for (int i = 0; i < word.length(); i++) {

        index = getIndexOfChar(word[i]);

        // Acquire lock on this node
        omp_set_lock(&cur->nodeLock_);

        if (!cur->children_[index]) {

            // Unlock access to this node
            omp_unset_lock(&cur->nodeLock_);
            
            rwLock_->endRead();
            asyncWriteLock_->endRead();
            return false;  // If word was in the ConcurrentTrie, this would not have been NULL.
        }
        
        omp_unset_lock(&cur->nodeLock_);
        cur = cur->children_[index];
    }
    
    omp_set_lock(&cur->nodeLock_);
    bool result = cur->isEnd_;
    omp_unset_lock(&cur->nodeLock_);


    rwLock_->endRead();
    asyncWriteLock_->endRead();
    return result;
}

// Checks if multiple words are present in the ConcurrentTrie.
// Returns a vector of booleans, where each boolean corresponds to the word at the same index in the input vector.
// The boolean is true if the word is present in the ConcurrentTrie.
std::vector<bool> ConcurrentTrie::contains(std::vector<std::string>* words) {

    rwLock_->startRead();
    bool results[words->size()];
    #pragma omp parallel for
    for (int i = 0; i < words->size(); i++) {
        results[i] = contains((*words)[i]);
    }
    rwLock_->endRead();
    return std::vector<bool>(results, results + words->size());

}


// Deletes a string from the ConcurrentTrie.
void ConcurrentTrie::remove(std::string word) {

    // There are a few possibilities:
    // 1. The word is not in the ConcurrentTrie. Return false in this scenario.
    // 2. The word is present. Unmark the end of the word (set as false).
    // If the word is not a prefix of another word, we delete the node.
    // If there exists another word that is a prefix of this word, we do not delete the word
    // otherwise we keep going up the ConcurrentTrie, deleting nodes until we reach a node that has more than one child.

    if (word.length() == 0) {
        return;
    }

    rwLock_->startWrite();

    int index;
    std::shared_ptr<ConcurrentNode> cur = root_;

    for (int i = 0; i < word.length(); i++) {
        index = getIndexOfChar(word[i]);
        if (!cur->children_[index]) {
            rwLock_->endWrite();
            return;  // Scenario 1
        }
        cur = cur->children_[index];
    }
 
    if (!cur->isEnd_) {
        rwLock_->endWrite();
        return;  // Scenario 1
    }

    omp_set_lock(&cur->nodeLock_);
        cur->isEnd_ = false;
    omp_unset_lock(&cur->nodeLock_);

    omp_set_lock(&sizeLock_);
        size_--;
    omp_unset_lock(&sizeLock_);

    possiblyDeleteNode(cur);
    rwLock_->endWrite();
}

// Deletes multiple strings from the ConcurrentTrie.
void ConcurrentTrie::remove(std::vector<std::string>* words) {

    rwLock_->startWrite();

    #pragma omp parallel for
    for (int i = 0; i < words->size(); i++) {
        remove((*words)[i]);
    }

    rwLock_->endWrite();
}

void ConcurrentTrie::removeAsyncHelper(std::shared_ptr<ConcurrentTrie> trie, std::vector<std::string>* words) {
   
    #pragma omp parallel for
    for (int i = 0; i < words->size(); i++) {
        trie->insert((*words)[i]);
    }

    trie->asyncWriteLock_->endWrite();  // asyncWriteLock_->startWrite() was called by removeAsync()
}

void ConcurrentTrie::removeAsync(std::vector<std::string>* words) {
    // Calls removeAsyncHelper on a separate thread

    asyncWriteLock_->startWrite();  // Within removeAsyncHelper, asyncWriteLock_->endWrite() will be called

    // Pass shared pointer to this ConcurrentTrie to the helper function,
    // otherwise the ConcurrentTrie will be destroyed before the helper function is done.
    std::shared_ptr<ConcurrentTrie> self = createSharedPtr();
    std::thread helperThread(ConcurrentTrie::removeAsyncHelper, self, words);
    helperThread.detach();
    return;
}

// This method is called on the last node when a word is removed from the ConcurrentTrie.
// If the word is not a prefix of another word, we delete the node.
// Until we find a prefix of this word that exists in the set, we keep going up the ConcurrentTrie, deleting nodes.
// For instance, if our set has "beta" and "be", and we remove "beta",
// we want to remove the "a" node, and go up and remove the "t" node as well.
void ConcurrentTrie::possiblyDeleteNode(std::shared_ptr<ConcurrentNode> node) {

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
    std::shared_ptr<ConcurrentNode> parent = node->parent_;
    
    omp_set_lock(&parent->nodeLock_);
    parent->children_[node->selfIndex_] = NULL;  // This removes the reference to the current node from the parent
    parent->numChildren_--;  // Decrement number of children of parent, since there are no more children with this next character
    omp_unset_lock(&parent->nodeLock_);

    possiblyDeleteNode(parent);
}


int ConcurrentTrie::size() {
    return size_;
}


// Given a prefix, return all words in the ConcurrentTrie that strictly starts with that prefix.
std::vector<std::string> ConcurrentTrie::getStringsWithPrefix(std::string prefix) {

    // Find the node that corresponds to the prefix
    std::shared_ptr<ConcurrentNode> cur = root_;
    int index;
    for (int i = 0; i < prefix.length(); i++) {
        index = getIndexOfChar(prefix[i]);
        if (!cur->children_[index]) {
            return std::vector<std::string>();  // return empty vector
        }
        cur = cur->children_[index];
    }

    return getAllStringsSortedHelper(cur, prefix);
}


// Returns all strings in the ConcurrentTrie, sorted alphabetically.
std::vector<std::string> ConcurrentTrie::getAllStringsSorted() {
    // In this sequential implementation, this returns the same thing as getStringsWithPrefix("")

    return getAllStringsSortedHelper(root_, "");

}

// Helper function for getAllStringsSorted()
std::vector<std::string> ConcurrentTrie::getAllStringsSortedHelper(std::shared_ptr<ConcurrentNode> node, std::string prefix) {

    if (node == NULL) {
        return std::vector<std::string>();
    }

    std::vector<std::string> words;

    // Initialise a stack of (node, word)
    std::stack<std::pair<std::shared_ptr<ConcurrentNode>, std::string>> stack;
    stack.push(std::make_pair(node, prefix));

    std::pair<std::shared_ptr<ConcurrentNode>, std::string> curPairToEvaluate;
    std::shared_ptr<ConcurrentNode> curNodeToEvaluate;
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
