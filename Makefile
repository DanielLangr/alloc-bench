CC=g++

CCFLAGS = -std=c++11
CCFLAGS = -O2
CCFLAGS = -DNDEBUG
CCFLAGS += -fopenmp
CCFLAGS	+= -Wno-narrowing
CCFLAGS += -I$(BOOST_ROOT) -DHAVE_BOOST

PROGRAMS = stress

.PHONY: all
all: stress

$(PROGRAMS): % : %.cpp
	$(CC) $(CCFLAGS) -o $@ $<
