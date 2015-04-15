# PieCalc Makefile by David Benes
# Written in Vim editor
#
# TODO: Solve dependencies
#

ifeq ($(platform),win32)
	CC := i686-w64-mingw32-gcc
	export PKG_CONFIG_PATH=../gtk/lib/pkgconfig
endif

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

all: dirs $(TARGET)

dirs:
	mkdir -p bin $(BUILDDIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LIBS) $(GTK)

$(BUILDDIR)/%.o: $(SRC)/%.$(CODE)
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS) $(GTK)

pack:
	tar --exclude='run' --exclude='plan' --exclude='bin' --exclude='build' -pczf piecalc-1.0.tar.gz *

clean:
	rm -rf build || true
	rm -rf bin || true
