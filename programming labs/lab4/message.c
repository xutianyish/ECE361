#include "message.h"

void message_to_string(const struct message* msg, char* buff){
   char* ptr = (char*) msg;

   for(int i = 0; i < sizeof(struct message); i++){
      buff[i] = ptr[i];
   }
}

void string_to_message(const char* buff, struct message* msg){
   char* ptr = (char*) msg;

   for(int i = 0; i < sizeof(struct message); i++){
      ptr[i] = buff[i];
   }
   
}

void m_send(int sockfd, struct message* msg){
   unsigned char buff[BUFFER_SIZE];
   message_to_string(msg, buff);
   send(sockfd, buff, sizeof(struct message), NO_FLAGS);
}

void m_receive(int sockfd, struct message* msg){
   unsigned char buff[BUFFER_SIZE];
   int recv_len = recv(sockfd, buff, BUFFER_SIZE, NO_FLAGS);
   string_to_message(buff, msg);
}
