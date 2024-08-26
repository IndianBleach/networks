#include "../utils/types.h"
#include "sockets/socket.h"
#include <stdio.h>
#include "logging/logger.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> // close(socketf)

#define SOCKET_ERROR -1

int main() {
     
    socketinfo s;
    s.port = 9999;
    s.ip_addr = "74.125.235.20";

    printf("socket=%i %s", s.port, s.ip_addr);

    // create socket
    int socketId = socket(AF_INET, SOCK_STREAM, 0);
    if (socketId == SOCKET_ERROR) {
        log_err("socket() returned SOCKET_ERROR", "socket");
        return 1;
    }

    // bind 
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5064);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(socketId, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        log_err("bind() returned error < 0", "socket");
        return 1;
    }

    // listen
    listen(socketId, 1);

    int client = -1;
    int bytes_read = 0;
    char client_buff[1024];

    printf("listening\n");

    for(;;) {
        // accept client connections
        client = accept(socketId, NULL, NULL);
        if (client < 0) {
            log_err("can't accept client socket.", "socket");
            return 1;
        }

        while(1) {
            recv(socketId, client_buff, 1024, 0);
            if (bytes_read <= 0) break;
            //send()
        }

        close(client);
    }

    return 0;
}
