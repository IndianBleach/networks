
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

/*

    httpapi.handle(request, &httpresponse);
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
};

#include <pthread.h>

/*
struct ntnode_api {
    tid *listeners;
    const char *node_name;
};
*/


/*
    ntnode_api
        tid* listeners;
        node_name name;

*/


/*
    ntnode_api* ntnode_up(tid*)
    запускает ноду в отдельном потоке <tid>
    нода запускает в свою очередь множество прослушивателей на сокет согласно настройке <config>
    ret: ntnode_api с информацией о ноде
*/


int ntnode_run(ntnode_config *config) {}

void ntnode_up(pthread_t *tid, ntnode_config *config) { pthread_create(tid, NULL, ntnode_run, config); }

#define SCALE_EPOLL_EVENTS_CAP 1000
#define SCALE_SOCK_LISTENER_MAX_CON 1000
#define SCALE_LS_PER_NODE 2
#define ADDR_NODE_IP "127.0.0.1"
#define ADDR_NODE_PORT "8011"

int main() {
    // thread.a (new socket, ls)
    // thread.b (new socket, ls)

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

    sock_up_single(&config);

    //server_start(&config);
    /*
    pthread_t t1 = lsnode_run(&config);
    pthread_t t2 = lsnode_run(&config);

    pthread_join(t1, NULL);
    printf("node thread end.\n");
    pthread_join(t2, NULL);
    printf("node thread end.\n");

*/
    printf("end. !\n");
    return 0;
}