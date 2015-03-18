# PieCalc Makefile by David Benes

ifndev CC
	CC = gcc
endif

CFLAG = -std=c99 -Wall -Wextra -pedantic -g 
LIBS = -lm
GTK = `pkg-config --cflags --libs gtk+-3.0`


