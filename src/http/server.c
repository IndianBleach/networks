#include "parser.h"

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "4221"
#define BACKLOG 5

int get_server_socket(void) {
    int status, server_fd;
    struct addrinfo hints = {0};
    struct addrinfo *res = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) == -1) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        return -1;
    }

    if ((server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        perror("socket");
        return -1;
    }
    const int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse) == -1) {
        perror("setsockopt");
        return -1;
    }
    if (bind(server_fd, res->ai_addr, res->ai_addrlen)) {
        perror("bind");
        return -1;
    }
    freeaddrinfo(res);
    if (listen(server_fd, BACKLOG)) {
        perror("listen");
        return -1;
    }
    return server_fd;
}

void test() {
    char *r = "POST / HTTP/1.1";
    http_req req = {0};
    int err = parse_req_line(&req, &r);
    printf("%d\n", req.method);
}

int main() {
    test();
    return 1;
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    int server_fd = get_server_socket();
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof peer_addr;

    printf("Waiting for a client to connect...\n");

    int peer_fd;
    peer_fd = accept(server_fd, (struct sockaddr *) &peer_addr, &peer_addr_len);
    printf("Client connected\n");

    char responce[] = "HTTP/1.1 200 OK\r\n\r\n";
    size_t rsize = strlen(responce);
    ssize_t sent;
    if ((sent = send(peer_fd, responce, strlen(responce), 0)) <= 0) {
        if (sent == -1) {
            perror("sent");
        } else if (sent == 0) {
            fprintf(stderr, "server: client disconnected\n");
        } else if (sent < rsize) {
            fprintf(stderr, "sent != rsize\n");
        }
    }

    close(server_fd);

    return 0;
}
