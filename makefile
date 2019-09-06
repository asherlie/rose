CXX=gcc
all: rose
rose:
	$(CXX) rose.c -lmemcarve -o rose -Wall -Wextra -Wpedantic -Werror -std=c99 -lpthread

clean:
	rm -f rose
