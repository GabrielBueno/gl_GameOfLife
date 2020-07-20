CC        = gcc
SDL_FLAGS = -I/usr/include/SDL2 -D_REENTRANT -L/usr/lib -pthread
GLLIBS    = -lglut -lGLEW -lGL -lSDL2
CFLAGS    = -Wall -Wextra
SRC       = ./src/*.c
OUT       = ./bin/gol

all:
	${CC} -o ${OUT} ${SRC} ${SDL_FLAGS} ${GLLIBS}
