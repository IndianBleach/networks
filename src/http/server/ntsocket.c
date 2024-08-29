#include "../include/sockets/ntsocket.h"

#include "../include/sockets/epoll.h"
#include "../include/sockets/epollfd.h"
#include "../include/types.h"

#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

// fix: sepr (httpresponse)
char resp2[] = "HTTP/1.0 200 OK\r\n"
               "Server: webserver-c\r\n"
               "Content-type: text/html\r\n\r\n"
               "<html>hello, world</html>\r\n";

/// FN: sock_listen(ntnode_config *config)
/// PARAMS: config='config with ip, port, scaling and epoll metadata'
/// DESC: запускает и начинает прослушивать сокет на указанном в <config> адресе
/// RET: socket file descriptor (socket_fd)
int ntsock_listen(ntnode_config *config) {
    pthread_t tid = pthread_self();
    printf("node.%lu started.\n", tid);

    //printf("config.name=%s\n", config->node_name);

    struct addrinfo *host_addr_parsed;
    int status;
    if ((status = getaddrinfo(config->addr.ip, config->addr.port, &config->host, &host_addr_parsed)) == -1) {
        fprintf(stderr, "sock_listen.getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    // create socket
    int socket_fd;
    if ((socket_fd =
             socket(host_addr_parsed->ai_family, host_addr_parsed->ai_socktype, host_addr_parsed->ai_protocol)) == -1) {
        perror("sock_listen.socket:");
        return -1;
    }

    // hard config
    int reuse = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));

    if (bind(socket_fd, host_addr_parsed->ai_addr, host_addr_parsed->ai_addrlen)) {
        perror("sock_listen.bind:");
        freeaddrinfo(host_addr_parsed);
        return -1;
    }

    freeaddrinfo(host_addr_parsed);

    if (listen(socket_fd, config->scale.max_listen_connections) == -1) {
        perror("sock_listen.listen: ");
        return -1;
    }

    return socket_fd;
};

/// FN: sock_up_single(tid*)
/// PARAMS: tid='thread to be initialized'
/// DESC: запускает в отдельном потоке epoll прослушивателя для сокета
/// RET:
void ntsock_io_run(ntnode_config *config) {
    pthread_t tid = pthread_self();
    printf("node.[%s]> sock.up() thread=%lu.\n", config->node_name, tid);

    //printf("config.name=%s\n", config->node_name);
    int socket_fd = ntsock_listen(config);

    // config.epoll
    struct epoll_event ev;
    struct epoll_event poll_events[config->scale.epoll_events_cap];
    int epoll_fd;
    int nfds;

    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("ntnode_run.epoll_create1:");
        return -1;
    }

    // fix: sepr
    int client_fd;
    ssize_t bytes_read;
    char rbuff[1024];

    // add event (listener (server))
    epoll_add(epoll_fd, socket_fd, EPOLLIN);

    while (1) {
        int ret = epoll_wait(epoll_fd, poll_events, config->scale.epoll_events_cap, -1);
        if (ret == -1) {
            perror("ntnode_run.epoll_wait: ");
        }

        for (int i = 0; i < ret; i++) {
            client_fd = poll_events[i].data.fd;
            if ((client_fd == socket_fd) && (poll_events[i].events & EPOLLIN)) {
                printf("node.%lu: ACCEPT.\n", tid);
                fd_lsaccept(epoll_fd, socket_fd);
            } else if (poll_events[i].events & EPOLLIN) {
                printf("node.%lu: EPOLLIN.\n", tid);
                fd_read(epoll_fd, client_fd, rbuff);
            } else if (poll_events[i].events & EPOLLOUT) {
                printf("node.%lu: EPOLLOUT.\n", tid);
                fd_write(epoll_fd, client_fd, resp2);
                debuginfo_req_inc();
            }
        }

        //printf("debug.request_count=%i\n", debuginfo.total_requests);
    }

    return socket_fd;
}

/// FN: sock_up_parallel(tid* tids, int count) [scaling.port]
/// PARAMS: tids='threads to be initialized' count='tids count'
/// DESC: запускает в отдельном потоке epoll прослушивателя для сокета
/// RET:
void ntsock_io_up(ntnode_config *config, tid *tids, int count) {
    for (int i = 0; i < count; i++) {
        pthread_create(&tids[i], NULL, ntsock_io_run, config);
    }
}