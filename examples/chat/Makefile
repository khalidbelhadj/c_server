CC=cc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -g
LDFLAGS=-I../../lib/c_utils/include/ -L../../lib/c_utils/build/ -lutils

main: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lhttpserver -L../../build/ -o main main.c

clean:
	rm -f main
