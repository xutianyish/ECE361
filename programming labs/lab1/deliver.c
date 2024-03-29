#include "util.h"

void getfilename(int sockfd, struct addrinfo* server_addr_ptr, char* file_name);

// reference: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
int main(int argc, char** argv){
   // parse client input
   if(argc != 3){
      printf("Wrong number of arguments. Use \"deliver <server address> <server port number>\" to send a message to the server.\n");
      exit(EXIT_FAILURE);
   }
   
   char* server_addr = argv[1];
   char* server_portid = argv[2];
   
   // get server info
   struct addrinfo hints, *server_addr_ptr;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET; // IPv4
   hints.ai_socktype = SOCK_DGRAM; // datagram socket
   int status = getaddrinfo(server_addr, server_portid, &hints, &server_addr_ptr);
   if(status){
      perror("get server address info failed");
      exit(EXIT_FAILURE);
   }

   // creating socket file descriptor
   int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   if(sockfd < 0){
      perror("socket creation failed");
      exit(EXIT_FAILURE);
   }

   while(true){
      char file_name[BUFFER_SIZE];
      getfilename(sockfd, server_addr_ptr, file_name);
      printf("File exists. Sending a message to the server.\n");
      sendto(sockfd, "ftp", 4, 0, (const struct sockaddr *)server_addr_ptr->ai_addr, sizeof(struct sockaddr));
      printf("message \"ftp\" sent.\n");
      char reply[BUFFER_SIZE];
      int len = recvfrom(sockfd, reply, BUFFER_SIZE, 0, NULL, NULL);
      if(!strcmp(reply, "yes")){
         printf("A file transfer can start.\n");
      }else{
         printf("The server did not respond with \"yes\".");
         close(sockfd);
         freeaddrinfo(server_addr_ptr);
         exit(EXIT_FAILURE);
      }
   }
   
   return(0);
}

void getfilename(int sockfd, struct addrinfo* server_addr_ptr, char* file_name){
   // ask user to input a message as follows:
   printf("input format: ftp <file name>\n");
   char *line = NULL;
   size_t bufsize = BUFFER_SIZE;
   int len = getline(&line, &bufsize, stdin);
   
   // check if input format is correct
   if(strncmp(line, "ftp ", 4) != 0){
      printf("Input format is incorrect.\n");
      close(sockfd);
      freeaddrinfo(server_addr_ptr);
      exit(EXIT_FAILURE);
   } 

   // get file_name
   strncpy(file_name, line+4, len-5);

   // check the existence of the file
   printf("Check the existence of the file.\n");
   if(access(file_name, F_OK) != 0){
      printf("File %s does not exist.\n", file_name);
      close(sockfd);
      freeaddrinfo(server_addr_ptr);
      exit(EXIT_FAILURE);
   }
}
