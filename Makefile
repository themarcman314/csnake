CC=gcc
FLAGS=-I$(INCLUDEDIR) -std=gnu99 -g
BUILDDIR=build
SOURCEDIR=src
INCLUDEDIR=inc
SOURCES=$(wildcard $(SOURCEDIR)/*.c)
OBJ=$(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

all: $(BUILDDIR) $(BUILDDIR)/csnake

windows: CC=x86_64-w64-mingw32-gcc
windows: $(BUILDDIR) $(BUILDDIR)/csnake.exe

$(BUILDDIR):
	mkdir build

$(BUILDDIR)/csnake: $(OBJ)
	$(CC) $^ -o $@

$(BUILDDIR)/csnake.exe: $(OBJ)
	$(CC) $^ -o $@

$(OBJ): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) -c $(FLAGS) $< -o $@

run: all
	$(BUILDDIR)/csnake

clean:
	rm $(BUILDDIR)/*
