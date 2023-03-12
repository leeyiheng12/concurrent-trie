CXX=g++
CXXFLAGS:=-fopenmp

sample: SampleUsage.o
	$(CXX) SampleUsage.cpp ConcurrentTrie.cpp $(CXXFLAGS) -o SampleUsage

SampleUsage.o: SampleUsage.cpp
	$(CXX) $(CXXFLAGS) -c SampleUsage.cpp -o SampleUsage.o

test: TrieTest.o
	$(CXX) TrieTest.cpp SequentialTrie.cpp ConcurrentTrie.cpp $(CXXFLAGS) -o TrieTest

TrieTest.o: TrieTest.cpp
	$(CXX) $(CXXFLAGS) -c TrieTest.cpp -o TrieTest.o

benchmark: benchmark.o
	$(CXX) benchmark.cpp SequentialTrie.cpp ConcurrentTrie.cpp $(CXXFLAGS) -o benchmark

benchmark.o: benchmark.cpp
	$(CXX) $(CXXFLAGS) -c benchmark.cpp -o benchmark.o

clean:
	$(RM) *.o
