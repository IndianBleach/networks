#ifndef _SOCKETS_EPOLLFD
#define _SOCKETS_EPOLLFD

void fd_read(int epollfd, int fd, char *buf);

void fd_write(int epollfd, int fd, char *buf);

void fd_lsaccept(int epoll_fd, int server_fd);

#endif