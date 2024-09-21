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

/*
GET /favicon.ico HTTP/1.1
Host: 127.0.0.1:8013
Connection: keep-alive
sec-ch-ua: "Not/A)Brand";v="8", "Chromium";v="126", "YaBrowser";v="24.7", "Yowser";v="2.5"
sec-ch-ua-mobile: ?0
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 YaBrowser/24.7.0.0 Safari/537.36
sec-ch-ua-platform: "Windows"
Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,_/*;q=0.8
Sec-Fetch-Site: same-origin
Sec-Fetch-Mode: no-cors
Sec-Fetch-Dest: image
Referer: https://127.0.0.1:8013/
Accept-Encoding: gzip, deflate, br, zstd
Accept-Language: ru,en;q=0.9
*/


int main2() {
    printf("test2.start\n");

    //
    //char *t = "Referer: https://127.0.0.1:8013\nHost: 127.0.0.1:8013\nAccept-Encoding: gzip, deflate, br, "
    //          "zstd\nAccept: image/avif,image/webp,image/apng,image/svg+xml\nsec-ch-ua-platform: \"Windows\"";
    //GET /user HTTP/1.1

    char *t =
        "GET /user?name=john&age=23 HTTP/1.1\nReferer: https://127.0.0.1:8013\nHost: "
        "127.0.0.1:8013\nAccept-Encoding: gzip, "
        "deflate; br, time\nAccept: image/avif,image/webp,image/apng,image/svg+xml\nsec-ch-ua-platform: \"Windows\" ";

    httprequest_buff buff;
    buff.ptr = t;
    buff.capacity = 1024;

    parse_context ctx;
    ctx.buff = t;
    ctx.cursor = 0;
    ctx.end = strlen(t);

    httprequest req;
    parse_request(&buff, &req);

    /*
        vector qparams;
    vector_init(&qparams, 16, sizeof(queryparam *));

    // clean query params
    int res = extract_queryparams(&ctx, &qparams);
    printf("FREE\n");
    queryparam *prm1 = *(queryparam **) vector_at(&qparams, 0);
    queryparam *prm = *(queryparam **) vector_at(&qparams, 1);
    free(prm1->tag_name);
    free(prm1->value.value);
    free(prm1);
    for (size_t i = 0; i < prm->value.vec.size; i++) {
        queryparam_value *elem = vector_at(&prm->value.vec, i);
        printf("destr=%s\n", elem->value);
        free(elem->value);
    }
    free(prm->tag_name);
    vector_dstr(&prm->value.vec);
    free(prm);
    vector_dstr(&qparams);
    */
}
