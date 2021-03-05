#ifndef SESSION_H
#define SESSION_H
#include "util.h"

#define MAX_ACTIVEUSER 100

struct cred{
   char* clientID;
   char* password;
   struct cred* next;
};

struct active_user{
   char* clientID;
   struct session* curr_session;
   char* clientIP;
   int port;
   int sockfd;
   
   struct active_user* next;
};

struct session{
   char* sessionID;
   int num_user;

   struct session* next;
   struct active_user* connected_users;
};

// create mock database
struct cred* init_database();
// insert a user cred with clientID and password to db 
struct cred* insert_cred(struct cred* db, char* clientID, char* password);
// print all entries in db
void print_database(struct cred* db);
#endif
