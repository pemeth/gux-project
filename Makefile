CC = gcc
CFLAGS = -Wall -g -pedantic -std=c11 `pkg-config --cflags ${GTK_PKG}`

NAME = bedit
SRCS = bedit.c bezier.c utils.c
OBJECTS = bedit.o bezier.o utils.o
GTK_PKG = gtk+-3.0

LDLIBS = `pkg-config --libs ${GTK_PKG}`

.PHONY: build clean

all: build

build: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(CFLAGS) -o $(NAME) $(LDLIBS)

bedit.o: bedit.c
	$(CC) bedit.c $(LDFLAGS) $(CFLAGS) -c $(LDLIBS)

bezier.o: bezier.c
	$(CC) bezier.c $(LDFLAGS) $(CFLAGS) -c $(LDLIBS)

utils.o: utils.c
	$(CC) utils.c $(LDFLAGS) $(CFLAGS) -c $(LDLIBS)

clean:
	rm -f $(NAME) $(OBJECTS) bezier

