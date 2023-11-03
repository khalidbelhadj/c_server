CFLAGS="-Wall -Wextra -pedantic"
CC="clang"

main:
	$(CC) $(CFLAGS) main.c -o main
	
