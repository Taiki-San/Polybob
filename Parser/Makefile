CC = g++

BASE_FLAGS  = -fpermissive -std=gnu++11 -O2

# INCLUDE BASE DIRECTORY AND BOOST DIRECTORY FOR HEADERS
LDFLAGS = -I../Libraries/

# FINAL FLAGS -- TO BE USED THROUGHOUT
FLAGS = $(BASE_FLAGS) $(LDFLAGS)

all: clever

clever:
	$(CC) $(FLAGS) -c -o ../obj/catalogue.o catalogue.cpp
	$(CC) $(FLAGS) -c -o ../obj/entity.o entity.cpp
	$(CC) $(FLAGS) -c -o ../obj/interface.o interface.cpp
	$(CC) $(FLAGS) -c -o ../obj/parsingCore.o parsingCore.cpp
	$(CC) $(FLAGS) -c -o ../obj/sanitization.o sanitization.cpp
	$(CC) $(FLAGS) -c -o ../obj/spirit.o spirit.cpp
	$(CC) $(FLAGS) -c -o ../obj/thread.o thread.c
	$(CC) $(FLAGS) -c -o ../obj/toolbox.o toolbox.cpp
