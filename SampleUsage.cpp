#include "ConcurrentTrie.h"

#include <fstream>
#include <vector>
#include <unistd.h>
#include <string>
#include <thread>


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


void addString(std::shared_ptr<ConcurrentTrie> trie, std::string* string) {
    trie->insert(*string);
}

void addStrings(std::shared_ptr<ConcurrentTrie> trie, std::vector<std::string>* strings) {
    trie->insert(strings);
}

void addStringsAsync(std::shared_ptr<ConcurrentTrie> trie, std::vector<std::string>* strings) {
    trie->insertAsync(strings);
}




int main(int argc, char const* argv[]) {

    std::vector<std::string> words = {"hello", "world", "this", "is", "a", "test"};
    std::vector<std::string> words2 = {"https://www.facebook.com", "https://www.twitter.com", "https://www.instagram.com" };
    std::vector<std::string> words3 = {"https://www.google.com", "https://www.youtube.com", "https://www.reddit.com" };
    std::string word = "apple";
    std::string word2 = "banana";
    std::string word3 = "orange";
    std::string word4 = "pear";

    // Instantiate trie
    std::shared_ptr<ConcurrentTrie> trieSet = std::make_shared<ConcurrentTrie>();

    // Add words to trie in multiple threads
    std::thread helperThread(addStrings, trieSet, &words);
    printf("Added words to trie in new thread:\n");
    for (std::string word : words) {
        printf("%s\n", word.c_str());
    }
    printf("\n");

    std::thread helperThread2(addStrings, trieSet, &words2);
    printf("Added words to trie in new thread:\n");
    for (std::string word : words2) {
        printf("%s\n", word.c_str());
    }
    printf("\n");

    std::thread helperThread3(addStrings, trieSet, &words3);
    printf("Added words to trie in new thread:\n");
    for (std::string word : words3) {
        printf("%s\n", word.c_str());
    }
    printf("\n");

    std::thread helperThread4(addString, trieSet, &word);
    printf("Added word to trie in new thread: %s\n\n", word.c_str());

    std::thread helperThread5(addString, trieSet, &word2);
    printf("Added word to trie in new thread: %s\n\n", word2.c_str());

    std::thread helperThread6(addString, trieSet, &word3);
    printf("Added word to trie in new thread: %s\n\n", word3.c_str());

    std::thread helperThread7(addString, trieSet, &word4);
    printf("Added word to trie in new thread: %s\n\n", word4.c_str());

    helperThread.detach();
    helperThread2.detach();
    helperThread3.detach();
    helperThread4.detach();
    helperThread5.detach();
    helperThread6.detach();
    helperThread7.detach();

    // Simulate other operations
    usleep(1000000);
    printf("Doing other things...\n\n");
    std::vector<std::string> allWords = words;
    allWords.insert(allWords.end(), words2.begin(), words2.end());
    allWords.insert(allWords.end(), words3.begin(), words3.end());
    allWords.push_back(word);
    allWords.push_back(word2);
    allWords.push_back(word3);
    allWords.push_back(word4);
    
    // Check if words are in trie
    std::vector<bool> contains = trieSet->contains(&allWords);

    // Each of contains should be true
    for (int i = 0; i < contains.size(); i++) {
        if (contains[i]) {
            printf("'%s' is in the trie.\n", allWords[i].c_str());
        } else {
            printf("'%s' should be in the trie, but is not.\n", allWords[i].c_str());
        }
    }
    
    printf("Execution completed.\n");

    return 0;

}
