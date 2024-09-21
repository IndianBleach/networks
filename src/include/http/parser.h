#ifndef _HTTP_PARSER_
#define _HTTP_PARSER_

#include "../include/http/request.h"

// PARSING
typedef struct parse_context {
    int cursor;
    char *buff;
    int end;
} parse_context;

void ctx_move(parse_context *ctx, int steps);
char ctx_at(parse_context *ctx, int pos);

int is_enum_value(parse_context *ctx, int local_end);
int is_float(parse_context *ctx, int len);

int get_string(parse_context *ctx);
int get_word(parse_context *ctx);
int get_word_ext(parse_context *ctx);
int get_word_version(parse_context *ctx);
int get_number(parse_context *ctx);
int get_version(parse_context *ctx);
int get_ipaddr(parse_context *ctx);
int getm_path(parse_context *ctx);
int get_tag(parse_context *ctx, char delimiter);
int get_querytag_value(parse_context *ctx);

// SPEC
int get_fulladdr(parse_context *ctx);
int get_tagvalue(parse_context *ctx, char **__out_value, char **__out_tagname);

// PRE-PARSING
httpmethod extract_method(parse_context *ctx);
int extract_path(parse_context *ctx, httppath_segment **__out_pathhead);
int extract_httpversion(parse_context *ctx, httpversion *__out_version);
int extract_queryparams(parse_context *ctx, vector *__out_vec_qparams);

// PARSING
bool parse_request(httprequest_buff *buff, httprequest *__out_req);
bool parse_headers(parse_context *ctx, vector *__out_headers);

#endif