#pragma once

/*
 * HTTP request grammar
 *
 * request = method path http-version ;
 *
 * method = "GET" / "POST" / "CONNECT" / ... / "PUT" ;
 * version = "HTTP","/",("1.1" | "2.0")
 * path             = path-abempty    ; begins with "/" or is empty
                    / path-absolute   ; begins with "/" but not "//"
                    / path-noscheme   ; begins with a non-colon segment
                    / path-rootless   ; begins with a segment
                    / path-empty      ; zero characters

      path-abempty  = *( "/" segment )
      path-absolute = "/" [ segment-nz *( "/" segment ) ]
      path-noscheme = segment-nz-nc *( "/" segment )
      path-rootless = segment-nz *( "/" segment )
      path-empty    = 0<pchar>

      segment       = *pchar
      segment-nz    = 1*pchar
      segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
                    ; non-zero-length segment without any colon ":"
      pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
*/

#include <stdio.h>
#include <stdlib.h>
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
