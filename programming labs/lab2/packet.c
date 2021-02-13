#include "packet.h"
void packetToString(struct packet* file_packet, char* packet_str){
   int str_ptr = 0;
   memset(packet_str, 0, PACKETSIZE);
   sprintf(packet_str, "%d", file_packet->total_frag);
   str_ptr = strlen(packet_str);
   packet_str[str_ptr] = ':';

   printf("packet_str: %s\n", packet_str);
  
   sprintf(packet_str + str_ptr + 1, "%d", file_packet->frag_no);
   str_ptr = strlen(packet_str);
   packet_str[str_ptr] = ':';
   printf("packet_str: %s\n", packet_str);

   sprintf(packet_str + str_ptr + 1, "%d", file_packet->size);
   str_ptr = strlen(packet_str);
   packet_str[str_ptr] = ':';
   printf("packet_str: %s\n", packet_str);

   strcpy(packet_str + str_ptr + 1, file_packet->filename);
   str_ptr = str_ptr + strlen(file_packet->filename) + 1;
   packet_str[str_ptr] = ':';
   printf("packet_str: %s\n", packet_str);
   
   memcpy(packet_str + str_ptr + 1, file_packet->filedata, file_packet->size);
}

void stringToPacket(char* packet_str, struct packet* file_packet){
   char* token = strtok(packet_str, ":");
   int i = 0;
   int ptr = 0;
   while(i != 5){
      if(i == 0){
         file_packet->total_frag = atoi(token);
         ptr = ptr + strlen(token) + 1;
      }
      if(i == 1){
         file_packet->frag_no = atoi(token);
         ptr = ptr + strlen(token) + 1;
      }
      if(i == 2){
         file_packet->size = atoi(token);
         ptr = ptr + strlen(token) + 1;
      }
      if(i == 3){
         file_packet->filename = (char*) malloc(strlen(token)+1);
         strcpy(file_packet->filename, token);
         ptr = ptr + strlen(token) + 1;
      }
      if(i == 4){
         memset(file_packet->filedata, 0, DATASIZE);
         memcpy(file_packet->filedata, packet_str + ptr, file_packet->size);
      }
      token = strtok(NULL, ":");
      i++;
   }
}

