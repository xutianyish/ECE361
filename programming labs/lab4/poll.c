#include "poll.h"

nfds_t add_poll(struct pollfd* pfds, nfds_t num_poll, int sockfd){
   pfds[num_poll].fd = sockfd;
   pfds[num_poll].events = POLLIN | POLLHUP;
   pfds[num_poll].revents = 0;

   return num_poll+1;
}

nfds_t remove_poll(struct pollfd* pfds, nfds_t num_poll, int sockfd){
   for(int i = 0; i < num_poll; i++){
      if(pfds[i].fd == sockfd){
         pfds[i].fd = pfds[num_poll-1].fd;
         return num_poll - 1;
      }
   }
   return num_poll;
}

void clear_poll(struct pollfd* pfds, nfds_t num_poll){
   for(int i = 0; i < num_poll; i++){
      pfds[i].revents = 0;
   }
}
