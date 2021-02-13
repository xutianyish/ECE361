CC=gcc
CFLAGS=-Werror -g
OBJFILES= deliver.o server.o packet.o
TARGET=deliver server

default: $(TARGET)

all: $(TARGET)

packet.o: packet.c packet.h util.h
	$(CC) $(CFLAGS) -c packet.c

deliver: deliver.o packet.o
	$(CC) $(CFLAGS) -o deliver deliver.o packet.o

deliver.o: deliver.c util.h packet.h
	$(CC) $(CFLAGS) -c deliver.c 

server: server.o packet.o
	$(CC) $(CFLAGS) -o server server.o packet.o

server.o: server.c util.h packet.h
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f $(OBJFILES) $(TARGET)
