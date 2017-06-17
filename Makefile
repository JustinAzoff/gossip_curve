all: main
CFLAGS=-Wall -Wextra $(shell pkg-config --cflags libczmq)
LDFLAGS= $(shell pkg-config --libs libczmq)
main: main.c
