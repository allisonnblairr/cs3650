# Makefile for Lab 5
# CS 3650 Spring 2024

CFLAGS = -Wall -pedantic -g
CC = gcc
EXES = concat_server concat_client

# rules are "target: <dependencies>" followed by zero or more
# lines of actions to create the target
#
all: $(EXES)

# '$^' expands to all the dependencies (i.e. homework.o hw3fuse.o misc.o)
# and $@ expands to the target ('lab4-fuse')
#
concat_server: concat_server.o util.o
	$(CC) -g $^ -o $@

concat_client: concat_client.o util.o
	$(CC) -g $^ -o $@

# action lines have to begin with a **tab**, not a space - for the
# stupid reason why, see:
#  https://beebo.org/haycorn/2015-04-20_tabs-and-makefiles.html

# standard practice is to add a 'clean' target that gets rid of
# build output.
#
clean:
	rm -f *.o $(EXES)
