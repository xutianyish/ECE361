#include "util.h"
#ifndef PACKET_H
#define PACKET_H
#define DATASIZE 1000
#define PACKETSIZE 2000

struct packet{
   unsigned int total_frag;
   unsigned int frag_no;
   unsigned int size;
   char* filename;
   char filedata[DATASIZE];
};

void packetToString(struct packet* file_packet, char* packet_str);

void stringToPacket(char* packet_str, struct packet* file_packet);

#endif
