#include "util.h"
#include "packet.h"

void getfilename(int sockfd, struct addrinfo* server_addr_ptr, char* file_name);
void handshake(struct addrinfo* server_addr_ptr,int sockfd);
void send_file(struct addrinfo* server_addr_ptr, int sockfd, char* file_name);
int getnumfragment(FILE* fp);

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

   char file_name[BUFFER_SIZE];
   getfilename(sockfd, server_addr_ptr, file_name);
   printf("File exists. Sending a message to the server.\n");

   // if handshake successful send file
   handshake(server_addr_ptr, sockfd);
   send_file(server_addr_ptr, sockfd, file_name);

   return(0);
}

void send_file(struct addrinfo* server_addr_ptr, int sockfd, char* file_name){
   // create packet and prepare
   FILE* fp = fopen(file_name, "r");
   int numfragment = getnumfragment(fp);
   for (int fragid = 0; fragid < numfragment; fragid++){
      struct packet file_packet;
      file_packet.total_frag = numfragment;
      file_packet.frag_no = fragid;
      file_packet.size = fread(file_packet.filedata, sizeof(char), DATASIZE, fp);
      file_packet.filename = file_name;
      char packet_str[PACKETSIZE];
      packetToString(&file_packet, packet_str);
   
      // send packet to server
      sendto(sockfd, packet_str, PACKETSIZE, 0, (const struct sockaddr *)server_addr_ptr->ai_addr, sizeof(struct sockaddr));
   
      // wait for ack
      char reply[PACKETSIZE];
      recvfrom(sockfd, reply, BUFFER_SIZE, 0, NULL, NULL);
      stringToPacket(reply, &file_packet);
      if(!strcmp(file_packet.filedata, "ACK")){
         printf("packet sent successfully.\n");
      }else{
         printf("The packet was not sent successfully\n");
      }
   }
   fclose(fp);
}

int getnumfragment(FILE* fp){
   fseek(fp, 0, SEEK_END);
   int file_size = ftell(fp);
   int numfragment = file_size / 1000;
   rewind(fp);
   if(file_size % 1000 == 0){
      return numfragment;
   } else {
      return numfragment + 1;
   }
}

void handshake(struct addrinfo* server_addr_ptr, int sockfd){
   // calculate RTT time
   // start time
   clock_t start = clock();
   
   sendto(sockfd, "ftp", 4, 0, (const struct sockaddr *)server_addr_ptr->ai_addr, sizeof(struct sockaddr));
   printf("message \"ftp\" sent.\n");
   char reply[BUFFER_SIZE];
   recvfrom(sockfd, reply, BUFFER_SIZE, 0, NULL, NULL);
   
   // end time
   clock_t end = clock();
   double time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
   printf("The RTT is %f seconds.\n", time_used);

   if(!strcmp(reply, "yes")){
      printf("A file transfer can start.\n\n");
   }else{
      printf("The server did not respond with \"yes\".\n");
      close(sockfd);
      freeaddrinfo(server_addr_ptr);
      exit(EXIT_FAILURE);
   }
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

   // get file_name from user input
   strncpy(file_name, line+4, len-5);
   file_name[len-5] = '\0';

   // check the existence of the file
   printf("Check the existence of the file.\n");
   if(access(file_name, F_OK) != 0){
      printf("File %s does not exist.\n", file_name);
      close(sockfd);
      freeaddrinfo(server_addr_ptr);
      exit(EXIT_FAILURE);
   }
}
