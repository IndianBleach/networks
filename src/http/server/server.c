
#define _GNU_SOURCE

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define IP "127.0.0.1"
#define PORT "8003"

// ___________ server.config.io
typedef struct {
    int epoll_max_events;
} serverconfig_io;

// ___________ server.config
typedef struct {
    struct addrinfo host_addr;
    serverconfig_io io;
    const char *server_port;
    const char *server_ip;
    int server_max_con;
} serverconfig;

int server_start(serverconfig *config) {
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
        perror("socket\n");
        return -1;
    }

    // bind
    if (bind(socket_fd, parseaddr->ai_addr, parseaddr->ai_addrlen)) {
        perror("bind\n");
        return -1;
    }

    freeaddrinfo(parseaddr);

    if (listen(socket_fd, config->server_max_con) == -1) {
        perror("listen");
        return -1;
    }

    int client_fd;
    ssize_t bytes_read;
    char rbuff[1024];

    char resp2[] = "HTTP/1.0 200 OK\r\n"
                   "Server: webserver-c\r\n"
                   "Content-type: text/html\r\n\r\n"
                   "<html>hello, world</html>\r\n";

    int addrlen = sizeof(config->host_addr);

    printf("listenning...\n");
    for (;;) {
        // accept any client
        client_fd = accept(socket_fd, NULL, NULL);

        if (client_fd < 0) {
            perror("accept\n");
            continue;
        }

        bytes_read = recv(client_fd, rbuff, 1024, 0);
        if (bytes_read <= 0) {
            continue;
        }
        printf("server.recv=%s\n", rbuff);

        ssize_t sent = send(client_fd, resp2, strlen(resp2), 0);
        printf("server.sent3=%zd\n", sent);
        printf("serv.client-close\n");
        close(client_fd);
    }


    printf("serv.end\n");

    return socket_fd;
}

int main() {
    printf("start. !\n");

    serverconfig config;
    config.server_ip = IP;
    config.server_max_con = 50;
    config.server_port = PORT;
    config.host_addr.ai_family = AF_INET;
    config.host_addr.ai_socktype = SOCK_STREAM;
    config.host_addr.ai_flags = AI_PASSIVE;
    config.host_addr.ai_protocol = IPPROTO_TCP;
    config.io.epoll_max_events = 50;

    server_start(&config);

    printf("end. !\n");
    return 0;
}
