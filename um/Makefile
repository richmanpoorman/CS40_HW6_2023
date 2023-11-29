

############### Variables ###############
# The compiler being used
CC = gcc 

# Include path
IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii \
	 -I/comp/40/build/lib -I/comp/40/build/include

# Compiler Flags
CFLAGS = -g -O1 -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking Flags
LDFLAGS = -g  -L/comp/40/build/lib -L/usr/sup/cii40/lib64
	  

# Libraries for Linking
LDLIBS = -lcii40-O1 -lbitpack -lm -lrt -L/comp/40/build/lib  -lcii40
 	 

# all the .h files
INCLUDES = $(shell echo *.h)

############### Modules ###############
MODULES = Segment.o Memory.o CPU.o

############### Rules ###############

all: um test makeTests

## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

## Linking step (.o -> executable program)
um: um.o $(MODULES)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test: test.o $(MODULES)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

makeTests: runTestMaker.o makeTests.o testsSuite1.o testSuite2.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	
clean:
	rm -f um *.o