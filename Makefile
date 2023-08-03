CC = gcc
CFLAGS = -Wall -Wextra -pedantic

all: main

main: main.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f main