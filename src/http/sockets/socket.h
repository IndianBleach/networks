#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <stdio.h>

typedef struct socketinfo socketinfo;

struct socketinfo {
    u_short port;
    const char *ip_addr;
};

#endif