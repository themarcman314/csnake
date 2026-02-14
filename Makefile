.PHONY: clean

CC=gcc -std=c99
BUILDDIR=build
SOURCEDIR=src
SOURCES=$(wildcard $(SOURCEDIR)/*.c)
OBJ=$(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

all: $(BUILDDIR) $(BUILDDIR)/csnake

$(BUILDDIR):
	mkdir build

$(BUILDDIR)/csnake: $(OBJ)
	$(CC) $^ -o $@

$(OBJ): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) -c $< -o $@

clean: $(BUILDDIR)/*.o $(BUILDDIR)/csnake
	rm $(BUILDDIR)/*
