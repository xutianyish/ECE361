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
void process_request(int sockfd);

// handle redundant login
void re_login(int sockfd, char* source);

// create session
void create_session(int sockfd, struct message* msg);

// join session
void join_session(int sockfd, struct message* msg);

// leave_session
void leave_session(int sockfd, struct message* msg);

// broadcast
void broadcast(int sockfd, struct message* msg);

// send user and session list
void send_user_session_list(int sockfd, char* source);

// logout user
void logout_user(int sockfd, char* source);

#endif
