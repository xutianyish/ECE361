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
   struct active_user* connected_users[MAX_ACTIVEUSER];
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
// remove user with clientID from connected users in sess
void remove_user_from_session(struct session* session, char* clientID);

// find active user
// return the pointer to active_user if found, otherwise return NULL
struct active_user* find_active_user(struct active_user* active_users, char* clientID);
// add active user
struct active_user* add_active_user(struct active_user* active_users, char* clientID, char* clientIP, int port, int sockfd, struct session* curr_session);
// get active users list
void get_active_users(struct active_user* active_users, char* ret);
// insert session to user
void update_user_session(struct session* session, struct active_user* user);
// check if the user is in a session
bool is_in_session(struct active_user* active_users, char* clientID);
// remove user with clientID
struct active_user* remove_user(struct active_user* active_users, char* clientID);
// find active_user by sockfd
struct active_user* find_active_user_by_sockfd(struct active_user* active_users, int sockfd);

// find session with session ID
// return the pointer to the sesion if found, otherwise return NULL
struct session* find_session(struct session* sessions, char*sessionID);
// insert new session into sessions
struct session* insert_session(struct session* sessions, char* sessionID, struct active_user* user);
// update session with newly joined user
void update_session(struct session* sess, struct active_user* user);
// remove session
struct session* remove_session(struct session* sessions, char* sessionID);
#endif
