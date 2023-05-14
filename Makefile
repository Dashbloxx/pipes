CC = gcc
CFLAGS =

SRCDIR = source
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

LIBS = -lwebsockets

pipes: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o pipes $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
