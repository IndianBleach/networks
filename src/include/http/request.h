#ifndef _HTTP_REQUEST_
#define _HTTP_REQUEST_

#include "../include/core/vector.h"
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
    VALUE,
    LIST_VALUE,
    LIST_VALUE_DEEP,
    TAG_VALUE,
} header_value_type;

typedef union header_value {
    const char *value;

    struct {
        const char *tag;
        const char *value;
    } tagvalue;

    // pepresenting list like: accept: 'eg, gz, er;  v=0.9'
    //                                  __value__,  __next__
    //                                 ..next->next.
    struct {
        size_t count;
        union header_value *value;
        union header_value *next;
    } list;


} header_value;

typedef struct httpheader {
    header_value_type type;
    header_value value;
    const char *name;
} httpheader;

/////////// QUERY
typedef enum queryparam_value_type {
    QUERY_VALUE,
    QUERY_VALUE_LIST,
} queryparam_value_type;

typedef union queryparam_value {
    struct {
        char *value;
        union queryparam_value *next;
    } list;

    char *value;
} queryparam_value;

typedef struct queryparam {
    queryparam_value_type type;
    queryparam_value value;
    char *tag_name;
} queryparam;

queryparam *queryparam_new(queryparam_value_type type, int value_size, int tag_name_size);
char *queryparam_get_value(queryparam *qp);
void queryparam_init(queryparam *param, queryparam_value_type type, int value_size, int tag_name_size);

/////////// REQUEST
typedef struct httprequest {
    vector *httpheaders;
    vector *queryparams;
    httppath_segment *path;
    httpmethod method;
    nt_addrstr host_addr;
    const char *body;
} httprequest;

#endif