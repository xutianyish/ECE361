#ifndef SERVER_H
#define SERVER_H
#include "util.h"
#include "server.h"
#include "poll.h"
#include "session.h"
#include "message.h"

// parser command line arguments and return the TCP port number
int server_parser(int argc, char** argv);

// login new user
void process_login();

// process new requests
void process_req();



#endif
