# this is an makefile  example

# CC and CFLAGS are varilables 
CC=gcc -g -Wall
CFLAGS =
# -c option ask g++ to compile the source files, but do not link.
# -g option is for debugging

all: demo

demo: main.c ac.o
	$(CC) -o $@ main.c ac.o

ac.o: ac.c
	$(CC) -o $@ -c ac.c

# clean all the .o and executable files
clean:
	rm -rf *.o demo

