BUILDDIR_NATIVE=build/native
BUILDDIR_WEB=build/web

CC=gcc
FLAGS=-I$(INCLUDEDIR) -std=gnu99
LFLAGS=-lm -lGL -lm -lpthread -ldl -lrt -lX11
LIB=./libs/libraylib.a
TARGET=csnake

WEB_CC=emcc
WEB_FLAGS = $(FLAGS)
WEB_FLAGS += -DPLATFORM_WEB
WEB_LFLAGS = --shell-file src/shell.html -s USE_GLFW=3 -s ASYNCIFY  -sSTACK_SIZE=100000 --preload-file sounds --preload-file persistent -sALLOW_MEMORY_GROWTH  #FIXME remove memory growth ?
WEB_LIB=./libs/libraylib_web.a
TARGET_WEB=csnake.html
PORT = 8000

SOURCEDIR=src
INCLUDEDIR=inc
OBJ_NATIVE=$(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR_NATIVE)/%.o, $(SOURCES))
OBJ_WEB=$(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR_WEB)/%.o, $(SOURCES))

SOURCES=$(wildcard $(SOURCEDIR)/*.c)

all: $(BUILDDIR_NATIVE) $(BUILDDIR_NATIVE)/$(TARGET)

web: $(BUILDDIR_WEB) $(BUILDDIR_WEB)/$(TARGET_WEB)

$(BUILDDIR_NATIVE) $(BUILDDIR_WEB):
	mkdir -p $@

$(BUILDDIR_NATIVE)/$(TARGET): $(OBJ_NATIVE)
	$(CC) $^ -o $@ $(LIB) $(LFLAGS)

$(BUILDDIR_WEB)/$(TARGET_WEB): $(OBJ_WEB)
	$(WEB_CC) $^ -o $@ $(WEB_LIB) $(WEB_LFLAGS)

$(OBJ_NATIVE): $(BUILDDIR_NATIVE)/%.o: $(SOURCEDIR)/%.c
	$(CC) -c $(FLAGS) $< -o $@

$(OBJ_WEB): $(BUILDDIR_WEB)/%.o: $(SOURCEDIR)/%.c
	$(WEB_CC) -c $(WEB_FLAGS) $< -o $@

run: all
	$(BUILDDIR_NATIVE)/$(TARGET)

runweb: $(BUILDDIR_WEB)/$(TARGET_WEB)
	@echo "Starting server at http://localhost:$(PORT)"
	@sleep 5 && firefox --new-window http://localhost:$(PORT)/$(TARGET_WEB) &
	python -m http.server $(PORT) -d $(BUILDDIR_WEB)

clean:
	rm -rf build

uml: docs
	plantuml docs/state_machine.uml

docs:
	mkdir docs
