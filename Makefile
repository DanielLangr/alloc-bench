CC=g++

CCFLAGS = -std=c++11
CCFLAGS += -fopenmp
CCFLAGS	+= -Wno-narrowing
CCFLAGS += -I$(BOOST_ROOT) -DHAVE_BOOST

CCRELEASE_FLAGS = $(CCFLAGS) -O2 -DNDEBUG
CCDEBUG_FLAGS = $(CCFLAGS) -O0 -g

PROGRAMS = glibc ht stress

.PHONY: all
all: $(PROGRAMS) malloc.so

$(PROGRAMS): % : %.cpp
	$(CC) $(CCRELEASE_FLAGS) -o $@ $<
	$(CC) $(CCDEBUG_FLAGS) -o $@-debug $<

malloc.so: malloc.cpp
	g++ -shared -fPIC -fopenmp -o $@ $< -lpthread -ldl
