CC=g++

CCFLAGS = -std=c++11
CCFLAGS = -O2
CCFLAGS = -DNDEBUG
CCFLAGS += -fopenmp
CCFLAGS	+= -Wno-narrowing
CCFLAGS += -I$(BOOST_ROOT)

default: main

main: main.cpp
	$(CC) $(CCFLAGS) -o main main.cpp
