   #include "util.h"

void init_timeout(struct timeout* t){
   t->sample_rtt = 0;
   t->estimated_rtt = 1;
   t->dev_rtt = 0;
   t->timeout_interval = 1;
}

// use sampleRTT to update EstimatedRTT, DevRTT, and TimeoutInterval
void update_timeout(struct timeout* t){
   t->dev_rtt = (1-BETA) * t->dev_rtt + BETA * fabs(t->sample_rtt - t->estimated_rtt);
   t->estimated_rtt = (1-ALPHA) * t->estimated_rtt + ALPHA * t->sample_rtt;
   t->timeout_interval = t->estimated_rtt + GAMMA * t->dev_rtt;
   printf("update timeout:\n");
   printf("\t estimated_rtt = %fs\n", t->estimated_rtt);
   printf("\t dev_rtt = %fs\n", t->dev_rtt);
   printf("\t timeout_interval = %fs\n", t->timeout_interval);
}

bool set_timeout(double interval, int sockfd){
   struct timeval tv;
   tv.tv_sec = (int) interval;
   tv.tv_usec = (int) ((interval - tv.tv_sec) * 1000000.0);
   if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0){
      perror("setsockopt failed");
      return false;
   }
   
   printf("\nset timeout interval to: %fs\n\n", interval);
   return true;
}
