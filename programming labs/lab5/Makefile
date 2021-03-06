CC=gcc
CFLAGS=-Werror -g
OBJFILES=client.o server.o message.o session.o poll.o
TARGET=client server

default: $(TARGET)
all: $(TARGET)

poll.o: poll.h poll.c util.h
	$(CC) $(CFLAGS) -c poll.c

message.o: message.h message.c util.h
	$(CC) $(CFLAGS) -c message.c

session.o: session.h session.c util.h
	$(CC) $(CFLAGS) -c session.c

client: client.o message.o session.o poll.o
	$(CC) $(CFLAGS) -o client client.o message.o poll.o session.o

client.o: client.h client.c poll.h session.h message.h util.h
	$(CC) $(CFLAGS) -c client.c

server: server.o message.o session.o poll.o
	$(CC) $(CFLAGS) -o server server.o message.o poll.o session.o

server.o: server.h server.c message.h session.h poll.h util.h
	$(CC) $(CFLAGS) -c server.c

clean: 
	rm -f $(OBJFILES) $(TARGET)
