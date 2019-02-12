CC=g++

CCFLAGS = -std=c++11
CCFLAGS = -O2
CCFLAGS = -DNDEBUG
CCFLAGS += -fopenmp
CCFLAGS	+= -Wno-narrowing
CCFLAGS += -I$(BOOST_ROOT) -DHAVE_BOOST

PROGRAMS = glibc ht stress

.PHONY: all
all: $(PROGRAMS)

$(PROGRAMS): % : %.cpp
	$(CC) $(CCFLAGS) -o $@ $<
