#makefile for compiling the utf8 project
#Author: Kirill Akimov

CC = g++
FLAGS = -std=c++17

.PHONY: all clean

all: utf8

clean:
	rm -rf test *.o

utf_8.o:utf_8.cpp
	$(CC) $(FLAGS) -c -o utf_8.o utf_8.cpp

test.o: test.cpp
	$(CC) $(FLAGS) -c -o test.o test.cpp

utf8: utf_8.o test.o
	$(CC) $(FLAGS) -o test utf_8.o test.o

test: utf8
	./test

