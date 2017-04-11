EXEC = $(TOP)/crawler


all: crawler.o outbound.o indomain.o utils.o
	$(CXX) $(CXXFLAGS) -o $(EXEC) crawler.o outbound.o indomain.o utils.o $(TOP)/lib/chilkat-9.5.0-x86_64-linux-gcc/lib/libchilkat-9.5.0.so -lresolv -lpthread
	$ rm *.o


crawler.o: main.cpp
	$(CXX) $(CXXFLAGS) -I$(TOP)/lib/chilkat-9.5.0-x86_64-linux-gcc/include/ -c main.cpp -o crawler.o  

outbound.o: outbound.cpp
	$(CXX) $(CXXFLAGS) -I$(TOP)/lib/chilkat-9.5.0-x86_64-linux-gcc/include/ -c outbound.cpp -o outbound.o

indomain.o: indomain.cpp
	$(CXX) $(CXXFLAGS) -I$(TOP)/lib/chilkat-9.5.0-x86_64-linux-gcc/include/ -c indomain.cpp -o indomain.o

utils.o: utils.cpp
	$(CXX) $(CXXFLAGS) -I$(TOP)/lib/chilkat-9.5.0-x86_64-linux-gcc/include/ -c utils.cpp -o utils.o