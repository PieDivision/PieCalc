# PieCalc Makefile by David Benes
# Written in Vim editor
#
# TODO: Solve dependencies
#

CFLAGS := -std=c99 -Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-3.0`
LIBS := -lm -export-dynamic `pkg-config --libs gtk+-3.0`

CODE := c
SRC := src
BUILDDIR := build
BINDIR := bin
SOURCES := $(shell find $(SRC) -type f -name *.$(CODE))
OBJ := $(patsubst $(SRC)/%, $(BUILDDIR)/%, $(SOURCES:.$(CODE)=.o)) 
TARGET := $(BINDIR)/piecalc

#### MAIN RULES

# Development build
devel: GLADE_PATH := data/
devel: all

# Linux package build
linux: GLADE_PATH := /usr/share/piecalc/
linux: clean all

# Windows package build
# windows: GLADE_PATH := %ProgramFiles%\PieCalc\ -> I don't think it's needed, needs testing
windows: CC := i686-w64-mingw32-gcc
windows: export PKG_CONFIG_PATH=gtk/lib/pkgconfig
windows: TARGET := $(TARGET).exe
windows: clean gtk/gtk+-bundle_3.6.4-20130921_win32.zip all

gtk/gtk+-bundle_3.6.4-20130921_win32.zip:
	mkdir gtk || true ;\
	cd gtk ;\
	wget http://win32builder.gnome.org/gtk+-bundle_3.6.4-20130921_win32.zip 2>/dev/null ;\
	unzip gtk+-bundle_3.6.4-20130921_win32.zip > /dev/null ;\
	find -name '*.pc' | while read pc; do sed -e "s@^prefix=.*@prefix=$$PWD@" -i "$$pc"; done

# Rule which runs all tests
test:
	cd tests && make

### DEVELOPMENT RULES

# Run the program, simple
run: devel
	$(TARGET)

debug: devel
	ddd $(TARGET)


#### BUILDING RULES

# Classic all build rule
all: dirs $(TARGET)

# Create directories neede to build
dirs:
	mkdir -p $(BINDIR) $(BUILDDIR)

# Make target from object files
$(TARGET): $(OBJ)
	$(CC) $^ -o $(TARGET) $(LIBS)

# General rule how to make object file from source file
$(BUILDDIR)/%.o: $(SRC)/%.$(CODE)
	$(CC) $(CFLAGS) -DGLADE_PATH='"$(GLADE_PATH)"' -c -o $@ $<

# Pack target
pack:
	tar --exclude='run' --exclude='plan' --exclude='$(BINDIR)' --exclude='$(BUILDDIR)' -pczf piecalc-1.0.tar.gz *

# Clean target
clean:
	rm -rf $(BUILDDIR) $(BINDIR) doc || true

# Phony
.PHONY: all dirs pack clean test install debian-package

prefix = /usr
bindir = $(prefix)/bin
sharedir = $(prefix)/share
mandir = $(sharedir)/man
man1dir = $(mandir)/man1

install:
	mkdir -p $(DESTDIR)$(sharedir)/piecalc
	install bin/piecalc $(DESTDIR)$(bindir)
	install -m 0644 data/piecalc.glade $(DESTDIR)$(sharedir)/piecalc

	gzip -k man/piecalc.1
	install -m 0644 man/piecalc.1 $(DESTDIR)$(man1dir)
	rm man/piecalc.1.gz

debian-package:
	./debian-package.sh

remove:
	rm -rf /usr/bin/piecalc /usr/share/piecalc /usr/share/man/man1/piecalc.1.gz
