#include "../utils/types.h"
#include "logging/logger.h"
#include "sockets/socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // close(socketf)

#define SOCKET_ERROR -1
#define SOCKET_PORT 3750

char msg[] = "ping!\n";
char rbuff[1024];

// return thread_id
void *server_start() {
    // create socket
    int socketId = socket(AF_INET, SOCK_STREAM, 0);
    if (socketId == SOCKET_ERROR) {
        log_err("socket() returned SOCKET_ERROR", "socket");
    }

    // bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCKET_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketId, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        log_err("bind() returned error < 0", "socket");
    }

    // listen
    listen(socketId, 1);

    int client = -1;
    ssize_t bytes_read = 0;
    char client_buff[1024];

    //const char* body = "<title>tt</title>\n<h1>Test page</h1>\n";
    const char *resp = "HTTP/1.1 200 OK\r\n \
        Version: HTTP/1.1\r\n \
        Content-Type: text/html; charset=utf-8\r\n \
        Content-Length: 47\n\0";

    for (;;) {
        //sleep(1);

        client = accept(socketId, NULL, NULL);
        if (client < 0) {
            log_err("can't accept client socket.", "socket");
            break;
        }

        while (1) {
            bytes_read = recv(client, client_buff, 1024, 0);
            if (bytes_read <= 0)
                break;
            send(client, resp, 124, 0);
        }
    }

    //close(client);

    return NULL;
}

void background_run(pthread_t *thread, void *(*funcptr)(void *) ) { pthread_create(thread, NULL, funcptr, NULL); }

void *client_start() {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        log_err("socket() return SOCKET_ERROR -1", "socket");
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCKET_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        log_err("connect() return SOCKET_ERROR -1", "socket");
    }

    for (;;) {
        sleep(1);

        send(sock, msg, sizeof(msg), 0);
        recv(sock, rbuff, 1024, 0);
        printf("recv=%s\n", rbuff);
    }

    close(sock);
}

int main() {
    pthread_t server;
    background_run(&server, server_start);

    pthread_t client;
    background_run(&client, client_start);

    pthread_join(server, NULL);
    pthread_join(client, NULL);

    return 0;
}
