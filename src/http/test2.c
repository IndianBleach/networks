#define _GNU_SOURCE

#include "../include/http/parser.h"
#include "../include/http/request.h"
#include "../include/sockets/epoll.h"
#include "../include/sockets/epollfd.h"
#include "../include/sockets/ntconfig.h"
#include "../include/sockets/ntsocket.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

// tokens


/*
TODO:
    ----------------------
            HTTP REQUEST

    void make_headers(httptoken_list, vector<http_header>*)

    -----------------------
            FILE-CONTROLLER-VIEW
    -определить роуты? по файлу
    -смотреть на путь и возвращать html страницу, если такая определена, подгружать зависимости
    -как отправлять файлы
    -----------------------
    HTTP RESPONSE
    -file/text
    -metadata
    -status_codes
    -type
    
    -----------------------
            CLI (с++)
    commands

    -----------------------
            CACHE
    sessions/ connections/ ssl?



*/


/*  
    TODO!
    vector<query_param> parse_query(query_string)
    vector<headers> parse_headers(request_buffer)
    const char* get_body(request_buffer)

    ->> parse_request(request_buffer)

    httprequest
        -vector<headervalue> headers;
        -long_string body
        -vector<httpquery_param> params; query_parser
        -char* path; look(tag=path)
        -httpmethod method;  look(tag=method)
        -ipadrr addr; look(tag=host)
*/

// extract_queryparams
// comparator
// alg functions + generics

#include <stdio.h>
#include <stdlib.h>

void foo(int **t) {
    *t = NULL;
    *t = (int *) malloc(sizeof(int));
}

#include "../include/core/coredef.h"

typedef struct usr usr;

struct usr {
    const char *name;
};

usr *crt(const char *name) {
    usr *p = (usr *) malloc(sizeof(usr));
    p->name = name;
    return p;
}

int usr_comp(usr *a, char *name) {
    if (strcmp(a->name, name) == 0)
        return 0;
    else
        return -1;
}

int main() {
    printf("test2.start\n");

    char *t = "?user=john&age=21";

    parse_context ctx;
    ctx.buff = t;
    ctx.cursor = 0;
    ctx.end = strlen(t);

    //vector extract_queryparams(&ctx, )

    /*
    parse_context ctx;
    ctx.buff = t;
    ctx.cursor = 0;
    ctx.end = strlen(t);

    int t1 = get_ipaddr(&ctx);

    printf("RES=%i\n", t1);
    return 0;

    httpreq_buff *reqbuff = reqbuff_new(1024);

    reqbuff_copy(t, reqbuff);


    parse(reqbuff);

    reqbuff_dstr(reqbuff);

    */
}