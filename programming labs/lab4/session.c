#include "session.h"

// create mock database
struct cred* init_database(){
   struct cred* db = NULL;
   db = insert_cred(db, "user1", "user1");
   db = insert_cred(db, "user2", "user2");
   db = insert_cred(db, "user3", "user3");
   return db;
}

// insert a user cred with clientID and password to db
struct cred* insert_cred(struct cred* db, char* clientID, char* password){
   struct cred* curr = db;
   struct cred* prev = NULL;
   while(curr != NULL){
      prev = curr;
      curr = curr->next;
   }

   // insert at head
   if(prev == NULL){
      db = malloc(sizeof(struct cred));
      db->clientID = malloc(strlen(clientID)+1);
      db->password = malloc(strlen(password)+1);
      db->next = NULL;
      strcpy(db->clientID, clientID);
      strcpy(db->password, password);
   } else {
      prev->next = malloc(sizeof(struct cred));
      prev->next->clientID = malloc(strlen(clientID)+1);
      prev->next->password = malloc(strlen(password)+1);
      prev->next->next = NULL;
      strcpy(prev->next->clientID, clientID);
      strcpy(prev->next->password, password);
   }
   
   return db;
}

// print all entries in db
void print_database(struct cred* db){
   struct cred* curr = db;
   printf("Printing all users in database.\n");
   while(curr != NULL){
      printf("clientID:%s, password:%s\n", curr->clientID, curr->password);
      curr = curr->next;
   }
}

// verify user cred
bool verify_cred(struct cred* registered_users, char* clientID, char* password){
   struct cred* curr = registered_users;
   while(curr != NULL){
      if((strcmp(curr->clientID, clientID) == 0) && (strcmp(curr->password, password) == 0)){
         printf("curr->clientID: %s, curr->password: %s\n", curr->clientID, curr->password);
         return true;
      }
      curr = curr->next;
   }
   return false;
}

// find active user
// return the pointer to active_user if found, otherwise return NULL
struct active_user* find_active_user(struct active_user* active_users, char* clientID, char* clientIP){
   struct active_user* curr = active_users;
   while(curr != NULL){
      if(strcmp(clientID, curr->clientID) == 0 || strcmp(clientIP, curr->clientIP) == 0){
         return curr;
      }
      curr = curr->next;
   }
   return NULL;
}

// add active user
struct active_user* add_active_user(struct active_user* active_users, char* clientID, char* clientIP, int port, int sockfd, struct session* curr_session){
   struct active_user* prev = NULL;
   struct active_user* curr = active_users;
   
   while(curr != NULL){
      prev = curr;
      curr = curr->next;
   }

   // insert at head
   if(prev == NULL){
      active_users = malloc(sizeof(struct active_user));
      active_users->clientID = malloc(strlen(clientID)+1);
      active_users->clientIP = malloc(strlen(clientIP)+1);
      strcpy(active_users->clientID, clientID);
      strcpy(active_users->clientIP, clientIP);
      active_users->port = port;
      active_users->sockfd = sockfd;
      active_users->curr_session = curr_session;
      active_users->next = NULL;
   }else{
      prev->next = malloc(sizeof(struct active_user));
      prev->next->clientID = malloc(strlen(clientID)+1);
      prev->next->clientIP = malloc(strlen(clientIP)+1);
      strcpy(prev->next->clientID, clientID);
      strcpy(prev->next->clientIP, clientIP);
      prev->next->port = port;
      prev->next->sockfd = sockfd;
      prev->next->curr_session = curr_session;
      prev->next->next = NULL;
   }

   return active_users;
}


