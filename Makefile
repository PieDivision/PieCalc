# PieCalc Makefile by David Benes

ifndef CC
	CC := gcc
endif

CFLAGS := -std=c99 -Wall -Wextra -pedantic -g 
LIBS := -lm
GTK := `pkg-config --cflags --libs gtk+-3.0`

SOURCE := src
OBJ := build
TARGET := bin/pie_calc

$(TARGET): createDirs $(OBJ)/main.o
	$(CC) $(CFLAGS) $(OBJ)/main.o -o $(TARGET)

$(OBJ)/main.o: $(SOURCE)/main.c
	$(CC) $(CFLAGS) -c $(SOURCE)/main.c -o $(OBJ)/main.o

createDirs:
	mkdir bin build || true

clean:
	rm $(TARGET) || true
	rm $(OBJ)/* || true
	rmdir bin build
