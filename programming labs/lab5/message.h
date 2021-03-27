#ifndef MESSAGE_H
#define MESSAGE_H
#include "util.h"

#define MAX_NAME 100
#define MAX_DATA 1000
#define NO_FLAGS 0

struct message{
   unsigned int type;
   unsigned int size;
   unsigned char source[MAX_NAME];
   unsigned char data[MAX_DATA];
};

typedef enum msg_type{
   LOGIN,
   LO_ACK,
   LO_NAK,
   EXIT,
   JOIN,
   JN_ACK,
   JN_NAK,
   LEAVE_SESS,
   NEW_SESS,
   NS_ACK,
   NS_NAK,
   MESSAGE,
   QUERY,
   QU_ACK,
   ERR,
   LS_ACK,
   LS_NAK,
   CLOSE
} MSG_TYPE;

void message_to_string(const struct message* msg, char* buff);
void string_to_message(const char* buff, struct message* msg);
int m_send(int sockfd, struct message* msg);
int m_receive(int sockfd, struct message* msg);
#endif
