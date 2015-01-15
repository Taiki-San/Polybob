CC = clang++

BASE_FLAGS  =  -Wall -g

FLAGS = $(BASE_FLAGS)

all: main

main:
	$(CC) $(FLAGS) -c -o ../obj/interface.o interface.c
	$(CC) $(FLAGS) -c -o ../obj/easter.o fluffle.c
	$(CC) $(FLAGS) -c -o ../obj/wrapper.o wrapper.cpp

clean:
	rm -r obj