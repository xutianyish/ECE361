#ifndef MESSAGE_H
#define MESSAGE_H
#include "util.h"

#define MAX_NAME 100
#define MAX_DATA 1000


struct message{
   unsigned int type;
   unsigned int size;
   unsigned char source[MAX_NAME];
   unsigned char data[MAX_DATA];
};
#endif
