all: main

main: main.o linenoise.o interface.o
	gcc -Wall main.o linenoise.o interface.o -o main

interface.o: interface.c
	gcc -Wall -c interface.c

linenoise.o: linenoise.c
	gcc -Wall -c linenoise.c

clean:
	rm -f main *.o
