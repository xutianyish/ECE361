#include "server.h"
// reference: https://www.geeksforgeeks.org/socket-programming-cc/

// session globals
struct cred* registered_users;
struct session* sessions;
struct active_user* active_users;

// poll globals
static struct pollfd pfds[MAX_ACTIVEUSER+1];
static nfds_t num_poll;

int main(int argc, char** argv){
   // init
   int portid = server_parser(argc, argv);
   registered_users = init_database();
   sessions = NULL;
   active_users = NULL;
   num_poll = 0;

   // create a new TCP socket 
   int welcome_sock = socket(AF_INET, SOCK_STREAM, 0);
   if(welcome_sock == -1){
      perror("Error: failed to create a new socket.\n");
      exit(EXIT_FAILURE);
   }

   // setsockopt to allow socket reuse
   int yes = 1;
   if(setsockopt(welcome_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
      perror("Error: failed to setsocketopt for socket reuse.\n");
      exit(EXIT_FAILURE);
   }

   // init server addr info
   struct sockaddr_in serveraddr_in;
   serveraddr_in.sin_family = AF_INET;
   serveraddr_in.sin_port = htons(portid);
   serveraddr_in.sin_addr.s_addr = INADDR_ANY;

   // bind socket with server ip address and port number
   if(bind(welcome_sock, (struct sockaddr*)&serveraddr_in, sizeof(serveraddr_in)) != 0){
      perror("Error: failed to bind welcome socket with server port.\n");
      exit(EXIT_FAILURE);
   }
   
   // TODO: do we need listen here? if we do polling
   listen(welcome_sock, BACKLOG);
   num_poll = add_poll(pfds, num_poll, welcome_sock);

   // server loop
   while(true){
      clear_poll(pfds, num_poll);
      int ret = poll(pfds, num_poll, INF_TIMEOUT);

      for(int i = 0; i < num_poll; i++){
         if(pfds[i].revents & (POLLIN | POLLHUP)){
            if(i == 0){
               process_login();
            }else{
               if(pfds[i].revents & POLLIN){
                  process_request(pfds[i].fd);
               }
            }
         }
      }
   }

   return(0);
}

// process request
void process_request(int sockfd){
   struct message msg;
   if(m_receive(sockfd, &msg) == 0){
      struct active_user* user = find_active_user_by_sockfd(active_users, sockfd);
      printf("Client %s closed the connection abruptly. Logging off automatically...\n", user->clientID);
      logout_user(sockfd, user->clientID);
   }

   if(msg.type == LOGIN){
      re_login(sockfd, msg.source);
   }
   else if(msg.type == EXIT){
      logout_user(sockfd, msg.source);
   }
   else if(msg.type == JOIN){
      join_session(sockfd, &msg);
   }
   else if(msg.type == NEW_SESS){
      create_session(sockfd, &msg);
   }
   else if(msg.type == LEAVE_SESS){
      leave_session(sockfd, &msg);
   }
   else if(msg.type == MESSAGE){
      broadcast(sockfd, &msg);
   }
   else if(msg.type == QUERY){
      send_user_session_list(sockfd, msg.source);
   }

}

// login new user
void process_login(){
   int welcome_sockfd = pfds[0].fd;
   struct sockaddr_in clientaddr_in;
   int clientaddr_in_len = sizeof(clientaddr_in);
   int connection_sock = accept(welcome_sockfd, (struct sockaddr*)&clientaddr_in, &clientaddr_in_len);
   
   // get client port and ip
   char* client_ip = inet_ntoa((&clientaddr_in)->sin_addr);
   int client_port = ntohs((&clientaddr_in)->sin_port);

   // receive message from user
   struct message msg, reply;
   m_receive(connection_sock, &msg);
   
   char* clientID = strtok(msg.data, ",");
   char* password = strtok(NULL, ",");
   while(strtok(NULL, ","))
      ;


   // check cred, refuse login if cred does not match
   if(!verify_cred(registered_users, clientID, password)){
      reply.type = LO_NAK;
      reply.size = sprintf(reply.data, "%s", "Username does not exist or incorrect Password.");
      m_send(connection_sock, &reply);
      close(connection_sock);
      return;
   }

   // refuse login if client already logged in
   struct active_user* user = find_active_user(active_users, clientID);
   if(user != NULL){
      reply.type = LO_NAK;
      reply.size = sprintf(reply.data, "User:%s already logged in.", clientID);
      m_send(connection_sock, &reply);
      close(connection_sock);
      return;
   }
   
   // send reply message to client 
   reply.type = LO_ACK;
   reply.size = 0;
   m_send(connection_sock, &reply);

   // add active user to list
   active_users = add_active_user(active_users, clientID, client_ip, client_port, connection_sock, NULL);
   num_poll = add_poll(pfds, num_poll, connection_sock);

   printf("-----------------------------------------------\n");
   printf("New Client Connected.\n");
   printf("ClienID: %s\n", clientID);
   printf("IP address: %s\n", client_ip);
   printf("Port Number: %d\n", client_port);
   printf("-----------------------------------------------\n");
}

// parser command line arguments and return the TCP port number
int server_parser(int argc, char** argv){
   if(argc != 2){
      printf("The number of arguments is incorrect.\n");
      printf("The command input should be as follow: server <TCP port number to listen on>.\n");
      exit(1);
   }
  
   int portid = atoi(argv[1]);
   if(portid < 1024 || portid > 65536){
      printf("Error: Port number must be in range 1024~65535\n");
      exit(1);
   }

   return portid;
}

// handle redundant login
void re_login(int sockfd, char* source){
   struct message reply;
   reply.type = LO_NAK;
   reply.size = sprintf(reply.data, "User already logged in as %s", source);
   strcmp(reply.source, source);
   m_send(sockfd, &reply);
}

// create new session
void create_session(int sockfd, struct message* msg){
   struct message reply; 
   // check if session already exist
   if(find_session(sessions, msg->data) != NULL){
      reply.type = NS_NAK;
      strcpy(reply.source, msg->source);
      reply.size = sprintf(reply.data, "%s", "session already exist.");
      m_send(sockfd, &reply);
      return;
   }

   // init session and insert
   struct active_user* user = find_active_user(active_users, msg->source);
   if(user == NULL){
      reply.type = NS_NAK;
      strcpy(reply.source, msg->source);
      reply.size = sprintf(reply.data, "%s", "user does not exist.");
      m_send(sockfd, &reply);
      return;
   }

   sessions = insert_session(sessions, msg->data, user);
   reply.type = NS_ACK;
   strcpy(reply.source, msg->source);
   reply.size = sprintf(reply.data, "%s", msg->data);
   m_send(sockfd, &reply);
   
   printf("-----------------------------------------------\n");
   printf("New session created.\n");
   printf("Session ID: %s\n", msg->data);
   printf("-----------------------------------------------\n");
}

// join session
void join_session(int sockfd, struct message* msg){
   struct message reply;
   // check if session exist 
   struct session* sess = find_session(sessions, msg->data);
   if(sess == NULL){
      reply.type = JN_NAK;
      reply.size = sprintf(reply.data, "%s,Session ID does not exist.", msg->data);
      strcpy(reply.source, msg->source);
      m_send(sockfd, &reply);
      return;
   }

   // check if user already join a session
   if(is_in_session(active_users, msg->source)){
      reply.type = JN_NAK;
      reply.size = sprintf(reply.data, "%s,The user is already is a session.", msg->data);
      strcpy(reply.source, msg->source);
      m_send(sockfd, &reply);
      return;
   }

   // if exist reply with ACK and update new session
   struct active_user* user = find_active_user(active_users, msg->source);
   user->curr_session = sess;
   update_session(sess, user);
   reply.type = JN_ACK;
   reply.size = sprintf(reply.data, "%s", msg->data);
   strcpy(reply.source, msg->source);
   m_send(sockfd, &reply);

   // print info
   printf("-----------------------------------------------\n");
   printf("User %s joined session %s.\n", msg->source, msg->data);
   printf("-----------------------------------------------\n");
}

// leave_session
void leave_session(int sockfd, struct message* msg){
   // check if the client joined a session
   struct active_user* user = find_active_user(active_users, msg->source);
   char sessionID[BUFFER_SIZE];
   if(user != NULL && user->curr_session != NULL){
      remove_user_from_session(user->curr_session, msg->source);
      strcpy(sessionID, user->curr_session->sessionID);
      if(user->curr_session->num_user == 0){
         sessions = remove_session(sessions, user->curr_session->sessionID);
      }
   }
   
   // set curr_session ptr to NULL
   user->curr_session = NULL;
   // print info
   printf("-----------------------------------------------\n");
   printf("User %s left session %s.\n", msg->source, sessionID);
   printf("-----------------------------------------------\n");
}

// broadcast
void broadcast(int sockfd, struct message* msg){
   printf("-----------------------------------------------\n");
   printf("broadcasting message:%s\n", msg->data);
   
   struct active_user* user = find_active_user(active_users, msg->source);
   if(user->curr_session == NULL){
      printf("Error: the user does not have an active session.\n");
      struct message reply;
      reply.type = ERR;
      reply.size = sprintf(reply.data, "%s", "Error: the user does not have an active session. Please join a session.");
      strcpy(reply.source, msg->source);
      m_send(sockfd, &reply);
      return;
   }
   for(int i = 0; i < user->curr_session->num_user; i++){
      int client_sockfd = user->curr_session->connected_users[i]->sockfd;
      if(sockfd != client_sockfd){
         struct message reply;
         reply.type = MESSAGE;
         reply.size = sprintf(reply.data, "%s", msg->data);
         strcpy(reply.source, msg->source);
         m_send(client_sockfd, &reply);
      }
   }
   printf("broadcasting finish!\n");
   printf("-----------------------------------------------\n");
}

// send user and session list
void send_user_session_list(int sockfd, char* source){
   // create message
   struct message msg;
   msg.type = QU_ACK;
   strcmp(msg.source, source);
   get_active_users(active_users, msg.data);
   msg.size = strlen(msg.data)+1;

   m_send(sockfd, &msg); 
}

// logout user
void logout_user(int sockfd, char* source){
   printf("-----------------------------------------------\n");
   printf("Logging off User %s.\n", source);
   
   struct active_user* user = find_active_user(active_users, source);
   
   // decrement session num_user counter
   if(user->curr_session != NULL){
      user->curr_session->num_user--;
      // remove sessio if it is the last user
      if(user->curr_session->num_user == 0){
         remove_session(sessions, user->curr_session->sessionID);
      }
   }

   // remove user from active user list 
   active_users = remove_user(active_users, source);
   num_poll = remove_poll(pfds, num_poll, sockfd);
   printf("User successfully logged out.\n", source);
   printf("-----------------------------------------------\n");
}

