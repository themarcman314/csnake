CC=gcc
FLAGS=-I$(INCLUDEDIR) -std=gnu99 -g
LFLAGS=-lm -lGL -lm -lpthread -ldl -lrt -lX11
BUILDDIR=build
SOURCEDIR=src
INCLUDEDIR=inc
SOURCES=$(filter-out $(SOURCEDIR)/graphical.c $(SOURCEDIR)/term.c, $(wildcard $(SOURCEDIR)/*.c))
OBJ=$(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))
LIB=./libs/libraylib.a

all: $(BUILDDIR) $(BUILDDIR)/csnake_graphical

windows: CC=x86_64-w64-mingw32-gcc
windows: $(BUILDDIR) $(BUILDDIR)/csnake.exe

$(BUILDDIR):
	mkdir build

$(BUILDDIR)/csnake: $(OBJ)
	$(CC) $^ -o $@ $(LFLAGS) 

graphical: FLAGS += -D GRAPHICAL
graphical: $(BUILDDIR) $(BUILDDIR)/csnake_graphical

$(BUILDDIR)/csnake_graphical: $(OBJ) $(BUILDDIR)/graphical.o $(LIB)
	$(CC) $^ -o $@ $(LFLAGS)

$(BUILDDIR)/csnake.exe: $(OBJ)
	$(CC) $^ -o $@

$(OBJ): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) -c $(FLAGS) $< -o $@

$(BUILDDIR)/graphical.o: $(SOURCEDIR)/graphical.c
	$(CC) -c $(FLAGS) $< -o $@


run: FLAGS += -D GRAPHICAL
run: $(BUILDDIR)/csnake_graphical
	$(BUILDDIR)/csnake_graphical

clean:
	rm $(BUILDDIR)/*

uml: docs
	plantuml docs/state_machine.uml

docs:
	mkdir docs
