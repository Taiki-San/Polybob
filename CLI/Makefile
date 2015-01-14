all: main

main: main.o linenoise.o interface.o wrapper.o
	g++ -Wall main.o linenoise.o interface.o wrapper.o -o main

main.o: main.c
	g++ -Wall -c main.c

interface.o: interface.c 
	g++ -Wall -c interface.c

linenoise.o: linenoise.c
	gcc -Wall -c linenoise.c

wrapper.o: wrapper.cpp
	g++ -Wall -c wrapper.cpp

clean:
	rm -f main *.o