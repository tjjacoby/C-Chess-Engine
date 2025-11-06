
all: main

main: main.c binary_printer.h moves.h globals.h
	gcc -Wall main.c -o main

clean:
	rm -f main
