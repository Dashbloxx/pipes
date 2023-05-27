CC = gcc
CFLAGS = -I/usr/include/mariadb

SRCDIR = source
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

LIBS = -lwebsockets -ljansson -pthread -lmysqlclient -lssl -lcrypto

pipes: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o pipes $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf pipes source/*.o
