CC=gcc -std=c99
FLAGS=-I$(INCLUDEDIR) 
BUILDDIR=build
SOURCEDIR=src
INCLUDEDIR=inc
SOURCES=$(wildcard $(SOURCEDIR)/*.c)
OBJ=$(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

all: $(BUILDDIR) $(BUILDDIR)/csnake

$(BUILDDIR):
	mkdir build

$(BUILDDIR)/csnake: $(OBJ)
	$(CC) $^ -o $@

$(OBJ): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) -c $(FLAGS) $< -o $@

run: all
	$(BUILDDIR)/csnake

clean:
	rm $(BUILDDIR)/*
