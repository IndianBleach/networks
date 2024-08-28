#pragma once
#include <stdlib.h>
#include <string.h>
#include "token.h"

#define MAX_REQ_LINE_SZ 128

// typedef char* trusted_str;

enum http_method { GET, POST, PUT, END };

enum http_ver { VER_1_1, VER_2 };

// extern enum http_method methods_lookup[K_M_FIN];

typedef struct {
    enum http_method method;
    char *path;
    enum http_ver ver;
    char *headers;
} http_req;

void req_destroy(http_req *r) {
    free(r->path);
}
void req_fill_path(http_req *r, trusted_str p) {
    r->path = (char*) malloc(strlen(p) + 1);
    strcpy(r->path, p);
}
