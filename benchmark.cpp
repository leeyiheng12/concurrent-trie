#include <algorithm>
#include <chrono>
#include <fstream>
#include <random>
#include <sys/time.h>
#include <unordered_set>
#include <vector>

#include "ConcurrentTrie.h"
#include "SequentialTrie.h"

#define NUM_ITERATIONS 100000


double read_timer() {
  static bool initialized = false;
  static struct timeval start;
  struct timeval end;
  if (!initialized) {
      gettimeofday( &start, NULL );
      initialized = true;
  }
  gettimeofday( &end, NULL );
  return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
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


void time_initialisation() {
    double start_time, end_time;

    printf("\n\n");

    // Time initialisation for SequentialTrie
    start_time = read_timer();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        SequentialTrie* seq_trie = new SequentialTrie();
        delete seq_trie;
    }
    end_time = read_timer();
    printf("[Seq] Average time taken to initialise: %g seconds.\n", (end_time - start_time) / NUM_ITERATIONS);



    // Time initialisation for ConcurrentTrie
    start_time = read_timer();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        ConcurrentTrie* conc_trie = new ConcurrentTrie();
        delete conc_trie;
    }
    end_time = read_timer();
    printf("[Con] Average time taken to initialise: %g seconds.\n", (end_time - start_time) / NUM_ITERATIONS);



    // Time initialisation for HashSet
    start_time = read_timer();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        std::unordered_set<std::string>* hashset = new std::unordered_set<std::string>();
        delete hashset;
    }
    end_time = read_timer();
    printf("[Hash] Average time taken to initialise: %g seconds.\n", (end_time - start_time) / NUM_ITERATIONS);
    
    printf("\n\n");

}

void time_add_single_word(std::vector<std::string> words) {

    std::shared_ptr<SequentialTrie> seq_trie = std::make_shared<SequentialTrie>();
    std::shared_ptr<ConcurrentTrie> conc_trie = std::make_shared<ConcurrentTrie>();
    std::shared_ptr<std::unordered_set<std::string>> hashset = std::make_shared<std::unordered_set<std::string>>();
    double start_time, end_time;
    int numWords = words.size();
    
    printf("\n\n");

    // Time SequentialTrie
    start_time = read_timer();
    for (std::string word : words) seq_trie->insert(word);
    end_time = read_timer();
    printf("[Seq] Average time taken to add a single string: %g seconds.\n", (end_time - start_time) / numWords);

    // Time ConcurrentTrie
    start_time = read_timer();
    for (std::string word : words) conc_trie->insert(word);
    end_time = read_timer();
    printf("[Conc] Average time taken to add a single string: %g seconds.\n", (end_time - start_time) / numWords);

    // Time HashSet
    start_time = read_timer();
    for (std::string word : words) hashset->insert(word);
    end_time = read_timer();
    printf("[Hash] Average time taken to add a single string: %g seconds.\n", (end_time - start_time) / numWords);

    printf("\n\n");

}


void time_add_multiple_words(std::vector<std::string> words) {

    std::shared_ptr<SequentialTrie> seq_trie = std::make_shared<SequentialTrie>();
    std::shared_ptr<std::unordered_set<std::string>> hashset = std::make_shared<std::unordered_set<std::string>>();
    std::shared_ptr<std::unordered_set<std::string>> hashset2 = std::make_shared<std::unordered_set<std::string>>();

    double start_time, end_time;
    int numWords = words.size();

    printf("\n\n");

    // Time SequentialTrie
    start_time = read_timer();
    seq_trie->insert(&words);
    end_time = read_timer();
    printf("[Seq] Time taken to add %d strings: %g seconds.\n", numWords, end_time - start_time);

    // Time ConcurrentTrie for each number of threads
    int minThreads = 3;
    int maxThreads = 6;
    for (int numThreads = minThreads; numThreads <= maxThreads; numThreads++) {

        std::shared_ptr<ConcurrentTrie> conc_trie = std::make_shared<ConcurrentTrie>();
        conc_trie->setNumThreads(numThreads);
        
        start_time = read_timer();
        conc_trie->insert(&words);
        end_time = read_timer();

        printf("[Conc (Threads=%d)] Time taken to add %d strings: %g seconds.\n", numThreads, numWords, end_time - start_time);

    }
    
    std::shared_ptr<ConcurrentTrie> conc_trie_2 = std::make_shared<ConcurrentTrie>();
    start_time = read_timer();
    conc_trie_2->insertAsync(&words);
    end_time = read_timer();
    printf("[Conc Async] Time taken to add %d strings: %g seconds.\n", numWords, end_time - start_time);


    // Time HashSet
    start_time = read_timer();
    hashset->insert(words.begin(), words.end());
    end_time = read_timer();
    printf("[Hash] Time taken to add %d strings (hashmap.insert(vec.begin(), vec.end())): %g seconds.\n", numWords, end_time - start_time);
    
    start_time = read_timer();
    for (std::string word : words) hashset2->insert(word);
    end_time = read_timer();
    printf("[Hash] Time taken to add %d strings (for-loop hashmap.insert(string)): %g seconds.\n", numWords, end_time - start_time);

    printf("\n\n");
}

void time_check_single_string_exists(std::vector<std::string> words) {

    std::shared_ptr<SequentialTrie> seq_trie = std::make_shared<SequentialTrie>();
    std::shared_ptr<ConcurrentTrie> conc_trie = std::make_shared<ConcurrentTrie>();
    std::shared_ptr<std::unordered_set<std::string>> hashset = std::make_shared<std::unordered_set<std::string>>();
    double start_time, end_time;
    int numWords = words.size();

    // Add words to the data structures
    for (std::string word : words) {
        if (rand() % 2 == 0) {
            hashset->insert(word);
            seq_trie->insert(word);
            conc_trie->insert(word);
        }
    }
    
    // Time executions
    start_time = read_timer();
    for (std::string word : words) { hashset->find(word) != hashset->end();}
    end_time = read_timer();
    printf("[Hash] Average time taken to check if a string exists: %g seconds.\n", (end_time - start_time) / numWords);

    start_time = read_timer();
    for (std::string word : words) { seq_trie->contains(word); }
    end_time = read_timer();
    printf("[Seq] Average time taken to check if a string exists: %g seconds.\n", (end_time - start_time) / numWords);

    start_time = read_timer();
    for (std::string word : words) { conc_trie->contains(word); }
    end_time = read_timer();
    printf("[Conc] Average time taken to check if a string exists: %g seconds.\n", (end_time - start_time) / numWords);

    printf("\n\n");
}


void time_check_multiple_strings_exist(std::vector<std::string> words) {

    std::shared_ptr<SequentialTrie> seq_trie = std::make_shared<SequentialTrie>();
    std::shared_ptr<ConcurrentTrie> conc_trie = std::make_shared<ConcurrentTrie>();
    std::shared_ptr<std::unordered_set<std::string>> hashset = std::make_shared<std::unordered_set<std::string>>();
    double start_time, end_time;
    int numWords = words.size();

    std::vector<bool> answer;
    // Add words to the data structures
    for (std::string word : words) {
        if (rand() % 2 == 0) {
            hashset->insert(word);
            seq_trie->insert(word);
            conc_trie->insert(word);
        }
    }

    // Time executions
    start_time = read_timer();
    for (std::string word : words) { answer.push_back(hashset->find(word) != hashset->end()); }
    end_time = read_timer();
    printf("[Hash] Time taken to check if multiple strings exist: %g seconds.\n", (end_time - start_time));

    std::vector<bool> seqResults;
    start_time = read_timer();
    seqResults = seq_trie->contains(&words);
    end_time = read_timer();
    printf("[Seq] Time taken to check if multiple strings exist: %g seconds.\n", (end_time - start_time));

    std::vector<bool> concResults;
    start_time = read_timer();
    concResults = conc_trie->contains(&words);
    end_time = read_timer();
    printf("[Conc] Time taken to check if multiple strings exist: %g seconds.\n", (end_time - start_time));

    for (int i = 0; i < numWords; i++) {
        if (seqResults[i] != answer[i]) {
            printf("ERROR: Seq result %d is not correct!\n", i);
            std::string word = words[i];
            printf("In hashset: %d\n", (hashset->find(word) != hashset->end()) ? 1 : 0);
            printf("In seq trie: %d\n", seq_trie->contains(word) ? 1 : 0);
            printf("In conc trie: %d\n", conc_trie->contains(word) ? 1 : 0);
            printf("Seq result: %d\n", seqResults[i] ? 1 : 0);
            printf("Conc result: %d\n", concResults[i] ? 1 : 0);
            printf("Answer: %d\n", answer[i] ? 1 : 0);
            break;
        }
    }
    printf("=======\n");
    for (int i = 0; i < numWords; i++) {
        if (concResults[i] != answer[i]) {
            printf("ERROR: Conc result %d is not correct!\n", i);
            std::string word = words[i];
            printf("In hashset: %d\n", (hashset->find(word) != hashset->end()) ? 1 : 0);
            printf("In seq trie: %d\n", seq_trie->contains(word) ? 1 : 0);
            printf("In conc trie: %d\n", conc_trie->contains(word) ? 1 : 0);
            printf("Seq result: %d\n", seqResults[i] ? 1 : 0);
            printf("Conc result: %d\n", concResults[i] ? 1 : 0);
            printf("Answer: %d\n", answer[i] ? 1 : 0);
            break;
        }
    }

    printf("\n\n");
}


void time_get_sorted_words(std::vector<std::string> words) {

    std::shared_ptr<SequentialTrie> seq_trie = std::make_shared<SequentialTrie>();
    std::shared_ptr<ConcurrentTrie> conc_trie = std::make_shared<ConcurrentTrie>();
    std::shared_ptr<std::unordered_set<std::string>> hashset = std::make_shared<std::unordered_set<std::string>>();
    double start_time, end_time;
    int numWords = words.size();
    
    seq_trie->insert(&words);
    conc_trie->insert(&words);
    hashset->insert(words.begin(), words.end());
    
    printf("\n\n");

    // Time HashSet
    start_time = read_timer();
    std::vector<std::string> h = std::vector<std::string>(hashset->begin(), hashset->end());
    std::sort(h.begin(), h.end());
    end_time = read_timer();
    printf("[Hash] Time taken to retrive %d strings in sorted order: %g seconds.\n", numWords, end_time - start_time);

    // Time SequentialTrie
    start_time = read_timer();
    std::vector<std::string> s = seq_trie->getAllStringsSorted();
    end_time = read_timer();
    printf("[Seq] Time taken to retrive %d strings in sorted order: %g seconds.\n", numWords, end_time - start_time);

    // Time ConcurrentTrie
    start_time = read_timer();
    std::vector<std::string> c = conc_trie->getAllStringsSorted();
    end_time = read_timer();
    printf("[Conc] Time taken to retrive %d strings in sorted order: %g seconds.\n", numWords, end_time - start_time);

    if (h != s) {
        printf("ERROR: Seq!\n");
    }
    if (h != c) {
        printf("ERROR: Conc!\n");
    }
    if (s != c) {
        printf("ERROR: Seq != Conc!\n");
    }    
    printf("\n\n");

}

void time_get_strings_with_prefix(std::vector<std::string> words, std::string prefix) {

    std::shared_ptr<SequentialTrie> seq_trie = std::make_shared<SequentialTrie>();
    std::shared_ptr<ConcurrentTrie> conc_trie = std::make_shared<ConcurrentTrie>();
    std::shared_ptr<std::unordered_set<std::string>> hashset = std::make_shared<std::unordered_set<std::string>>();
    double start_time, end_time;
    int numWords = words.size();
    
    seq_trie->insert(&words);
    conc_trie->insert(&words);
    hashset->insert(words.begin(), words.end());
    
    printf("\n\n");

    // Time HashSet
    start_time = read_timer();
    std::vector<std::string> h;
    for (std::string word : *hashset) {
        if (word.find(prefix) == 0) {
            h.push_back(word);
        }
    }
    std::sort(h.begin(), h.end());
    end_time = read_timer();
    printf("[Hash] Time taken to retrive %ld strings with prefix %s: %g seconds.\n", h.size(), prefix.c_str(), end_time - start_time);

    // Time SequentialTrie
    start_time = read_timer();
    std::vector<std::string> s = seq_trie->getStringsWithPrefix(prefix);
    end_time = read_timer();
    printf("[Seq] Time taken to retrive %ld strings with prefix %s: %g seconds.\n", s.size(), prefix.c_str(), end_time - start_time);

    // Time ConcurrentTrie
    start_time = read_timer();
    std::vector<std::string> c = conc_trie->getStringsWithPrefix(prefix);
    end_time = read_timer();
    printf("[Conc] Time taken to retrive %ld strings with prefix %s: %g seconds.\n", s.size(), prefix.c_str(), end_time - start_time);

    if (h != s) {
        printf("ERROR: Seq!\n");
    }
    if (h != c) {
        printf("ERROR: Conc!\n");
    }
    if (s != c) {
        printf("ERROR: Seq != Conc!\n");
    }

    
    printf("\n\n");

}



void time_delete_single_word(std::vector<std::string> words) {

    std::shared_ptr<SequentialTrie> seq_trie = std::make_shared<SequentialTrie>();
    std::shared_ptr<ConcurrentTrie> conc_trie = std::make_shared<ConcurrentTrie>();
    std::shared_ptr<std::unordered_set<std::string>> hashset = std::make_shared<std::unordered_set<std::string>>();
    double start_time, end_time;
    int numWords = words.size();
    
    // seq_trie->insert(&words);
    conc_trie->insert(&words);
    hashset->insert(words.begin(), words.end());

    
    printf("\n\n");

    // Time SequentialTrie
    // start_time = read_timer();
    // for (std::string word : words) seq_trie->remove(word);
    // end_time = read_timer();
    // printf("[Seq] Average time taken to remove a single string: %g seconds.\n", (end_time - start_time) / numWords);

    // Time ConcurrentTrie
    start_time = read_timer();
    for (std::string word : words) conc_trie->remove(word);
    end_time = read_timer();
    printf("[Conc] Average time taken to remove a single string: %g seconds.\n", (end_time - start_time) / numWords);

    // Time HashSet
    start_time = read_timer();
    for (std::string word : words) hashset->erase(word);
    end_time = read_timer();
    printf("[Hash] Average time taken to remove a single string: %g seconds.\n", (end_time - start_time) / numWords);

    printf("\n\n");

}

void time_delete_multiple_words(std::vector<std::string> words) {

    std::shared_ptr<SequentialTrie> seq_trie = std::make_shared<SequentialTrie>();
    std::shared_ptr<std::unordered_set<std::string>> hashset = std::make_shared<std::unordered_set<std::string>>();
    double start_time, end_time;
    int numWords = words.size();
    
    seq_trie->insert(&words);
    hashset->insert(words.begin(), words.end());
    
    printf("\n\n");

    // Time SequentialTrie
    start_time = read_timer();
    seq_trie->remove(&words);
    end_time = read_timer();
    printf("[Seq] Time taken to remove %d strings: %g seconds.\n", numWords, end_time - start_time);

    // Time ConcurrentTrie for each number of threads
    // int maxThreads = omp_get_max_threads();
    int minThreads = 4;
    int maxThreads = 12;
    for (int numThreads = minThreads; numThreads <= maxThreads; numThreads += 2) {

        std::shared_ptr<ConcurrentTrie> conc_trie = std::make_shared<ConcurrentTrie>();
        conc_trie->setNumThreads(numThreads);
        conc_trie->insert(&words);
        
        start_time = read_timer();
        conc_trie->remove(&words);
        end_time = read_timer();

        printf("[Conc (Threads=%d)] Time taken to remove %d strings: %g seconds.\n", numThreads, numWords, end_time - start_time);

    }
    
    std::shared_ptr<ConcurrentTrie> conc_trie_2 = std::make_shared<ConcurrentTrie>();
    start_time = read_timer();
    conc_trie_2->removeAsync(&words);
    end_time = read_timer();
    printf("[Conc Async] Time taken to remove %d strings: %g seconds.\n", numWords, end_time - start_time);

    // Time HashSet
    start_time = read_timer();
    for (std::string word : words) hashset->erase(word);
    end_time = read_timer();
    printf("[Hash] Time taken to remove %d strings: %g seconds.\n", numWords, end_time - start_time);


    printf("\n\n");

}


int main(int argc, char const* argv[]) {

    std::string filepath = "wordlist_100k.txt";
    int maxNumWords = 200000;
    if (argc > 1) filepath = argv[1];
    if (argc > 2) maxNumWords = atoi(argv[2]);
    std::vector<std::string> wordList = loadWords(filepath, maxNumWords);

    // Shuffle the wordlist
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(std::begin(wordList), std::end(wordList), rng);

    // printf("Number of words in wordlist: %ld\n", wordList.size());
    // time_initialisation();

    // time_add_single_word(wordList);
    // time_add_multiple_words(wordList);

    // time_check_single_string_exists(wordList);
    // time_check_multiple_strings_exist(wordList);

    // time_get_sorted_words(wordList);
    std::string prefix = "ca";
    if (argc > 3) prefix = argv[3];
    time_get_strings_with_prefix(wordList, prefix);

    // time_delete_single_word(wordList);
    // time_delete_multiple_words(wordList);

    return 0;

}
