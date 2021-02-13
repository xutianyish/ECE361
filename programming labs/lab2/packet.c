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
   int prev_ptr = 0;
   int curr_ptr = 0;
   char buff[100];
   for(int field_idx = 0; field_idx < 5; field_idx++){
      while(*(packet_str + curr_ptr) != ':' && field_idx != 4){
         curr_ptr++;
      }
      // get the field value
      if(field_idx == 0){
         memcpy(buff, packet_str+prev_ptr, curr_ptr - prev_ptr);
         buff[curr_ptr-prev_ptr] = '\0';
         file_packet->total_frag = atoi(buff);
         printf("total_frag:%d\n", file_packet->total_frag);
      }
      if(field_idx == 1){
         memcpy(buff, packet_str+prev_ptr, curr_ptr - prev_ptr);
         buff[curr_ptr-prev_ptr] = '\0';
         file_packet->frag_no = atoi(buff);
         printf("frag_no: %d\n", file_packet->frag_no);
      }
      if(field_idx == 2){
         memcpy(buff, packet_str+prev_ptr, curr_ptr - prev_ptr);
         buff[curr_ptr-prev_ptr] = '\0';
         file_packet->size = atoi(buff);
         printf("size: %d\n", file_packet->size);
      }
      if(field_idx == 3){
         file_packet->filename = (char*) malloc(curr_ptr - prev_ptr + 1);
         memcpy(file_packet->filename, packet_str+prev_ptr, curr_ptr - prev_ptr);
         file_packet->filename[curr_ptr-prev_ptr] = '\0';
         printf("file_name: %s", file_packet->filename);
      }
      if(field_idx == 4){
         memset(file_packet->filedata, 0, DATASIZE);
         memcpy(file_packet->filedata, packet_str + curr_ptr, file_packet->size);
      }
      
      // incr prev_ptr
      curr_ptr++;
      prev_ptr = curr_ptr;
   }
}

