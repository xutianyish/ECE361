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
void logout();

// join the conference session with the given session id
void joinsession();

// leave the current established session
void leavesession();

// create new session
void createsession();

// get the list of connected clients and available sessions
void list();

// terminate the program, close socket, free connected user, remove user from session
void quit();

#endif
