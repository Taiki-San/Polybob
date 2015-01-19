CC = g++

BASE_FLAGS  =  -Wall -g

LINK_FLAGS = -pthread

all:
	rm -rf obj/*
	$(MAKE) -C CLI
	$(MAKE) -C Parser
	$(MAKE) -C Maths

	$(CC) $(BASE_FLAGS) -O2 -c entrypoint.cpp -o obj/entrypoint.o
	gcc $(BASE_FLAGS) -O2 -c Libraries/linenoise.c -o obj/linenoise.o

	$(CC) $(LINK_FLAGS) obj/* -o PolyBob

clean:
	rm -rf obj/*