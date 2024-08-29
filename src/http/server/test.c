#include "../include/sockets/epoll.h"
#include "../include/sockets/epollfd.h"
#include "../include/sockets/ntconfig.h"
#include "../include/sockets/ntsocket.h"

#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define SCALE_EPOLL_EVENTS_CAP 1000
#define SCALE_SOCK_LISTENER_MAX_CON 1000
#define SCALE_LS_PER_NODE 2
#define ADDR_NODE_IP "127.0.0.1"
#define ADDR_NODE_PORT "8011"

int main() {
    printf("test.start\n");

    ntnode_config config;
    ntnode_scale node_scale;
    node_scale.epoll_events_cap = SCALE_EPOLL_EVENTS_CAP;
    node_scale.listeres_per_port = SCALE_LS_PER_NODE;
    node_scale.max_listen_connections = SCALE_SOCK_LISTENER_MAX_CON;
    config.scale = node_scale;
    config.addr.ip = ADDR_NODE_IP;
    config.addr.port = ADDR_NODE_PORT;
    config.host.ai_family = AF_INET;
    config.host.ai_protocol = IPPROTO_TCP;
    config.host.ai_socktype = SOCK_STREAM;
    config.host.ai_flags = AI_PASSIVE;
    config.node_name = "node.a";

    tid *td;

    ntsock_io_up(&config, &td, 1);

    pthread_join(td, NULL);

    printf("test.end\n");

    return 0;
}