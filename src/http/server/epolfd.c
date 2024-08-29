
#include "include/sockets/epoll.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>

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
