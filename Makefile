CC=g++

CCFLAGS = -std=c++14
CCFLAGS += -fopenmp
CCFLAGS	+= -Wno-narrowing
CCFLAGS += -I$(BOOST_ROOT) -DHAVE_BOOST

CCRELEASE_FLAGS = $(CCFLAGS) -O2 -DNDEBUG
CCDEBUG_FLAGS = $(CCFLAGS) -O0 -g

PROGRAMS = glibc stress

.PHONY: all
all: $(PROGRAMS) ht malloc.so

ht: % : %.cpp
	$(CC) $(CCRELEASE_FLAGS) -o $@ $<
	for i in `seq 1 12` ; do $(CC) $(CCRELEASE_FLAGS) -DSBO=$$i -o $@-sbo$$i $< ; done
	$(CC) $(CCRELEASE_FLAGS) -DMP -o $@-mp $<
	$(CC) $(CCRELEASE_FLAGS) -DSBO -DMP -o $@-sbo-mp $<
	for i in `seq 1 12` ; do $(CC) $(CCRELEASE_FLAGS) -DSBO=$$i -DMP -o $@-sbo$$i-mp $< ; done

$(PROGRAMS): % : %.cpp
	$(CC) $(CCRELEASE_FLAGS) -o $@ $<
#	$(CC) $(CCDEBUG_FLAGS) -o $@-debug $<

malloc.so: malloc.cpp
	g++ -shared -fPIC -fopenmp -o $@ $< -lpthread -ldl
