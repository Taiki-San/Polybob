CC = clang++

BASE_FLAGS  =  -Wall -g

LINK_FLAGS = -llinenoise -LLibraries/

all:
	rm -rf obj/*
	$(MAKE) -C CLI
	$(MAKE) -C Parser
	$(MAKE) -C Maths

	$(CC) $(BASE_FLAGS) -c entrypoint.c -o obj/entrypoint.o

	$(CC) $(LINK_FLAGS) obj/* -o polybob.out

clean:
	rm -rf obj/*