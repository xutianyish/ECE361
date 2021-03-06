#include "client.h"
// reference: https://www.educative.io/edpresso/splitting-a-string-using-strtok-in-c
char* args[MAX_ARG];
int num_arg = 0;
char curr_user[MAX_NAME];

struct pollfd pfds[2];
int num_poll = 0; 

int main(int argc, char** argv){
   char buff[BUFFER_SIZE+1];
   int sockfd = -1;
   
   num_poll = add_poll(pfds, num_poll,STDIN_FILENO);

   while(true){
      clear_poll(pfds, num_poll);
      poll(pfds, num_poll, INF_TIMEOUT);

      if(sockfd != -1 && pfds[1].revents){
         struct message msg;
         if(m_receive(sockfd, &msg) <= 0){
            printf("Server has been shut down.\n");
            close(sockfd);
            exit(1);
         }
         else if(msg.type == MESSAGE){
            printf("-----------------------------------------------\n");
            printf("Received broadcasting message from server: \n");
            printf("%s\n", msg.data);
            printf("-----------------------------------------------\n");
         }
         else if(msg.type == ERR){
            printf("-----------------------------------------------\n");
            printf("Received error message from server: \n");
            printf("%s\n", msg.data);
            printf("-----------------------------------------------\n");
         }
         continue;
      }

      memset(buff, 0, BUFFER_SIZE);
      num_arg = 0;
      fgets(buff, BUFFER_SIZE, stdin);
      buff[BUFFER_SIZE] = '\0';
      char buff_copy[BUFFER_SIZE+1];
      strcpy(buff_copy, buff);
      client_parser(buff);
      
      if(strcmp(args[0], "/login") == 0){
         if(sockfd = -1){
            sockfd = login();
         }
         else{
            printf("You have already logged in. \n");   
         }
      }
      else if(strcmp(args[0], "/logout") == 0){
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }else{
            logout(sockfd);
            sockfd = -1;
         }
      }
      else if(strcmp(args[0], "/joinsession") == 0){
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }else{
            joinsession(sockfd);
         }
      }
      else if(strcmp(args[0], "/leavesession") == 0){
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }else{
            leavesession(sockfd);
         }
      }
      else if(strcmp(args[0], "/createsession") == 0){
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }else{
            createsession(sockfd);
         }
      }
      else if(strcmp(args[0], "/list") == 0){
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }
         else{
            list(sockfd);
         }
      }
      else if(strcmp(args[0], "/quit") == 0){
         quit(sockfd);
         return 0;
      }
      else{
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }else{
            send_message(sockfd, buff_copy);
         }
      }

   }
   return(0);
}

void client_parser(char* buff){
   // replace end of line with '\0'
   for(int i = 0; i < BUFFER_SIZE; i++){
      if(buff[i] == '\n'){
         buff[i] = '\0';
      } 
   }
   

   char* token = strtok(buff, " ");
   while(token != NULL){
      // debug print all args
      args[num_arg] = malloc(strlen(token)+1);
      strcpy(args[num_arg], token);
      num_arg++;
      token = strtok(NULL, " ");
   }
}

// log into the server at the given address and port. The IP address is specified in the string dot format
int login(){
   if(num_arg != 5){
      printf("Error: the format for login is incorrect.\n");
      printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
      return -1;
   }

   strcpy(curr_user, args[1]);
   char* password = args[2];
   char* server_ip = args[3];
   char* server_port = args[4];

   // TCP IPv4 hints
   struct addrinfo hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET; // IPv4
   hints.ai_socktype = SOCK_STREAM; // TCP
   
   // get server addr info
   struct addrinfo* serveraddr_in;
   if(getaddrinfo(server_ip, server_port, &hints, &serveraddr_in) != 0){
      printf("Error: get server address info failed.\n");
      return -1;
   }
   
   // create new socket
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if(sockfd == -1){
      printf("Error: socket creation failed. \n");
      return -1;
   }

   // allow socket to be reused through setsockopt
   int yes = 1;
   if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
      printf("Error: setsockopt failed. \n");
      close(sockfd);
      return -1;
   }

   // connect to server
   if(connect(sockfd, serveraddr_in->ai_addr, serveraddr_in->ai_addrlen) != 0){
      printf("Error: failed to connect to server.\n");
      close(sockfd);
      return -1;
   }
   freeaddrinfo(serveraddr_in);

   struct message msg, reply;
   // create msg
   memset(msg.source, 0, sizeof(msg.source));
   memset(msg.data, 0, sizeof(msg.data));
   strcpy(msg.source, curr_user);
   msg.size = sprintf(msg.data, "%s,%s", curr_user, password);
   msg.type = LOGIN;

   m_send(sockfd, &msg);
   m_receive(sockfd, &reply);
   
   if(reply.type != LO_ACK){
      printf("Login failed: %s\n", reply.data);
      close(sockfd);
      return -1;
   }

   printf("Successfully logged in as %s.\n", args[1]);
   num_poll = add_poll(pfds, num_poll, sockfd);
   return sockfd;
}

// exit the server
void logout(int sockfd){
   if(num_arg != 1){
      printf("Error: the format for logout is incorrect.\n");
      printf("Format: /logout.\n");
      return;
   }

   struct message msg;
   msg.type = EXIT;
   msg.size = 0;
   strcpy(msg.source, curr_user);
   m_send(sockfd, &msg);
   printf("-----------------------------------------------\n");
   printf("User %s successfully logged out.\n", curr_user);
   printf("-----------------------------------------------\n");
}

// join the conference session with the given session id
void joinsession(int sockfd){
   if(num_arg != 2){
      printf("Error: the format for joinsession is incorrect.\n");
      printf("Format: /joinsession <session ID>.\n");
      return;
   }

   struct message msg, reply;
   msg.type = JOIN;
   strcpy(msg.source, curr_user);
   msg.size = sprintf(msg.data, "%s", args[1]);
   m_send(sockfd, &msg);

   m_receive(sockfd, &reply);
   if(reply.type == JN_NAK){
      char* sessionID = strtok(reply.data, ",");
      char* failure_msg = strtok(NULL, ",");
      while(strtok(NULL, ","))
         ;

      printf("Error: joinsession %s failed. %s\n", sessionID, failure_msg);
      return;
   }

   printf("-----------------------------------------------\n");
   printf("Successfully joined session %s\n", reply.data);
   printf("-----------------------------------------------\n");
}

// leave the current established session
void leavesession(int sockfd){
   if(num_arg != 1){
      printf("Error: the format for leavesession is incorrect.\n");
      printf("Format: /leavesession.\n");
      return;
   }

   struct message msg;
   msg.type = LEAVE_SESS;
   msg.size = 0;
   strcpy(msg.source, curr_user);
   m_send(sockfd, &msg);

   printf("-----------------------------------------------\n");
   printf("Left all sessions joined.\n");
   printf("-----------------------------------------------\n");
}

// create a new session
void createsession(int sockfd){
   if(num_arg != 2){
      printf("Error: the format for createsession is incorrect.\n");
      printf("Format: /createsession <session ID>.\n");
      return;
   }
   
   struct message msg, reply;
   msg.type = NEW_SESS;
   msg.size = sprintf(msg.data, "%s", args[1]);
   strcpy(msg.source, curr_user);
   m_send(sockfd, &msg);

   m_receive(sockfd, &reply);
   if(reply.type != NS_ACK){
      printf("Error: failed to create new session.\n");
   }
   printf("-----------------------------------------------\n");
   printf("Successfully created new session with ID: %s\n", reply.data);
   printf("-----------------------------------------------\n");
}

// get the list of connected clients and available sessions
void list(int sockfd){
   if(num_arg != 1){
      printf("Error: the format for list is incorrect.\n");
      printf("Format: /list.\n");
      return;
   }

   struct message msg; 
   msg.type = QUERY;
   msg.size = 0;
   m_send(sockfd, &msg);

   m_receive(sockfd, &msg);
   printf("-----------------------------------------------\n");
   printf("current connected users:\n");
   printf("%s", msg.data);
   printf("-----------------------------------------------\n");
}

// terminate the program, close socket, free connected user, remove user from session
void quit(int sockfd){
   if(num_arg != 1){
      printf("Error: the format for quit is incorrect.\n");
      printf("Format: /quit.\n");
      return;
   }

   if(sockfd != -1){
      printf("Loggin off before exiting...\n");
      struct message msg;
      msg.type = EXIT;
      msg.size = 0;
      strcpy(msg.source, curr_user);
      m_send(sockfd, &msg);

      close(sockfd);
   }

   printf("Exiting the program...\n");
}

void send_message(int sockfd, char* buff){
   for(int i = 0; i < BUFFER_SIZE; i++){
      if(buff[i] == '\n') buff[i] = '\0';
   }

   struct message msg;
   msg.type = MESSAGE;
   msg.size = sprintf(msg.data, "%s", buff);
   strcpy(msg.source, curr_user);
   m_send(sockfd, &msg);
   printf("-----------------------------------------------\n");
   printf("Broadcasting message: %s\n", buff);
   printf("-----------------------------------------------\n");
}

