CC=gcc
# TODO: Remove -g ?
CFLAGS=-g -std=gnu99 -Wall -Wextra -Werror -pedantic

.PHONY=zip clean

proj2: proj2.o
	$(CC) -o $@ $^

proj2.o: proj2.c proj2.h
	$(CC) $(CFLAGS) -c $< -o $@


clear: clean

clean:
	rm -f *.o proj2 xspacpe00.zip

zip:
	zip xspacpe00.zip *.c *.h makefile