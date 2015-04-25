#
# Makefile for PieCalc tests
#

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
LIBRARIES = -lm

SRC=../src
OBJ=piemath_tests.o piemath.o

all: pietests
	./pietests
	rm pietests

pietests: $(OBJ) $(SRC)/piemath.h
	$(CC) -o $@ $(OBJ) $(LIBRARIES)

piemath_tests.o: piemath_tests.c
	$(CC) $(CFLAGS) -c $<

piemath.o: $(SRC)/piemath.c
	$(CC) $(CFLAGS) -c $<