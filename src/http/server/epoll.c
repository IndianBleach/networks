

#include "include/sockets/epoll.h"

#include <sys/epoll.h>

void epoll_del(int epoll_fd, int fd, int state) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = state;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev);
}

void epoll_mod(int epoll_fd, int fd, int state) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = state;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

void epoll_add(int epoll_fd, int fd, int state) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = state;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}