#include <fstream>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

#include "ConcurrentTrie.h"
#include "SequentialTrie.h"


#define IS_TRUE(x) { if (!(x)) printf("%s failed on line %d\n", __FUNCTION__, __LINE__); }
#define IS_FALSE(x) { if ((x)) printf("%s failed on line %d\n", __FUNCTION__, __LINE__); }

// Simple test that tests insert and contains
void testBasicInsertAndContains() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"be", "bet", "beta"};

    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }

    for (std::string word : words) {
        IS_TRUE(sequentialTrie.contains(word));
        IS_TRUE(concurrentTrie.contains(word));
    }

    IS_TRUE(sequentialTrie.size() == words.size());
    IS_TRUE(concurrentTrie.size() == words.size());

}

// Tests basic deletion - deletion of a word that is a prefix of another word and has a prefix in the trie
void testBasicRemove() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"be", "bet", "beta"};
    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }


    // Remove "bet" - which returns true if the word was in the trie and was successfully removed
    sequentialTrie.remove("bet");
    concurrentTrie.remove("bet");

    // Check that "bet" is no longer in the trie
    IS_FALSE(sequentialTrie.contains("bet"));
    IS_FALSE(concurrentTrie.contains("bet"));

    // Check that "be" and "beta" are still in the trie
    IS_TRUE(sequentialTrie.size() == 2);
    IS_TRUE(sequentialTrie.contains("be"));
    IS_TRUE(sequentialTrie.contains("beta"));
    
    IS_TRUE(concurrentTrie.size() == 2);
    IS_TRUE(concurrentTrie.contains("be"));
    IS_TRUE(concurrentTrie.contains("beta"));

}


// Tests deletion of a word that is a prefix of another word, but does not have a prefix in the trie
void testBasicRemove2() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"bet", "beta"};
    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }

    // Remove "bet" - which returns true if the word was in the trie and was successfully removed
    sequentialTrie.remove("bet");
    concurrentTrie.remove("bet");

    // Check that "bet" is no longer in the trie
    IS_FALSE(sequentialTrie.contains("bet"));
    IS_FALSE(concurrentTrie.contains("bet"));

    // Check that "beta" is still in the trie
    IS_TRUE(sequentialTrie.size() == 1);
    IS_TRUE(sequentialTrie.contains("beta"));

    IS_TRUE(concurrentTrie.size() == 1);
    IS_TRUE(concurrentTrie.contains("beta"));

    // Check that "b" and "be" is not in the trie
    IS_FALSE(sequentialTrie.contains("b"));
    IS_FALSE(sequentialTrie.contains("be"));

    IS_FALSE(concurrentTrie.contains("b"));
    IS_FALSE(concurrentTrie.contains("be"));

}

// Tests deletion of a word that is not a prefix of another word, but has a prefix in the trie
void testBasicRemove3() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"be", "bet"};
    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }

    // Remove "bet" - which returns true if the word was in the trie and was successfully removed
    sequentialTrie.remove("bet");
    concurrentTrie.remove("bet");

    // Check that "bet" is no longer in the trie
    IS_FALSE(sequentialTrie.contains("bet"));
    IS_FALSE(concurrentTrie.contains("bet"));

    // Check that "beta" is still in the trie
    IS_TRUE(sequentialTrie.size() == 1);
    IS_TRUE(sequentialTrie.contains("be"));

    IS_TRUE(concurrentTrie.size() == 1);
    IS_TRUE(concurrentTrie.contains("be"));

}


// Tests deletion of a word that is not a prefix of another word, and does not have a prefix in the trie
void testBasicRemove4() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"a", "bet", "c"};
    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }

    // Remove "bet" - which returns true if the word was in the trie and was successfully removed
    sequentialTrie.remove("bet");
    concurrentTrie.remove("bet");

    // Check that "bet" is no longer in the trie
    IS_FALSE(sequentialTrie.contains("bet"));
    IS_FALSE(concurrentTrie.contains("bet"));

    // Check that "b" and "be" is not in the trie
    IS_TRUE(sequentialTrie.size() == 2);
    IS_FALSE(sequentialTrie.contains("b"));
    IS_FALSE(sequentialTrie.contains("be"));

    IS_TRUE(concurrentTrie.size() == 2);
    IS_FALSE(concurrentTrie.contains("b"));
    IS_FALSE(concurrentTrie.contains("be"));

}

void testGetWordsWithPrefix() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"a", "be", "bet", "beta", "c"};
    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }

    std::vector<std::string> wordsS = sequentialTrie.getStringsWithPrefix("b");
    IS_TRUE(wordsS.size() == 3);
    IS_TRUE(std::find(wordsS.begin(), wordsS.end(), "be") != wordsS.end());
    IS_TRUE(std::find(wordsS.begin(), wordsS.end(), "bet") != wordsS.end());
    IS_TRUE(std::find(wordsS.begin(), wordsS.end(), "beta") != wordsS.end());

    std::vector<std::string> wordsC = concurrentTrie.getStringsWithPrefix("b");
    IS_TRUE(wordsC.size() == 3);
    IS_TRUE(std::find(wordsC.begin(), wordsC.end(), "be") != wordsC.end());
    IS_TRUE(std::find(wordsC.begin(), wordsC.end(), "bet") != wordsC.end());
    IS_TRUE(std::find(wordsC.begin(), wordsC.end(), "beta") != wordsC.end());
    
}

void testGetWordsSorted() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> stringsToTest = std::vector<std::string>({"a", "be", "bet", "beta", "c"});
    for (std::string word : stringsToTest) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }

    std::vector<std::string> wordsS = sequentialTrie.getAllStringsSorted();
    std::vector<std::string> wordsC = concurrentTrie.getAllStringsSorted();
    IS_TRUE(stringsToTest == wordsS);
    IS_TRUE(stringsToTest == wordsC);

}

void basicTests() {

    testBasicInsertAndContains();
    testBasicRemove();
    testBasicRemove2();
    testBasicRemove3();
    testBasicRemove4();

    testGetWordsWithPrefix();
    testGetWordsSorted();
}

void testMultipleInsert(std::vector<std::string> wordList) {
    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    // Insert words
    sequentialTrie.insert(&wordList);
    concurrentTrie.insert(&wordList);

    // Check that all words are in the trie
    for (std::string word : wordList) {
        IS_TRUE(sequentialTrie.contains(word));
        IS_TRUE(concurrentTrie.contains(word));
    }
}

void testMultipleContains(std::vector<std::string> wordList) {
    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;
    std::unordered_set<std::string> wordSet;

    sequentialTrie.insert(&wordList);
    concurrentTrie.insert(&wordList);
    wordSet.insert(wordList.begin(), wordList.end());

    // Check that all words are in the trie
    std::vector<bool> sequentialResult = sequentialTrie.contains(&wordList);
    std::vector<bool> concurrentResult = concurrentTrie.contains(&wordList);

    for (int i = 0; i < wordList.size(); i ++) {
        IS_TRUE(sequentialResult[i]);
        IS_TRUE(concurrentResult[i]);
    }

    IS_TRUE(sequentialTrie.size() == wordSet.size());
    IS_TRUE(concurrentTrie.size() == wordSet.size());
}


void testMultipleRemove(std::vector<std::string> wordList) {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;
    sequentialTrie.insert(&wordList);
    concurrentTrie.insert(&wordList);

    // Get a subset of words
    std::vector<std::string> wordListSubset;
    for (int i = 0; i < wordList.size(); i ++) {
        if (rand() % 2 == 0) {
            wordListSubset.push_back(wordList[i]);
        }
    }

    // Remove words
    sequentialTrie.remove(&wordListSubset);
    concurrentTrie.remove(&wordListSubset);

    // Check that all words are in the trie
    for (std::string word : wordListSubset) {
        IS_FALSE(sequentialTrie.contains(word));
        IS_FALSE(concurrentTrie.contains(word));
    }

}


void testAsyncInsert(std::vector<std::string> wordList) {
    
    std::shared_ptr<ConcurrentTrie> concurrentTrie = std::make_shared<ConcurrentTrie>();

    // Insert words
    concurrentTrie->insertAsync(&wordList);

    // Check that all words are in the trie
    for (std::string word : wordList) {
        IS_TRUE(concurrentTrie->contains(word));
    }
}

void testAsyncRemove(std::vector<std::string> wordList) {
    
    std::shared_ptr<ConcurrentTrie> concurrentTrie = std::make_shared<ConcurrentTrie>();
    concurrentTrie->insert(&wordList);

    // Insert words
    concurrentTrie->removeAsync(&wordList);

    // Check that all words are in the trie
    for (std::string word : wordList) {
        IS_FALSE(concurrentTrie->contains(word));
    }
}


void concurrentTests(std::vector<std::string> wordList) {

    testMultipleInsert(wordList);
    testMultipleContains(wordList);
    testMultipleRemove(wordList);

    testAsyncInsert(wordList);
    testAsyncRemove(wordList);

}


std::vector<std::string> loadWords(std::string filepath, int numWords) {
    std::vector<std::string> wordList;
    std::string word;

    int wordNum = 0;
    std::ifstream* wordListFile = new std::ifstream(filepath);
    if (wordListFile->is_open()) {
        while (std::getline(*wordListFile, word)) {
            wordNum++;
            // Check if word is valid
            for (char c : word) {
                if (int(c) < 0 || int(c) > 127) {
                    printf("Word num %d is invalid\n", wordNum);
                    break;
                }
            }
            wordList.push_back(word);
            if (wordNum == numWords) break;
        }
    }
    return wordList;
}
 
// Driver
int main(int argc, char const* argv[]) {
    
    std::string filepath = "wordlist_100k.txt";
    int maxNumWords = 2000;
    if (argc > 1) filepath = argv[1];
    if (argc > 2) maxNumWords = atoi(argv[2]);
    std::vector<std::string> wordList = loadWords(filepath, maxNumWords);

    // Shuffle the wordlist
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(std::begin(wordList), std::end(wordList), rng);

    // Test basic functionality
    basicTests();

    // Test concurrent functionality
    concurrentTests(wordList);

    printf("Done.\n");

    return 0;

}
