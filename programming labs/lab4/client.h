#ifndef CLIENT_H
#define CLIENT_H
#include "util.h"
#include "message.h"
#include "session.h"
#include "poll.h"

#define MAX_ARG 10

// parse cmdline args and stores results in args and num_arg
void client_parser(char* buff);

// log into the server at the given address and port. The IP address is specified in the string dot format
// return sockfd
int login();

// exit the server
void logout(int sockfd);

// join the conference session with the given session id
void joinsession(int sockfd);

// leave the current established session
void leavesession(int sockfd);

// create new session
void createsession(int sockfd);

// get the list of connected clients and available sessions
void list(int sockfd);

// terminate the program, close socket, free connected user, remove user from session
void quit(int sockfd);

// broadcast message to all users in the same session
void send_message(int sockfd, char* buff);

#endif
