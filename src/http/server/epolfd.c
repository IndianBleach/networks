
#include "../include/sockets/epoll.h"

#include <assert.h>
#include <fcntl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>


// ssl
#include "../include/sockets/ssl/ntssl.h"
static char sslbuff[1024];

void fd_read(int epollfd, int fd, char *buf) {
    int nread;
    if (fd <= 0) {
        return;
    }

    //printf("sock.read: buff=%p epoll=%i fd=%i\n", buf, epollfd, fd);
    //perror("last err: ");

    nread = read(fd, buf, 1024);
    if (nread == -1) {
        perror("err.do_read:\n");
        close(fd);                       // Remember Close FD
        epoll_del(epollfd, fd, EPOLLIN); // Delete monitor
    } else if (nread == 0) {
        fprintf(stderr, "client close.\n");
        close(fd);                       // Remember Close FD
        epoll_del(epollfd, fd, EPOLLIN); // Delete monitor
    } else {
        //printf("read message is : %s\n", buf);
        // Modify the event corresponding to the descriptor, and write it to write
        epoll_mod(epollfd, fd, EPOLLOUT);
    }
}

void fd_write(int epollfd, int fd, char *buf) {
    int nwrite;

    if (fd <= 0) {
        return;
    }

    nwrite = write(fd, buf, strlen(buf));

    if (nwrite == -1) {
        perror("err.do_write\n");
        close(fd);                        // Remember Close FD
        epoll_del(epollfd, fd, EPOLLOUT); // Delete Listening
    } else {
        close(fd);
    }
}

void fd_lsaccept(int epoll_fd, int server_fd) {
    int clfd;
    clfd = accept(server_fd, NULL, NULL);

    if (clfd < 0) {
        //printf("cl.serv_handle_accept<0\n");
        return;
    }

    int err;
    err = fcntl(server_fd, F_SETFL, O_NONBLOCK);
    assert(!err);

    epoll_add(epoll_fd, clfd, EPOLLIN | EPOLLOUT | EPOLLET);
}

void fdssl_accept(int epoll_fd, int server_fd, SSL_CTX *sslctx, SSL *sslcon) {
    int clfd;
    clfd = accept(server_fd, NULL, NULL);

    printf("ssl.fd_lsaccept_ssl: \n");
    SSL_set_fd(sslcon, clfd);

    int ssl_err = SSL_accept(sslcon);
    if (ssl_err <= 0) {
        ERR_print_errors_fp(stderr);
        int err = SSL_get_error(sslcon, ssl_err);

        if (err = SSL_ERROR_WANT_READ) {
            printf("ssl.err: SSL_ERROR_WANT_READ\n");
        } else if (err = SSL_ERROR_WANT_WRITE) {
            printf("ssl.err: SSL_ERROR_WANT_WRITE\n");
        } else {
            printf("ssl.error: err=%i d_err=%i\n", ssl_err, err);
            ERR_error_string(err, sslbuff);
            printf("msg=%s\n", sslbuff);
            memset(sslbuff, 0, 1024);
            ssl_shutdown(sslcon);
        }
    } else {
        printf("ssl.accept=OK\n");
    }


    if (clfd < 0) {
        //printf("cl.serv_handle_accept<0\n");
        return;
    }

    int err;
    err = fcntl(server_fd, F_SETFL, O_NONBLOCK);
    assert(!err);

    epoll_add(epoll_fd, clfd, EPOLLIN | EPOLLOUT | EPOLLET);
}

void fdssl_read(int epollfd, int fd, char *buf, SSL *sslcon) {
    int nread = 0;

    //printf("sock.read: buff=%p epoll=%i fd=%i\n", buf, epollfd, fd);
    //perror("last err: ");

    nread = SSL_read(sslcon, buf, 1024);
    printf("SSL_read=%i\n", nread);

    if (nread == -1) {
        perror("err.do_read:\n");
        close(fd);                       // Remember Close FD
        epoll_del(epollfd, fd, EPOLLIN); // Delete monitor
    } else if (nread == 0) {
        fprintf(stderr, "client close.\n");
        close(fd);                       // Remember Close FD
        epoll_del(epollfd, fd, EPOLLIN); // Delete monitor
    } else {
        //printf("read message is : %s\n", buf);
        // Modify the event corresponding to the descriptor, and write it to write
        epoll_mod(epollfd, fd, EPOLLOUT);
    }
}

void fdssl_write(int epollfd, int fd, char *buf, SSL *sslcon) {
    int nwrite = 0;

    nwrite = SSL_write(sslcon, buf, strlen(buf));
    printf("SSL_write=%i\n", nwrite);

    if (nwrite == -1) {
        //perror("err.do_write\n");
        ERR_print_errors_fp(stderr);
        int err = SSL_get_error(sslcon, nwrite);
        if (err = SSL_ERROR_WANT_READ) {
            printf("ssl.err: SSL_ERROR_WANT_READ\n");
        } else if (err = SSL_ERROR_WANT_WRITE) {
            printf("ssl.err: SSL_ERROR_WANT_WRITE\n");
        } else {
            printf("ssl.err=%i\n", err);
        }

        close(fd);                        // Remember Close FD
        epoll_del(epollfd, fd, EPOLLOUT); // Delete Listening
    } else {
        close(fd);
    }
}
