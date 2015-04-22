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

CFLAGS := -std=c99 -Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-3.0`
LIBS := -lm -export-dynamic `pkg-config --libs gtk+-3.0`

CODE := c
SRC := src
BUILDDIR := build
BINDIR := bin
SOURCES := $(shell find $(SRC) -type f -name *.$(CODE))
OBJ := $(patsubst $(SRC)/%, $(BUILDDIR)/%, $(SOURCES:.$(CODE)=.o)) 
TARGET := bin/pie_calc

all: dirs $(TARGET)

dirs:
	mkdir -p bin $(BUILDDIR)

$(TARGET): $(OBJ)
	$(CC) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRC)/%.$(CODE)
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

pack:
	tar --exclude='run' --exclude='plan' --exclude='$(BINDIR)' --exclude='$(BUILDDIR)' -pczf piecalc-1.0.tar.gz *

clean:
	rm -rf $(BUILDDIR) $(BINDIR) doc || true
