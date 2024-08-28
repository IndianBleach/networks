
#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// ___________ server.config.io
typedef struct serverconfig_io serverconfig_io;

struct serverconfig_io {
    int epoll_max_events;
};

// ___________ server.config
typedef struct serverconfig serverconfig;

struct serverconfig {
    struct addrinfo host_addr;
    serverconfig_io io;
    const char *server_port;
    const char *server_ip;
    int server_max_con;
};

#include <sys/epoll.h>

char resp2[] = "HTTP/1.0 200 OK\r\n"
               "Server: webserver-c\r\n"
               "Content-type: text/html\r\n\r\n"
               "<html>hello, world</html>\r\n";

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

#include <fcntl.h>

int make_non_blocking(int fd) {
    int flags, s;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if (s == -1) {
        perror("fcntl");
        return -1;
    }

    return 0;
}

void do_read(int epollfd, int fd, char *buf) {
    int nread;
    if (fd <= 0) {
        return;
    }

    printf("sock.read: buff=%p epoll=%i fd=%i\n", buf, epollfd, fd);
    perror("last err: ");

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
        printf("read message is : %s\n", buf);
        // Modify the event corresponding to the descriptor, and write it to write
        epoll_mod(epollfd, fd, EPOLLOUT);
    }
}

void serv_handle_accept(int epoll_fd, int server_fd) {
    // ACCEPT NEW
    //printf("cl.serv_handle_accept.\n", server_fd);
    int clfd;
    clfd = accept(server_fd, NULL, NULL);

    if (clfd < 0) {
        //printf("cl.serv_handle_accept<0\n");
        return;
    }

    printf("cl.serv_handle_accept. cl=%i\n", clfd);

    int err;
    err = fcntl(server_fd, F_SETFL, O_NONBLOCK);
    assert(!err);

    epoll_add(epoll_fd, clfd, EPOLLIN | EPOLLOUT | EPOLLET);

    perror("cl.accept: ");
}

void do_write(int epollfd, int fd, char *buf) {
    int nwrite;

    printf("sock.write: buff=%p epoll=%i fd=%i\n", buf, epollfd, fd);
    perror("last err: ");

    if (fd <= 0) {
        return;
    }

    nwrite = write(fd, buf, strlen(buf));
    printf("sock.write.res: sended=%i\n", nwrite);
    if (nwrite == -1) {
        perror("err.do_write\n");
        close(fd);                        // Remember Close FD
        epoll_del(epollfd, fd, EPOLLOUT); // Delete Listening
    } else {
        close(fd);
        //epoll_del(epollfd, fd, EPOLLOUT);
        //epoll_mod(epollfd, fd, EPOLLIN);
    }
    //memset(buf, 0, 1024);
}

int server_start(serverconfig *config) {
    // config.epoll
    struct epoll_event ev;
    struct epoll_event poll_events[config->io.epoll_max_events];
    int epoll_fd;
    int nfds;

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        printf("epoll_create1\n");
        return -1;
    }


    struct addrinfo *parseaddr;

    // get addr
    int status;
    if ((status = getaddrinfo(config->server_ip, config->server_port, &config->host_addr, &parseaddr)) == -1) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        return -1;
    }

    // create socket
    int socket_fd;
    if ((socket_fd = socket(parseaddr->ai_family, parseaddr->ai_socktype, parseaddr->ai_protocol)) == -1) {
        printf("socket\n");
        return -1;
    }

    // bind
    if (bind(socket_fd, parseaddr->ai_addr, parseaddr->ai_addrlen)) {
        printf("bind\n");
        return -1;
    }

    freeaddrinfo(parseaddr);

    // fix maybe add_event
    if (listen(socket_fd, config->server_max_con) == -1) {
        printf("listen\n");
        return -1;
    }


    // fix: sepr
    int client_fd;
    ssize_t bytes_read;
    char rbuff[1024];

    // add event (listener (server))
    epoll_add(epoll_fd, socket_fd, EPOLLIN);

    while (1) {
        printf("epoll.new\n");
        int ret = epoll_wait(epoll_fd, poll_events, config->io.epoll_max_events, -1);
        printf("epoll.ret=%i\n", ret);

        if (ret == -1) {
            perror("epoll_wait: \n");
        }

        for (int i = 0; i < ret; i++) {
            client_fd = poll_events[i].data.fd;
            if ((client_fd == socket_fd) && (poll_events[i].events & EPOLLIN))
                serv_handle_accept(epoll_fd, socket_fd);
            else if (poll_events[i].events & EPOLLIN) {
                printf("event.EPOLLIN\n");
                do_read(epoll_fd, client_fd, rbuff);
            } else if (poll_events[i].events & EPOLLOUT) {
                printf("event.EPOLLOUT\n");
                do_write(epoll_fd, client_fd, resp2);
            }
        }
    }


    // fix maybe add_event
    if (listen(socket_fd, config->server_max_con) == -1) {
        printf("listen\n");
        return -1;
    }

    // config.epoll
    //ev.events = EPOLLIN;
    //ev.data.fd = socket_fd;

    // -- add host socket to poll IN
    if (1 == 2 && epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1) {
        printf("epoll_ctl\n");
        return -1;
    }


    for (;;) {
        // ACCEPT NEW
        printf("cl.accept(bef)=%i\n", client_fd);
        client_fd = accept(socket_fd, NULL, NULL);

        if (client_fd < 0) {
            printf("accept<0\n");
            continue;
        }

        printf("cl.accept=%i\n", client_fd);

        struct epoll_event clev;
        clev.data.fd = client_fd;
        clev.events = EPOLLIN;

        // ADD CLIENT FD
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &clev) == -1) {
            printf("epoll_ctl: client_fd\n");
            return -1;
        }

        // GET READY EVENTS
        nfds = epoll_wait(epoll_fd, poll_events, config->io.epoll_max_events, -1);

        if (nfds == -1) {
            printf("epoll_wait\n");
            return -1;
        }

        printf("nfds=%i\n", nfds);

        for (int i = 0; i < nfds; i++) {
            int cur_fd = poll_events[i].data.fd;
            printf("cl.event.fd=%i state=%u\n", cur_fd, poll_events[i].events);

            if (poll_events[i].data.fd == socket_fd) {
                //setnonblocking(conn_sock);
                // SERVER SOCKET
                printf("event.server\n");
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;


            } else {
                if (poll_events[i].events && EPOLLIN) // CAN READ
                {
                    // read from client
                    bytes_read = recv(cur_fd, rbuff, 1024, 0);
                    if (bytes_read == -1) { // client.error
                        printf("cl.event.EPOLLIN.error: recv=-1\n");
                        close(cur_fd);
                        epoll_del(epoll_fd, cur_fd, EPOLLIN);
                    } else if (bytes_read == 0) { // client.disconnect
                        printf("cl.event.EPOLLIN.disconnect\n");
                        close(cur_fd);
                        epoll_del(epoll_fd, cur_fd, EPOLLIN);
                    } else if (bytes_read > 0) { // now we readed from client_fd, set client_fd to POLLOUT (write) event
                        printf("cl.event.EPOLLIN.modify: to pollout\n");
                        epoll_mod(epoll_fd, cur_fd, EPOLLOUT);
                    } else {
                        printf("cl.event.EPOLLIN.ELSE\n");
                    }

                } else if (poll_events[i].events && EPOLLOUT) { // CAN WRITE
                    printf("cl.event.EPOLLOUT(bef): sended=%zd\n");
                    // send response
                    ssize_t sent = send(cur_fd, resp2, strlen(resp2), 0);
                    printf("cl.event.EPOLLOUT: sended=%zd\n", sent);
                } else {
                    printf("cl.event.ELSE(bef): ...\n");
                    int t = poll_events[i].events && EPOLLOUT;
                    printf("cl.event.ELSE(bef): result=%i\n", t);
                }


                //memset(rbuff, 0, 1024);
                //printf("FD.2.rec=%zd\n", bytes_read);
                /*
                printf("server.recv=%s\n", rbuff);
                int del = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cur_fd, &ev);

                if (del == -1) {
                    printf("epoll_ctl: EPOLL_CTL_DEL\n");
                    return -1;
                }

                printf("client=%i cur.fd=%i\n", client_fd, cur_fd);
                close(client_fd);
                */
            }
        }
    }

    printf("serv.end\n");

    return socket_fd;
}

int main() {
    printf("start5. !\n");

    printf("st=%u\n", EPOLLOUT);

    serverconfig config;
    config.server_ip = "127.0.0.1";
    config.server_max_con = 50;
    config.server_port = "8010";
    config.host_addr.ai_family = AF_INET;
    config.host_addr.ai_socktype = SOCK_STREAM;
    config.host_addr.ai_flags = AI_PASSIVE;
    config.host_addr.ai_protocol = IPPROTO_TCP;
    config.io.epoll_max_events = 50;

    server_start(&config);

    printf("end. !\n");
    return 0;
}