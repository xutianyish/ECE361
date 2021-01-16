CC=gcc
CFLAGS=-Werror -g
OBJFILES= deliver.o server.o
TARGET=deliver server

default: $(TARGET)

all: $(TARGET)

deliver: deliver.o
	$(CC) $(CFLAGS) -o deliver deliver.o

deliver.o: deliver.c util.h
	$(CC) $(CFLAGS) -c deliver.c

server: server.o
	$(CC) $(CFLAGS) -o server server.o

server.o: server.c util.h
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f $(OBJFILES) $(TARGET)
