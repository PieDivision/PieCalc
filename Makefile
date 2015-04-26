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
TARGET := bin/piecalc

#### MAIN TARGETS

# Development build
devel: GLADE_PATH := data/
devel: all

# Linux package build
linux: GLADE_PATH := /usr/share/piecalc/
linux: all

# Windows package build
# windows: GLADE_PATH := %ProgramFiles%\PieCalc\ -> I don't think it's needed, needs testing
windows: CC := i686-w64-mingw32-gcc
windows: windows-prebuild all

windows-prebuild: ../gtk+-bundle_3.10.4-20131202_win32.zip
	export PKG_CONFIG_PATH=../gtk/lib/pkgconfig

../gtk+-bundle_3.10.4-20131202_win32.zip:
	mkdir ../gtk ; cd ../gtk ; wget http://win32builder.gnome.org/gtk+-bundle_3.10.4-20131202_win32.zip ; unzip gtk+-bundle_3.10.4-20131202_win32.zip

all: dirs $(TARGET)

dirs:
	mkdir -p bin $(BUILDDIR)

$(TARGET): $(OBJ)
	$(CC) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRC)/%.$(CODE)
	$(CC) $(CFLAGS) -DGLADE_PATH='"$(GLADE_PATH)"' -c -o $@ $< $(LIBS)

pack:
	tar --exclude='run' --exclude='plan' --exclude='$(BINDIR)' --exclude='$(BUILDDIR)' -pczf piecalc-1.0.tar.gz *

clean:
	rm -rf $(BUILDDIR) $(BINDIR) doc || true

test:
	cd tests && make

.PHONY: all dirs pack clean test install debian-package

prefix = /usr/local
bindir = $(prefix)/bin
sharedir = $(prefix)/share
mandir = $(sharedir)/man
man1dir = $(mandir)/man1

install: all
	install bin/piecalc $(DESTDIR)$(bindir)
	install -m 0644 data/piecalc.glade $(DESTDIR)$(bindir)

debian-package:
	./debian-package.sh

run: devel
	bin/piecalc

debug: devel
	ddd bin/piecalc
