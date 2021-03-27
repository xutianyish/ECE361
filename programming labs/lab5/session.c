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
         return true;
      }
      curr = curr->next;
   }
   return false;
}

// find active user
// return the pointer to active_user if found, otherwise return NULL
struct active_user* find_active_user(struct active_user* active_users, char* clientID){
   struct active_user* curr = active_users;
   while(curr != NULL){
      if(strcmp(clientID, curr->clientID) == 0){
         return curr;
      }
      curr = curr->next;
   }
   return NULL;
}

// add active user
struct active_user* add_active_user(struct active_user* active_users, char* clientID, char* clientIP, int port, int sockfd){
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
      active_users->num_session_joined = 0;
      active_users->inactive_time = 0;
      active_users->next = NULL;
   }else{
      prev->next = malloc(sizeof(struct active_user));
      prev->next->clientID = malloc(strlen(clientID)+1);
      prev->next->clientIP = malloc(strlen(clientIP)+1);
      strcpy(prev->next->clientID, clientID);
      strcpy(prev->next->clientIP, clientIP);
      prev->next->port = port;
      prev->next->sockfd = sockfd;
      prev->next->num_session_joined = 0;
      prev->next->inactive_time = 0;
      prev->next->next = NULL;
   }

   return active_users;
}

// get active users list
void get_active_users(struct active_user* active_users, char* ret){
   struct active_user* curr = active_users;
   char temp[BUFFER_SIZE];
   ret[0] = '\0';

   while(curr != NULL){
      if(curr->num_session_joined == 0){
         sprintf(temp, "\tUser: %s, Current_sessions: N/A\n", curr->clientID);
         strcat(ret, temp);
      }else{
         sprintf(temp, "\tUser: %s, current sessions: \n", curr->clientID);
         strcat(ret, temp);
         for(int i = 0; i < curr->num_session_joined; i++){
            sprintf(temp, "\t\t%d: %s\n", i+1, curr->curr_sessions[i]->sessionID);
            strcat(ret, temp);
         }
      }
      curr = curr->next;
   }
}

// find session with session ID
// return the pointer to the sesion if found, otherwise return NULL
struct session* find_session(struct session* sessions, char* sessionID){
   struct session* curr = sessions;
   while(curr != NULL){
      if(strcmp(curr->sessionID, sessionID) == 0){
         return curr;
      }
      curr = curr->next;
   }
   return NULL;
}

// insert new session into sessions
// and let user join the session
struct session* insert_session(struct session* sessions, char* sessionID, struct active_user* user){
   struct session* prev = NULL;
   struct session* curr = sessions;

   while(curr != NULL){
      prev = curr;
      curr = curr->next;
   }

   if(prev == NULL){
      sessions = malloc(sizeof(struct session));
      sessions->sessionID = malloc(strlen(sessionID)+1);
      strcpy(sessions->sessionID, sessionID);
      sessions->num_user = 1;
      sessions->next = NULL;
      sessions->connected_users[0] = user;
      // update user session to user created the session
      update_user_session(sessions, user);
   }else{
      prev->next = malloc(sizeof(struct session));
      prev->next = malloc(sizeof(struct session));
      prev->next->sessionID = malloc(strlen(sessionID)+1);
      strcpy(prev->next->sessionID, sessionID);
      prev->next->num_user = 1;
      prev->next->next = NULL;
      prev->next->connected_users[0] = user;
      // update user session to user created the session
      update_user_session(prev->next, user);
   }

   return sessions;
}

// insert session to user
void update_user_session(struct session* session, struct active_user* user){
   user->curr_sessions[user->num_session_joined] = session;
   user->num_session_joined++;
}

// check if the user is in a session
int get_session_index(struct active_user* user, char* sessionID){
   for(int i = 0; i < user->num_session_joined; i++){
      if(strcmp(sessionID, user->curr_sessions[i]->sessionID) == 0){
         return i;
      }
   }
   return -1;
}

// update the session with newly joined user
void update_session(struct session* sess, struct active_user* user){
   sess->connected_users[sess->num_user] = user;
   sess->num_user = sess->num_user + 1;
}

// remove session
struct session* remove_session(struct session* sessions, char* sessionID){
   struct session* prev = NULL;
   struct session* curr = sessions;

   while(curr != NULL && strcmp(sessionID, curr->sessionID) != 0){
      prev = curr;
      curr = curr->next;
   }
   
   // remove at head
   if(prev == NULL){
      sessions = sessions->next;
      free(curr->sessionID);
      free(curr);
   }else{
      prev->next = curr->next;
      free(curr->sessionID);
      free(curr);
   }
   return sessions;
}

// remove user with clientID
struct active_user* remove_user(struct active_user* active_users, char* clientID){
   struct active_user* prev = NULL;
   struct active_user* curr = active_users;

   while(curr != NULL && strcmp(clientID, curr->clientID) != 0){
      prev = curr;
      curr = curr->next;
   }

    // remove at head
   if(prev == NULL){
      active_users = active_users->next;
      free(curr->clientID);
      free(curr->clientIP);
      free(curr);
   }else{
      prev->next = curr->next;
      free(curr->clientID);
      free(curr->clientIP);
      free(curr);
   }
   return active_users;
}

// find active_user by sockfd
struct active_user* find_active_user_by_sockfd(struct active_user* active_users, int sockfd){
   struct active_user* curr = active_users;
   while(curr != NULL){
      if(sockfd == curr->sockfd){
         return curr;
      }
      curr = curr->next;
   }

   return NULL;
}

// remove user with clientID from connected users in sess
// decr num_user counter
void remove_user_from_session(struct session* session, char* clientID){
   for(int i = 0; i < session->num_user; i++){
      if(strcmp(session->connected_users[i]->clientID, clientID) == 0){
         session->connected_users[i] = session->connected_users[session->num_user-1];
      }
   }
   session->num_user--;
}
