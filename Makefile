export TOP := $(shell pwd)
export UNAME_S := $(shell uname -s)

export CXX = g++
export CXXFLAGS = -std=c++14 -Wall -Wextra -pedantic

binaries = crawler 

clean:
	$ rm -f $(binaries)

crawler:
	+$(MAKE) -C src

