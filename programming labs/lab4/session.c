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
