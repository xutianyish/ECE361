#include "server.h"
// reference: https://www.geeksforgeeks.org/socket-programming-cc/

// session globals
static struct cred* registered_users;
static struct session* sessions;
static struct active_user* active_users;

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

   // create a new TCP socket //TODO: AF or PF? 
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
   if(bind(welcome_sock, (struct sockaddr*)&serveraddr_in, sizeof(serveraddr_in)) == 0){
      perror("Error: failed to bind welcome socket with server port.\n");
      exit(EXIT_FAILURE);
   }
   
   // TODO: do we need listen here? if we do polling
   listen(welcome_sock, BACKLOG);
   num_poll = add_poll(pfds, num_poll, welcome_sock);

   // server loop
   while(true){
      clear_poll(pfds, num_poll);
      poll(pfds, num_poll, INF_TIMEOUT);

      for(int i = 0; i < num_poll; i++){
         if(pfds[i].revents & (POLLIN | POLLHUP)){
            if(i == 0){
               login();
            }else{
               process_req();
            }
         }
      }
   }

   return(0);
}

// login new user
void login(){
   int welcome_sockfd = pfds[0].fd;
   struct sockaddr_in clientaddr_in;
   int clientaddr_in_len = sizeof(clientaddr_in);
   int connection_sock = accept(welcome_sockfd, (struct sockaddr*)&clientaddr_in, &clientaddr_in_len);

   // get client port and ip
   char* client_ip = inet_ntoa((&clientaddr_in)->sin_addr);
   int client_port = ntohs((&clientaddr_in)->sin_port);

   // receive message from user

   // check cred, refuse login if cred does not match

   // refuse login if client already logged in

   // send reply message to client 
   // LO_NACK 

   // add active user to list
   num_poll = add_poll(pfds, num_poll, connection_sock);

   printf("-----------------------------------------------\n");
   printf("New Client Connected.\n");
   printf("ClienID: \n");
   printf("IP address: %d\n", client_ip);
   printf("Port Number: %d\n", client_port);
   printf("-----------------------------------------------\n");
}

// process new requests
void process_req(){
   printf("TODO.\n");
   return;
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
