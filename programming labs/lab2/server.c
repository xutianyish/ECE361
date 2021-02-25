#include "util.h"
#include "packet.h"

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
      recvfrom(sockfd, (char*) buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &clientaddr_len);
      printf("received %s from client\n", buffer);
      
      if(strcmp(buffer, "ftp") == 0){
         printf("sending \"yes\" to the client.\n");
         sendto(sockfd, "yes", strlen("yes")+1, 0, (const struct sockaddr*)&clientaddr, clientaddr_len);

         // receive file from deliver
         struct packet recv_packet;
         FILE* fp = NULL;
         char filename[BUFFER_SIZE];
         int packetnum = 1;
         do{
            bool succ = true;
            char packet_str[PACKETSIZE];
            recvfrom(sockfd, (char*) packet_str, PACKETSIZE, 0, (struct sockaddr*)&clientaddr, &clientaddr_len);
            stringToPacket(packet_str, &recv_packet);
            
            // create the file and write to it
            if(fp == NULL){
               strcpy(filename, recv_packet.filename);

               // the file exist
               if(access(filename, F_OK) == 0){
                  printf("The file %s already exists. Creating a file named copy\n", filename);
                  strcpy(filename, "copy");
                  fp = fopen(filename, "w");
               }else{
                  fp = fopen(filename, "w");
               }

               // error checking
               if(packetnum != recv_packet.frag_no){
                  succ = false;
               }
            } else {
               // error checking
               packetnum++;
               if(packetnum != recv_packet.frag_no){
                  succ = false;
               }
               if(strcmp(filename, recv_packet.filename) != 0 && strcmp(filename, "copy") != 0){
                  succ = false;
               }
            }

            // write to file
            int numbyte = fwrite(recv_packet.filedata, sizeof(char), recv_packet.size, fp);
            if(numbyte != recv_packet.size){
               printf("Write Error!\n");
               exit(1);
            }
            
            // send ACK/NACK
            char recv_str[PACKETSIZE];
            if(succ){
               strcpy(recv_packet.filedata, "ACK");
               packetToString(&recv_packet, recv_str);
               sendto(sockfd, recv_str, PACKETSIZE, 0, (const struct sockaddr*)&clientaddr, clientaddr_len);
            }else{
               strcpy(recv_packet.filedata, "NACK");
               packetToString(&recv_packet, recv_str);
               sendto(sockfd, recv_str, PACKETSIZE, 0, (const struct sockaddr*)&clientaddr, clientaddr_len);
            }
         } while(recv_packet.total_frag != recv_packet.frag_no);
         printf("file transfer completed.\n");
         fclose(fp);
         free(recv_packet.filename);
      }else{
         printf("sendin \"no\" to the client.\n");
         sendto(sockfd, "no", strlen("no")+1, 0, (const struct sockaddr*)&clientaddr, clientaddr_len);
      }
   }

   return(0);
}
