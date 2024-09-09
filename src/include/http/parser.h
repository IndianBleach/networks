#ifndef _HTTP_PARSER_
#define _HTTP_PARSER_

#include "../include/http/request.h"

// TOKEN
typedef enum httpreq_tokentype {

    enum_value_begin,
    enum_value_end,

    enum_delim_comma,
    enum_delim_semicolon,


    word_path,
    word,

    header_tag,
    ipaddr,


    delim_semicolon,
    delim_dot,
    delim_space,
    delim_comma,

    pair_tag,
    pair_value,

    string,
    path,
    symbol,
    port,

    number,
    number_float,
    version,

    _parse_begin,
    _parse_end
} httpreq_tokentype;

typedef struct httpreq_token {
    httpreq_tokentype type;
    const char *value;
    struct httpreq_token *next;
} httpreq_token;

httpreq_token *token_new(const char *value, httpreq_tokentype type);
httpreq_token *token_new_at(httpreq_tokentype type, char *buff, int len);
void token_dump(httpreq_token *tk);

// PARSING
typedef struct parse_context {
    int cursor;
    char *buff;
    int end;
} parse_context;

void parse(httprequest_buff *buff);

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
int get_tag(parse_context *ctx);
int get_querytag_value(parse_context *ctx);

// PRE-PARSING
httpmethod extract_method(parse_context *ctx);
int extract_path(parse_context *ctx, httppath_segment **__out_pathhead);
int extract_httpversion(parse_context *ctx, httpversion *__out_version);
int extract_queryparams(parse_context *ctx, vector *__out_vec_qparams);

// token
// token_list

// parse_headers
// parse_body


#endif