#include <fstream>
#include <string>


bool is_valid_word(std::string word, int smallest_char_num, int largest_char_num) {
    for (char c : word) {
        if (int(c) < smallest_char_num || int(c) > largest_char_num) {
            return false;
        }
    }
    return true;
}

int main(int argc, char const* argv[]) {


    std::string file = argv[1];
    int smallest_char_num = atoi(argv[2]);
    int largest_char_num = atoi(argv[3]);
    std::string output_file = argv[4];

    std::ifstream* wordListFile = new std::ifstream(file);
    std::ofstream outfile(output_file);
    std::string word;
    // printf("Starting to read file\n");
    while (std::getline(*wordListFile, word)) {
        // printf("%s\n", word.c_str());
        if (is_valid_word(word, smallest_char_num, largest_char_num)) {
            outfile << word << std::endl;
        }
    }
    delete wordListFile;
    return 0;
}
