BUILDDIR_NATIVE=build/native
BUILDDIR_WEB=build/web
NGINX_DIR=/usr/share/nginx/html/

CC=gcc
FLAGS=-I$(INCLUDEDIR) -std=c99
LFLAGS=-lm -lGL -lm -lpthread -ldl -lrt -lX11
LIB=./libs/libraylib.a
TARGET=csnake

WEB_CC=emcc
WEB_FLAGS = $(FLAGS)
WEB_FLAGS += -DPLATFORM_WEB
WEB_LFLAGS = --shell-file src/shell.html -s USE_GLFW=3 -s ASYNCIFY -s STACK_SIZE=1MB --preload-file sounds --preload-file flags/flags_strip.png -sINITIAL_MEMORY=67108864 -sFETCH
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

#run: runweb

.PHONY:server

prod:
	rsync $(BUILDDIR_WEB)/* root@marcrobison.com:/var/www/my_site/

server: server/server.c
	$(CC) $< -o $(BUILDDIR_WEB)/server -lcjson


runweb: web server
	systemctl stop snake_server
	cp $(BUILDDIR_WEB)/* $(NGINX_DIR)
	systemctl start snake_server
	firefox --new-window http://localhost:/$(TARGET_WEB)

clean:
	rm -rf build

uml: docs
	plantuml docs/state_machine.uml

docs:
	mkdir docs
