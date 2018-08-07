CXX=gcc
all: rose
rose:
	$(CXX) rose.c -lmemcarve -O2 -o rose -Wall -Wextra -Wpedantic -Werror -std=c99 

clean:
	rm -f rose
