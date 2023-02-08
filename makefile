#
# Name: Edgar Navarro
# Username: cssc0831
# Class Info: CS 480
# Assigment: Programming Assignment #2
# Filename: Makefile
#

# Variables to control Makefile operation
CXX =g++
CFLAGS= -g -shared -fPIC

# Targets needed to bring the executable up to date
msh: main.o
	$(CXX) $(CFLAGS) -o msh main.o

# The main.o target can be written more simply
main.o: main.cpp
	$(CXX) $(CFLAGS) -o main.o main.cpp

# Removes all files made from the Makefile
clean:
	rm -f msh *.o