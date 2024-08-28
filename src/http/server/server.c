
#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
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

char resp2[] = "HTTP/1.0 200 OK\r\n"
               "Server: webserver-c\r\n"
               "Content-type: text/html\r\n\r\n"
               "<html>hello, world</html>\r\n";

#pragma region sockets.epoll

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

#pragma endregion

#pragma region sockets.fd.operations

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

    //printf("sock.write: buff=%p epoll=%i fd=%i\n", buf, epollfd, fd);
    //perror("last err: ");

    if (fd <= 0) {
        return;
    }

    nwrite = write(fd, buf, strlen(buf));
    //printf("sock.write.res: sended=%i\n", nwrite);
    if (nwrite == -1) {
        perror("err.do_write\n");
        close(fd);                        // Remember Close FD
        epoll_del(epollfd, fd, EPOLLOUT); // Delete Listening
    } else {
        close(fd);
    }
    //memset(buf, 0, 1024);
}

void fd_lsaccept(int epoll_fd, int server_fd) {
    // ACCEPT NEW
    //printf("cl.serv_handle_accept.\n", server_fd);
    int clfd;
    clfd = accept(server_fd, NULL, NULL);

    if (clfd < 0) {
        //printf("cl.serv_handle_accept<0\n");
        return;
    }

    //printf("cl.serv_handle_accept. cl=%i\n", clfd);

    int err;
    err = fcntl(server_fd, F_SETFL, O_NONBLOCK);
    assert(!err);

    epoll_add(epoll_fd, clfd, EPOLLIN | EPOLLOUT | EPOLLET);

    //perror("cl.accept: ");
}

#pragma endregion

#pragma region nt.configs
typedef struct ntnode_addr ntnode_addr;
typedef struct ntnode_scale ntnode_scale;
typedef struct ntserver_scale ntserver_scale;
typedef struct ntnode_config ntnode_config;
typedef struct addrinfo addrinfo;

struct ntnode_addr {
    const char *port;
    const char *ip;
};

struct ntnode_scale {
    uint sockets_per_port;
    uint epoll_events_cap;
};

struct ntserver_scale {
    uint servers_per_broker;
};

struct ntnode_config {
    const char *node_name;
    ntnode_addr addr;
    ntnode_scale scale;
    addrinfo host;
};

#pragma endregion

/*
    thread_t ntnode_up(ntnode_config)
    - run ntnode (other process)

    int ntnode_run()
    - run ntnode

    httpresponse
    -new()...
    -BUILDER
    httprequest
    -parse()..

    todo:
    - read long body
    - read long query
    - read long files (http.files)
    - write files


    command_prompt (console)
    server_driver
*/


// debug
struct server_debuginfo {
    pthread_mutex_t _lock;
    int total_requests;
} debuginfo;

void debuginfo_req_inc() {
    pthread_mutex_lock(&debuginfo._lock);
    debuginfo.total_requests++;
    pthread_mutex_unlock(&debuginfo._lock);
}

// ___

int server_start(serverconfig *config) {
    pthread_t tid = pthread_self();
    printf("node.%lu started.\n", tid);

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

    // socket config
    int reuse = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));

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
        //printf("epoll.new\n");
        int ret = epoll_wait(epoll_fd, poll_events, config->io.epoll_max_events, -1);
        //printf("epoll.ret=%i\n", ret);

        if (ret == -1) {
            perror("epoll_wait: \n");
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

        printf("debug.request_count=%i\n", debuginfo.total_requests);
    }

    return socket_fd;
}

pthread_t lsnode_run(serverconfig *config) {
    pthread_t t;
    pthread_create(&t, NULL, server_start, config);
    return t;
}

int main() {
    // thread.a (new socket, ls)
    // thread.b (new socket, ls)

    serverconfig config;
    config.server_ip = "127.0.0.1";
    config.server_max_con = 50;
    config.server_port = "8012";
    config.host_addr.ai_family = AF_INET;
    config.host_addr.ai_socktype = SOCK_STREAM;
    config.host_addr.ai_flags = AI_PASSIVE;
    config.host_addr.ai_protocol = IPPROTO_TCP;
    config.io.epoll_max_events = 50;

    //server_start(&config);

    pthread_t t1 = lsnode_run(&config);
    pthread_t t2 = lsnode_run(&config);

    pthread_join(t1, NULL);
    printf("node thread end.\n");
    pthread_join(t2, NULL);
    printf("node thread end.\n");

    printf("end. !\n");
    return 0;
}