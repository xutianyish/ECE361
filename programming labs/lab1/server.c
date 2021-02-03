#include "util.h"

// reference: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
int main(int argc, char** argv){
   if(argc != 2){
      printf("Wrong number of arguments. Use \"server <UDP listen port>\" to start a server.\n");
      exit(EXIT_FAILURE);
   }
   
   int portid = atoi(argv[1]);
   
   // validate portid
   if(portid < 1024 || portid > 65536){
      printf("port id should be in the range of 1024~65536\n");
      exit(EXIT_FAILURE);
   }
   
   // open a UDP socket and listen at the specified port number
   int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   if(sockfd < 0){
      perror("socket creation failed");
      exit(EXIT_FAILURE);
   }
   
   // init clientaddr and server addr
   struct sockaddr_in serveraddr, clientaddr;
   memset(&serveraddr, 0, sizeof(serveraddr));
   memset(&clientaddr, 0, sizeof(clientaddr));
   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = INADDR_ANY;
   serveraddr.sin_port = htons(portid);

   // bind the socket with the server address
   if(bind(sockfd, (const struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0){
      perror("bind failed");
      exit(EXIT_FAILURE);
   }
   
   char buffer[BUFFER_SIZE];
   int clientaddr_len = sizeof(clientaddr);
   while(true){
      if(recvfrom(sockfd, (char*) buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &clientaddr_len) == -1) {
         printf("Error in receiving\n");
         exit(EXIT_FAILURE);
      }
      printf("received %s from client\n", buffer);
      
      if(strcmp(buffer, "ftp") == 0){
         printf("sending \"yes\" to the client.\n");
         if(sendto(sockfd, "yes", strlen("yes"), 0, (const struct sockaddr*)&clientaddr, clientaddr_len) == -1) {
            printf("Error in sending yes to client\n");
            exit(EXIT_FAILURE);
         }
      }else{
         printf("sendin \"no\" to the client.\n");
         if(sendto(sockfd, "no", strlen("no"), 0, (const struct sockaddr*)&clientaddr, clientaddr_len) == -1) {
            printf("Error in sending no to client\n");
            exit(EXIT_FAILURE);
         }
      }
   }

   return(0);
}
