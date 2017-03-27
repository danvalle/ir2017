EXEC = $(TOP)/crawler


all: crawler.o
	$(CXX) $(CXXFLAGS) -o $(EXEC) crawler.o $(TOP)/lib/chilkat-9.5.0-x86_64-linux-gcc/lib/libchilkat-9.5.0.so -lresolv -lpthread
	$ rm *.o


crawler.o: main.cpp
	$(CXX) $(CXXFLAGS) -I$(TOP)/lib/chilkat-9.5.0-x86_64-linux-gcc/include/ -c main.cpp -o crawler.o  


