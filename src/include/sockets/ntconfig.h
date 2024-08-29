#ifndef _SOCKETS_NTCONFIG
#define _SOCKETS_NTCONFIG

#define _GNU_SOURCE
#include "include/types.h"
#include <netdb.h>

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
    uint listeres_per_port;
    uint epoll_events_cap;
    uint max_listen_connections;
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

#endif