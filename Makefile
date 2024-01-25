#!/usr/bin/make -f

# variable to store the compiler name

CC := gcc

# variable to store the final executable file name

TARGET := debug

# variable to store compilation flags 
# (-Wall: prints all compiler warnings)

CFLAGS  := -Wall -pedantic -g

# If you type "make" without any specific target 
# everything specified after "all" will be built.

all: $(TARGET)

# Rules to create the "debug" executable file.
# It requires debug.o. The next line explains
# commands to create "debug." You can use the
# command "make debug" to trigger the compilation
# of the line below. If "debug.o" does not exist
# in the directory, rules to create "debug.o"
# will be searched and executed.

debug: debug.o
	$(CC) debug.o -o debug 

# Rules to create the "debug.o" object file.
# It requires "debug.c" file. The next line explains
# commands to create "debug.o."

debug.o: debug.c
	$(CC) -c $(CFLAGS) debug.c


# .PHONY: clean indicates that "clean" is just 
# not a target file to build. 
# We can use "make clean" command to delete
# object files and executable files to start
# the compilation fresh.

.PHONY: clean 
clean:
	rm -rf *.o $(TARGETS)
