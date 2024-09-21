#ifndef _SOCKETS_EPOLLFD
#define _SOCKETS_EPOLLFD

#include <openssl/ssl.h>

void fd_read(int epollfd, int fd, char *buf);
void fd_write(int epollfd, int fd, char *buf);
void fd_lsaccept(int epoll_fd, int server_fd);

void fdssl_accept(int epoll_fd, int server_fd, SSL_CTX *sslctx, SSL *sslcon);
void fdssl_read(int epollfd, int fd, char *buf, SSL *sslcon);
void fdssl_write(int epollfd, int fd, char *buf, SSL *sslcon);

#endif