CC = clang++

BASE_FLAGS  =  -Wall -g

# INCLUDE BASE DIRECTORY AND BOOST DIRECTORY FOR HEADERS
LDFLAGS = -I/usr/local/Cellar/boost/1.57.0/include -I/opt/local/include

# INCLUDE BASE DIRECTORY AND BOOST DIRECTORY FOR LIB FILES
LLIBFLAGS = -L/usr/local/Cellar/boost/1.57.0/

# FINAL FLAGS -- TO BE USED THROUGHOUT
FLAGS = $(BASE_FLAGS) $(LDFLAGS)
LINKFLAG = $(LLIBFLAGS) $(LINKFLAGS)

all: clever

spirit:
	$(CC) $(FLAGS) -c -o main.o base.cpp
	$(CC) $(FLAGS) -c -o spirit.o spirit.cpp
	$(CC) $(FLAGS) -c -o logic.o sanitization.cpp
	$(CC) $(FLAGS) -c -o toolbox.o toolbox.cpp
	$(CC) $(FLAGS) -c -o parsingCore.o parsingCore.cpp
	$(CC) $(LINKFLAG) -o main.out main.o logic.o toolbox.o spirit.o parsingCore.o

clever:
	$(CC) $(FLAGS) -c -o main.o base.cpp
	$(CC) $(FLAGS) -c -o logic.o sanitization.cpp
	$(CC) $(FLAGS) -c -o toolbox.o toolbox.cpp
	$(CC) $(FLAGS) -c -o parsingCore.o parsingCore.cpp
	$(CC) $(LINKFLAG) -o main.out main.o logic.o toolbox.o spirit.o parsingCore.o
