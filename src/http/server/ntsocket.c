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

// ssl
#define SSL_PUBKEY_PATH "/home/gcreep/github.local/networks/ssl/local.pem"
#define SSL_PRIVKEY_PATH "/home/gcreep/github.local/networks/ssl/local.pem"

#define REQUEST_BUFF_CAP 1024

#include "../include/http/request.h"
#include "../include/sockets/ssl/ntssl.h"

//#include "openssl-master/ms/applink.c"


int file_exists(const char *ln) {
    if (access(ln, F_OK) == 0) {
        return 1;
    } else {
        return -1;
    }
}

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
        perror("ntsock_listen.getaddrinfo: ");
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

    printf("ntsock_liste=ok. socket_fd=%i\n", socket_fd);
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

    // ssl
    SSL_CTX *sslctx;
    SSL *cssl;
    ssl_init();
    sslctx = SSL_CTX_new(SSLv23_server_method());
    // ssl.options
    SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);

    printf("ssl.init=ok\n");

    if (file_exists(SSL_PUBKEY_PATH) == -1) {
        printf("ssl.err=FILE_NO_EXISTS, SSL_PUBKEY\n");
        ssl_dstr();
    }


    if (file_exists(SSL_PRIVKEY_PATH) == -1) {
        printf("ssl.err=FILE_NO_EXISTS, SSL_PRIVKEY\n");
        ssl_dstr();
    }

    // ssl.certificates
    int use_cert = SSL_CTX_use_certificate_file(sslctx, SSL_PUBKEY_PATH, SSL_FILETYPE_PEM);
    int use_pkey = SSL_CTX_use_PrivateKey_file(sslctx, SSL_PRIVKEY_PATH, SSL_FILETYPE_PEM);

    printf("ssl.use: cert=%i pkey=%i\n", use_cert, use_pkey);

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
                //fd_lsaccept(epoll_fd, socket_fd);

                cssl = SSL_new(sslctx);
                printf("ssl.new()\n");

                fdssl_accept(epoll_fd, socket_fd, sslctx, cssl);
            } else if (poll_events[i].events & EPOLLIN) {
                printf("node.%lu: EPOLLIN.\n", tid);
                fdssl_read(epoll_fd, client_fd, rbuff, cssl);
                printf("readed=%s\n", rbuff);
                //fdssl_read(epoll_fd, client_fd, rbuff, cssl);
            } else if (poll_events[i].events & EPOLLOUT) {
                printf("node.%lu: EPOLLOUT.\n", tid);
                fdssl_write(epoll_fd, client_fd, resp2, cssl);
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