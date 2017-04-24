EXEC = $(TOP)/indexer


all: indexer.o
	$(CXX) $(CXXFLAGS) -o $(EXEC) indexer.o
	$ rm *.o


indexer.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o indexer.o  
