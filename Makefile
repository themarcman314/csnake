CC=gcc
FLAGS=-I$(INCLUDEDIR) -std=gnu99 -g -D TERM_SIMPLE
LFLAGS=-lm -lraylib
BUILDDIR=build
SOURCEDIR=src
INCLUDEDIR=inc
SOURCES=$(filter-out $(SOURCEDIR)/graphical.c $(SOURCEDIR)/term.c, $(wildcard $(SOURCEDIR)/*.c))
OBJ=$(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

all: $(BUILDDIR) $(BUILDDIR)/csnake_graphical

windows: CC=x86_64-w64-mingw32-gcc
windows: $(BUILDDIR) $(BUILDDIR)/csnake.exe

$(BUILDDIR):
	mkdir build

$(BUILDDIR)/csnake: $(OBJ)
	$(CC) $^ -o $@ $(LFLAGS)

graphical: $(BUILDDIR) $(BUILDDIR)/csnake_graphical
term: $(BUILDDIR) $(BUILDDIR)/csnake_term

$(BUILDDIR)/csnake_graphical: $(OBJ) $(BUILDDIR)/graphical.o
	$(CC) $^ -o $@ $(LFLAGS)

$(BUILDDIR)/csnake_term: $(OBJ) $(BUILDDIR)/term.o
	$(CC) $^ -o $@

$(BUILDDIR)/csnake.exe: $(OBJ)
	$(CC) $^ -o $@

$(OBJ): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) -c $(FLAGS) $< -o $@

$(BUILDDIR)/term.o: $(SOURCEDIR)/term.c
	$(CC) -c $(FLAGS) -D TERM_SIMPLE $< -o $@

$(BUILDDIR)/graphical.o: $(SOURCEDIR)/graphical.c
	$(CC) -c $(FLAGS) $< -o $@


run: $(BUILDDIR)/csnake_graphical
	$(BUILDDIR)/csnake_graphical

clean:
	rm $(BUILDDIR)/*

uml: docs
	plantuml docs/state_machine.uml

docs:
	mkdir docs
