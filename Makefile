CC = g++

BASE_FLAGS  =  -Wall -O2

LINK_FLAGS = -pthread

all:
	rm -rf obj
	mkdir obj
	$(MAKE) -C CLI
	$(MAKE) -C Parser
	$(MAKE) -C Maths

	$(CC) $(BASE_FLAGS) -O2 -c entrypoint.cpp -o entrypoint_tmp.o
	$(CC) $(FLAGS) -c -o faulhaber_tmp.o faulhaber.cpp
	gcc $(BASE_FLAGS) -O2 -c Libraries/linenoise.c -o obj/linenoise.o

	$(CC) $(LINK_FLAGS) obj/* entrypoint_tmp.o -o PolyBob
	$(CC) $(LINK_FLAGS) obj/* faulhaber_tmp.o -o Faulhaber

	rm entrypoint_tmp.o faulhaber_tmp.o

clean:
	rm -rf obj/*