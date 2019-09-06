CC=gcc
CFLAGS=-lmemcarve -Wall -Wextra -Wpedantic -Werror -std=c99 -lpthread

all: rose
rose: rose.c

clean:
	rm -f rose
