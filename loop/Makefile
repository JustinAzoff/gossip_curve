all: main allinone
CFLAGS=-Wall -Wextra $(shell pkg-config --cflags libczmq)
LDFLAGS= $(shell pkg-config --libs libczmq)
main: main.c
allinone: allinone.c
