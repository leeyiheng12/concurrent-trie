#include <fstream>
#include <string>
#include <vector>

#include "ConcurrentTrie.h"
#include "SequentialTrie.h"


#define IS_TRUE(x) { if (!(x)) printf("%s failed on line %d\n", __FUNCTION__, __LINE__); }
#define IS_FALSE(x) { if ((x)) printf("%s failed on line %d\n", __FUNCTION__, __LINE__); }

// Simple test that tests insertion
void testBasicInsert() {

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

}

// Tests basic deletion - deletion of a word that is a prefix of another word and has a prefix in the trie
void testBasicDelete() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"be", "bet", "beta"};
    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }


    // Delete "bet" - which returns true if the word was in the trie and was successfully removed
    IS_TRUE(sequentialTrie.erase("bet"));
    IS_TRUE(concurrentTrie.erase("bet"));

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
void testBasicDelete2() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"bet", "beta"};
    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }

    // Delete "bet" - which returns true if the word was in the trie and was successfully removed
    IS_TRUE(sequentialTrie.erase("bet"));
    IS_TRUE(concurrentTrie.erase("bet"));

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
void testBasicDelete3() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"be", "bet"};
    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }

    // Delete "bet" - which returns true if the word was in the trie and was successfully removed
    IS_TRUE(sequentialTrie.erase("bet"));
    IS_TRUE(concurrentTrie.erase("bet"));

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
void testBasicDelete4() {

    SequentialTrie sequentialTrie;
    ConcurrentTrie concurrentTrie;

    std::vector<std::string> words = {"a", "bet", "c"};
    for (std::string word : words) {
        sequentialTrie.insert(word);
        concurrentTrie.insert(word);
    }

    // Delete "bet" - which returns true if the word was in the trie and was successfully removed
    IS_TRUE(sequentialTrie.erase("bet"));
    IS_TRUE(concurrentTrie.erase("bet"));

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

    std::vector<std::string> wordsC = concurrentTrie.getWordsWithPrefix("b");
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
    std::vector<std::string> wordsC = concurrentTrie.getAllWordsSorted();
    IS_TRUE(stringsToTest == wordsS);
    IS_TRUE(stringsToTest == wordsC);

}

void basicTests() {
    testBasicInsert();
    testBasicDelete();
    testBasicDelete2();
    testBasicDelete3();
    testBasicDelete4();

    testGetWordsWithPrefix();
    testGetWordsSorted();

    printf("Done.\n");
}

std::vector<std::string> loadWords() {
    std::vector<std::string> wordList;
    std::string word;

    std::ifstream* wordListFile = new std::ifstream("wordlist.txt");
    if (wordListFile->is_open()) {
        while (std::getline(*wordListFile, word)) {
            wordList.push_back(word);
        }
    }
    return wordList;
}
 
// Driver
int main(int argc, char const* argv[]) {
    
    basicTests();

    return 0;

}
