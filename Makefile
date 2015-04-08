# PieCalc Makefile by David Benes
# Written in Vim editor
#
# TODO: Solve dependencies
#

ifndef CC
	CC := gcc
endif

CFLAGS := -std=c99 -Wextra -pedantic -g 
LIBS := -lm
GTK := -export-dynamic `pkg-config --cflags --libs gtk+-3.0`

CODE := c
SRC := src
BUILDDIR := build
SOURCES := $(shell find $(SRC) -type f -name *.$(CODE))
OBJ := $(patsubst $(SRC)/%, $(BUILDDIR)/%, $(SOURCES:.$(CODE)=.o)) 
TARGET := bin/pie_calc

$(TARGET): $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) $(GTK) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRC)/%.$(CODE)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(GTK) -c -o $@ $< $(LIBS)

clean:
	rm -rf build || true
	rm -rf bin || true
