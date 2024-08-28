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
#include "../include/vector.h"
#include "http.h"
#include "token.h"

#define move() (_move(p))
#define peek() (_peek(p))
#define is_end() (_is_end(p))
#define peek_next() (_peek_next(p))

typedef struct {
    vector *tokens;
    size_t cursor, start;
} parser;

int _is_end(parser *p);
token *_move(parser *p);
token *_peek(parser *p);
token *_peek_next(parser *p);

void parser_init(parser *p, vector *tkns);
void parser_destroy(parser *p);

void parse_req(http_req *, char *);
int parse_req_line(http_req *, char **);
void parse_headers(http_req *, char **);
void parse_body(http_req *, char **);
