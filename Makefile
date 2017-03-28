export TOP := $(shell pwd)
export UNAME_S := $(shell uname -s)

export CXX = g++
export CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

binaries = crawler 

clean:
	$ rm -f $(binaries)

crawler:
	+$(MAKE) -C src

