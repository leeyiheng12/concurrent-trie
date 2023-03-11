#include "ConcurrentTrie.h"

// #include <stdlib.h>
#include <vector>
#include <string>

int main(int argc, char const* argv[]) {

    // ConcurrentTrie t = ConcurrentTrie();
    std::shared_ptr<ConcurrentTrie> t = std::make_shared<ConcurrentTrie>();
    std::vector<std::string> words = {"hello", "world", "this", "is", "a", "test"};
    // printf("Memory location of words: %p\n", &words);
    t->insertAsync(&words);

    printf("Contains hello: %d\n", t->contains("hello"));

    return 0;

}
