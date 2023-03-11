import sys


def is_valid_word(word, smallest_char_num, largest_char_num):
    return all(smallest_char_num <= ord(char) <= largest_char_num for char in word)


def main():

    # Read file name from command line args
    if len(sys.argv) != 5:
        print("Usage: python clean_wordlist.py <file> <smallest_char_num> <largest_char_num> <output_file>")
        sys.exit(1)

    file = open(sys.argv[1], "r", errors="backslashreplace")
    smallest_char_num = int(sys.argv[2])
    largest_char_num = int(sys.argv[3])
    output_file = open(sys.argv[4], "w")

    # Read file line by line, including inverted commas
    print("Starting to read file...")
    for word in file:
        print(word)
        if is_valid_word(word, smallest_char_num, largest_char_num):
            output_file.write(word + "\n")

    file.close()
    output_file.close()

if __name__ == "__main__":
    main()
