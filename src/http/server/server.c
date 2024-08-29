
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
