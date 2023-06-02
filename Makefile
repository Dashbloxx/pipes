CC = gcc
CFLAGS = -g -I/usr/include/mariadb -I/usr/local/include/wsserver

SRCDIR = source
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

LIBS = -L/usr/local/lib -lcjson -lws -pthread -lmysqlclient -lssl -lcrypto

pipes: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o pipes $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf pipes source/*.o
