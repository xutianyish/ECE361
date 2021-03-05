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
   char* clientIP;
   int port;
   int sockfd;
   struct session* curr_session;

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
// verify user cred
// return true if match and exists, otherwise return false
bool verify_cred(struct cred* registered_users, char* clientID, char* password);

// find active user
// return the pointer to active_user if found, otherwise return NULL
struct active_user* find_active_user(struct active_user* active_users, char* clientID, char* clientIP);

// add active user
struct active_user* add_active_user(struct active_user* active_users, char* clientID, char* clientIP, int port, int sockfd, struct session* curr_session);

 
#endif
