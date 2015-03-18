# PieCalc Makefile by David Benes
# Written in Vim editor
#
# TODO: Time to coffee
#

ifndef CC
	CC := gcc
endif

CFLAGS := -std=c99 -Wall -Wextra -pedantic -g 
LIBS := -lm
GTK := `pkg-config --cflags --libs gtk+-3.0`

CODE := c
SRC := src
BUILDDIR := build
SOURCES := $(shell find $(SRC) -type f -name *.$(CODE))
OBJ := $(patsubst $(SRC)/%, $(BUILDDIR)/%, $(SOURCES:.$(CODE)=.o)) 
TARGET := bin/pie_calc

$(TARGET): $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRC)/%.$(CODE)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

removeDirs:
	rm -rf bin build || true
