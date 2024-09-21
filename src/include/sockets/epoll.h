#ifndef _SOCKETS_EPOLL
#define _SOCKETS_EPOLL

void epoll_del(int epoll_fd, int fd, int state);

void epoll_mod(int epoll_fd, int fd, int state);

void epoll_add(int epoll_fd, int fd, int state);

#endif