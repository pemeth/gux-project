CC = gcc
CFLAGS = -Wall -g

NAME = bedit
SRCS = $(wildcard *.c)
OBJECTS = $(SRCS:.c=.o)
GTK_PKG = gtk+-3.0

# This is used ONLY with the `bezier` target.
# In case I want to debug/test the bezier linked list,
# then I can just call `make bezier` and an executable is created
# with its own main function.
# If for whatever reason I need to keep in the GTK definitions
# and cannot have the main function included,
# use `make bezier NO_DEBUG=1`
ifeq ($(origin NO_DEBUG), undefined)
	DEBUG_OPTIONS = -DDEBUG -o $@
else
	GTK = `pkg-config ${GTK_PKG} --cflags --libs`
	GEN_OBJ = -c
endif

.PHONY: build clean

all: build

build: $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $(NAME) $^ `pkg-config ${GTK_PKG} --cflags --libs`

%.o: %.c
	$(CC) $(LDFLAGS) $(CFLAGS) -c $^ `pkg-config ${GTK_PKG} --cflags --libs`

bezier: bezier.c
	$(CC) $(LDFLAGS) $(CFLAGS) $(DEBUG_OPTIONS) $(GEN_OBJ) $^ $(GTK)

clean:
	rm -f $(NAME) $(OBJECTS) bezier

