CC = clang++

BASE_FLAGS  = -std=gnu++11 -O2 -c

LDFLAGS = -I../Libraries/

# FINAL FLAGS -- TO BE USED THROUGHOUT
FLAGS = $(BASE_FLAGS) $(LDFLAGS)

all:
	$(CC) $(FLAGS) -o ../obj/catalogue.o catalogue.cpp
	$(CC) $(FLAGS) -o ../obj/entity.o entity.cpp
	$(CC) $(FLAGS) -o ../obj/interface.o interface.cpp
	$(CC) $(FLAGS) -o ../obj/parsingCore.o parsingCore.cpp
	$(CC) $(FLAGS) -o ../obj/sanitization.o sanitization.cpp
	$(CC) $(FLAGS) -o ../obj/spirit.o spirit.cpp
	$(CC) $(FLAGS) -o ../obj/thread.o thread.cpp
	$(CC) $(FLAGS) -o ../obj/toolbox.o toolbox.cpp