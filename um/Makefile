

############### Variables ###############
# The compiler being used
CC = gcc 

# Include path
IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii \
	 -I/comp/40/build/lib

# Compiler Flags
CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking Flags
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

# Libraries for Linking
LDLIBS = -l40locality -lnetpbm -lcii40 -lm -lrt -L/comp/40/build/lib -larith40

# all the .h files
INCLUDES = $(shell echo *.h)

############### Modules ###############
MODULES = Segment.o Registers.o Mem.o CPU.o

############### Rules ###############

all: um test

## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

## Linking step (.o -> executable program)
um: um.o $(MODULES)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test: test.o $(MODULES)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f um *.o