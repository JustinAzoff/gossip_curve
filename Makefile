all: main server client
CFLAGS=-Wall -Wextra $(shell pkg-config --cflags libczmq)
LDFLAGS= $(shell pkg-config --libs libczmq)
main: main.c
server: server.c
client: client.c
