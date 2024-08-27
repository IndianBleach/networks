#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_REQ_LINE_SZ 128

enum http_method { GET, POST, UNKNWN, END };

enum http_ver { VER_1_1, VER_2 };

extern const char *methods_lookup[END];

typedef struct {
    enum http_method method;
    char *path;
    enum http_ver ver;
    char *headers;

} http_req;

void parse_req(http_req *, char *);
int parse_req_line(http_req *, char **);
void parse_headers(http_req *, char **);
void parse_body(http_req *, char **);
