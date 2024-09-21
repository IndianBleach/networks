#ifndef _HTTP_REQUEST_
#define _HTTP_REQUEST_

#include "../include/core/core.h"
#include "../include/http/httpinfo.h"

// REQUEST BUFFER
typedef struct httprequest_buff {
    char *ptr;
    unsigned int capacity;
} httprequest_buff;

httprequest_buff *requestbuff_new(unsigned int cap);
void requestbuff_dstr(httprequest_buff *buff);
void requestbuff_copy(const char *source, httprequest_buff *dest);
void requestbuff_init(httprequest_buff *buff, unsigned int capacity);

/////////// HEADER
typedef enum header_value_type {
    HVAL_WORD,
    HVAL_PATH,
    HVAL_STRING,
    HVAL_IPADDR,
    HVAL_SERVER_ADDR,
    HVAL_VERSION,
    HVAL_INT,
    HVAL_FLOAT,
    HVAL_TAGVALUE,
    HVAL_LIST,
    UNSET,
} header_value_type;

typedef struct header_value {
    union {
        const char *single_value;

        struct {
            const char *tag;
            const char *value;
        } tag_value;

        tree *nested_list;
    };

    header_value_type type;
} header_value;

typedef struct httpheader {
    header_value value;
    const char *name;
} httpheader;

httpheader *httpheader_new(header_value_type __base_type);

void httpheader_dump(httpheader *t);

/////////// QUERY
typedef enum queryparam_value_type {
    QUERY_VALUE,
    QUERY_VALUE_LIST,
} queryparam_value_type;

typedef union queryparam_value {
    vector vec;
    char *value;
} queryparam_value;

typedef struct queryparam {
    queryparam_value_type type;
    queryparam_value value;
    char *tag_name;
} queryparam;

//queryparam *queryparam_new(queryparam_value_type type, int value_size, int tag_name_size);
//char *queryparam_get_value(queryparam *qp);
void queryparam_init(queryparam *param, queryparam_value_type type, char **tag_name, char **tag_value);
void queryparam_switch_on_list(queryparam *param, queryparam_value_type type);

/////////// REQUEST
typedef struct httprequest {
    vector *httpheaders;
    vector *queryparams;
    httppath_segment *path;
    httpmethod method;
    httpversion version;
    const char *body;
} httprequest;

void httprequest_dump(httprequest *req);
void httprequest_dstr(httprequest *req);

#endif