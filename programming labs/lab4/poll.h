#ifndef POLL_H
#define POLL_H
#include "util.h"
#include <poll.h>

#define INF_TIMEOUT -1

nfds_t add_poll(struct pollfd* pfds, nfds_t num_poll, int sockfd);
nfds_t remove_poll(struct pollfd* pfds, nfds_t num_poll, int sockfd);
void clear_poll(struct pollfd* pfds, nfds_t num_poll);

#endif
