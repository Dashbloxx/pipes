CC = gcc
CFLAGS = -g

SRCDIR = source
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

LIBS = -lwebsockets -pthread

pipes: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o pipes $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf pipes source/*.o