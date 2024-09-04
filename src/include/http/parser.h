#ifndef _HTTP_PARSER_
#define _HTTP_PARSER_

#include "../include/http/httpinfo.h"

// TYPE
typedef enum httpreq_tokentype httpreq_tokentype;

enum httpreq_tokentype {

    enum_value_begin,
    enum_value_end,

    word_path,
    word,

    header_tag,

    enum_delim_semicolom,
    enum_delim_space,
    enum_delim_comma,

    delim_semicolon,
    delim_dot,
    delim_space,
    delim_comma,

    pair_tag,
    pair_value,

    string,
    path,
    symbol,
    ip,
    port,

    number,
    number_float,
    version,

    _parse_begin,
    _parse_end
};

// TOKEN


// token
// token_list

// parse_headers
// parse_body


#endif