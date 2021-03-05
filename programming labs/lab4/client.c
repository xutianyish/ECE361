#include "client.h"
// reference: https://www.educative.io/edpresso/splitting-a-string-using-strtok-in-c
char* args[MAX_ARG];
int num_arg = 0;

int main(int argc, char** argv){
   char buff[BUFFER_SIZE+1];
   int sockfd = -1;

   while(true){
      memset(buff, 0, BUFFER_SIZE);
      num_arg = 0;
      fgets(buff, BUFFER_SIZE, stdin);
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
            logout();
            sockfd = -1;
         }
      }
      else if(strcmp(args[0], "/joinsession") == 0){
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }else{
            joinsession();
         }
      }
      else if(strcmp(args[0], "/leavesession") == 0){
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }else{
            leavesession();
         }
      }
      else if(strcmp(args[0], "/createsession") == 0){
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }else{
            createsession();
         }
      }
      else if(strcmp(args[0], "/list") == 0){
         if(sockfd == -1){
            printf("You haven't logged in to the server. \n");
            printf("Format: /login <client ID> <password> <server-IP> <server-port>.\n");
         }
         else{
            list();
         }
      }
      else if(strcmp(args[0], "/quit") == 0){
         if(sockfd != -1){
            printf("Loggin off before exiting...\n");
            // TODO: call logout function here
            close(sockfd);
         }

         printf("Exiting the program...\n");
         return 0;
      }
      else{
         printf("Error: invalid command. It must be one of the followings: /login, /logout, /joinsession, /leavesession, /createsession, /list, /quit. Please try again. \n");
         printf("\n");
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
   buff[BUFFER_SIZE] = '\0';

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

   char* clientID = args[1];
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
   strcpy(msg.source, clientID);
   msg.size = sprintf(msg.data, "%s,%s", clientID, password);
   msg.type = LOGIN;

   m_send(sockfd, &msg);
   m_receive(sockfd, &reply);
   
   if(reply.type != LO_ACK){
      printf("Login failed: %s\n", reply.data);
      close(sockfd);
      return -1;
   }

   printf("Successfully logged in as %s.\n", args[1]);
   return sockfd;
}

// exit the server
void logout(){
}

// join the conference session with the given session id
void joinsession(){
}

// leave the current established session
void leavesession(){
}

// create a new session
void createsession(){
}

// get the list of connected clients and available sessions
void list(){
   
}

// terminate the program, close socket, free connected user, remove user from session
void quit(){
}

