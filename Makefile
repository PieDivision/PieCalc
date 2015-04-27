# PieCalc Makefile by David Benes
#
# Basic overview:
# make (alias make devel) - makes project for debugging
# make linux - makes linux release (requires GTK3 devel packages installed)
# make windows - makes windows release (requires MingW64 - dependencies are auto downloaded)
# make test - tests math library
# make run - runs the program (if not built, it build run make devel)
# make debug - runs the program in ddd debug enviroment
# make clean - cleans the building mess
# make install - installs the package to system
# make remove - removes the package from system
# make debian-package - makes deb package (requires debhelper and devscripts)
# make rpm-package - makes rpm package (requires rpm build package)
# make windows-package - makes windows installer (requires mingw32-nsis)
# make doc - makes doc
#

CFLAGS := -std=c99 -Wall -Wextra -pedantic -O2 `pkg-config --cflags gtk+-3.0`
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
devel: CFLAGS += -g
devel: all

# Linux package build
linux: GLADE_PATH := /usr/share/piecalc/
linux: clean all

# Windows package build
# windows: GLADE_PATH := %ProgramFiles%\PieCalc\ -> I don't think it's needed, needs testing
windows: CC := i686-w64-mingw32-gcc
windows: export PKG_CONFIG_PATH=gtk/lib/pkgconfig
windows: TARGET := $(TARGET).exe
windows: LIBS += -mwindows
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

# Clean target
clean:
	rm -rf $(BUILDDIR) $(BINDIR) docs || true

prefix = /usr
bindir = $(prefix)/bin
sharedir = $(prefix)/share
mandir = $(sharedir)/man
man1dir = $(mandir)/man1

install:
	mkdir -p $(DESTDIR)$(man1dir)
	mkdir -p $(DESTDIR)$(sharedir)/piecalc
	mkdir -p $(DESTDIR)$(sharedir)/applications
	mkdir -p $(DESTDIR)$(bindir)
	install bin/piecalc $(DESTDIR)$(bindir)
	install -m 0644 data/piecalc.glade $(DESTDIR)$(sharedir)/piecalc
	install -m 0644 data/icon_*.png $(DESTDIR)$(sharedir)/piecalc
	install -m 0644 data/pie_calc.desktop $(DESTDIR)$(sharedir)/applications

	gzip -k man/piecalc.1
	install -m 0644 man/piecalc.1.gz $(DESTDIR)$(man1dir)
	rm man/piecalc.1.gz

debian-package: linux
	./debian-package.sh

windows-package: windows
	makensis windows_install.nsi

rpm-package:
	./rpm-package.sh

remove:
	rm -rf /usr/bin/piecalc /usr/share/piecalc /usr/share/man/man1/piecalc.1.gz /usr/share/applications/pie_calc.desktop

doc:
	doxygen

#### DEVELOPMENT RULES

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

# Phony
.PHONY: devel linux windows test clean install debian-package rpm-package windows-package remove doc all dirs
