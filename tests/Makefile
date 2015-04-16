#
# Makefile for PieCalc tests
#

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

SRC=../src
OBJ=piemath_tests.o piemath.o

all: pietests.exe

pietests.exe: $(OBJ) $(SRC)/piemath.h
	$(CC) $(CFLAGS) $(OBJ) -o $@

piemath_tests.o: piemath_tests.c
	$(CC) $(CFLAGS) -c $<

piemath.o: $(SRC)/piemath.c
	$(CC) $(CFLAGS) -c $<