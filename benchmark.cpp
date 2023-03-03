#include <fstream>
#include <unordered_set>
#include <chrono>
#include <sys/time.h>

#define NUM_WORDS 99902
#define NUM_ITERATIONS 10000000



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




int main(int argc, char const* argv[]) {
    int wordSize;
    double elapsed_secs;
    double start_time, end_time;

    // Initialize array of 10000 words on the heap
    std::string *wordList = new std::string[NUM_WORDS];
    std::string word;


    std::ifstream* wordListFile = new std::ifstream("wordlist.txt");
    if (wordListFile->is_open()) {
        for (int i = 0; i < NUM_WORDS; ++i) {
            *wordListFile >> word;
            wordList[i] = word;
        }
    } else {
        free(wordList);
        free(wordListFile);
        return 1;
    }


    // ======================= Time initialisation =======================
    // Time execution
    start_time = read_timer();

    // Initialise trie NUM_ITERATIONS times
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        struct TrieNode *trie = getNode();
        free(trie);
    }

    // Time execution
    end_time = read_timer();

    // Print time
    printf("Time taken to initialise trie %d times: %g seconds.\n", NUM_ITERATIONS, end_time - start_time);


    // Time execution for hashset
    start_time = read_timer();
    
    // Initialise hashset
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        std::unordered_set<std::string>* hashset = new std::unordered_set<std::string>();
        free(hashset);
    }

    // Time execution
    end_time = read_timer();

    // Print time
    printf("Time taken to initialise hashset %d times: %g seconds.\n", NUM_ITERATIONS, end_time - start_time);

    // ======================= Time adding to set =======================

    struct TrieNode *root = getNode();
    std::unordered_set<std::string> hashset;

    // Generate boolean array of size NUM_WORDS
    int *indices = (int*) malloc(NUM_WORDS * sizeof(int));
    int numWordsAdded;
    for (int i = 0; i < NUM_WORDS; i++) {
        if (rand() % 2 == 0) {
            indices[i] = 1;
            numWordsAdded++;
        } else {
            indices[i] = 0;
        }
    }

    // Time execution
    start_time = read_timer();

    // Add words to trie
    for (int i = 0; i < NUM_WORDS; i++) {
        if (indices[i] == 1) {
            insert(root, wordList[i].c_str());
        }
    }

    // Time execution
    end_time = read_timer();

    // Print time
    printf("Time taken to add %d words to trie: %g seconds.\n", numWordsAdded, end_time - start_time);


    // Time execution for hashset
    start_time = read_timer();

    // Add words to hashset
    for (int i = 0; i < NUM_WORDS; i++) {
        if (indices[i] == 1) {
            hashset.insert(wordList[i]);
        }
    }

    // Time execution
    end_time = read_timer();

    // Print time
    printf("Time taken to add %d words to hashset: %g seconds.\n", numWordsAdded, end_time - start_time);


    // ======================= Time checking for set existence =======================

    std::string wordToCheck;

    // Time execution
    start_time = read_timer();
    
    // Check for existence of words in trie
    for (int i = 0; i < NUM_WORDS; i++) {
        if (indices[1] != search(root, wordList[i].c_str())) {
            printf("Error: word %s not found in trie.\n", wordList[i].c_str());
        }
    }

    free(indices);
    free(wordListFile);
    free(wordList);
    return 0;

}
