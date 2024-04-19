CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
LDFLAGS=-pthread

.PHONY=zip clean clean all

source_files = memory.c processes.c semaphores.c util.c proj2.c
obj_files = $(source_files:.c=.o)

all: proj2

proj2: $(obj_files)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

proj2.o: proj2.c
	$(CC) $(CFLAGS) -c $< -o $@

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c $< -o $@

processes.o: processes.c processes.h
	$(CC) $(CFLAGS) -c $< -o $@

semaphores.o: semaphores.c semaphores.h
	$(CC) $(CFLAGS) -c $< -o $@

util.o: util.c util.h
	$(CC) $(CFLAGS) -c $< -o $@

clear: clean

clean:
	rm -f *.o proj2 *.zip

zip:
	zip proj2.zip *.c *.h Makefile
