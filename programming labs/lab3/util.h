#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <regex.h>
#include "math.h"

#define BUFFER_SIZE 1100
#define ALPHA       0.125
#define BETA        0.25
#define GAMMA       4

struct timeout{
   double sample_rtt;
   double estimated_rtt;
   double dev_rtt;
   double timeout_interval;
};

void init_timeout(struct timeout* t);
void update_timeout(struct timeout* t);
bool set_timeout(double interval, int sockfd);

#endif
