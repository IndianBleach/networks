#ifndef _HTTP_REQUEST_
#define _HTTP_REQUEST_

#include "../include/http/httpinfo.h"


// REQUEST BUFFER
typedef struct httpreq_buff httpreq_buff;

struct httpreq_buff {
    char *ptr;
    unsigned int capacity;
};

httpreq_buff *reqbuff_new(unsigned int cap);
void reqbuff_dstr(httpreq_buff *buff);
void reqbuff_copy(const char *source, httpreq_buff *dest);

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
        header_value *value;
        header_value *next;
    } list;


} header_value;

typedef struct httpheader {
    header_value_type type;
    header_value value;
    const char *name;
} httpheader;

/////////// QUERY
typedef enum queryparam_value_type {
    LIST_VALUES,
    VALUE,
} queryparam_value_type;

typedef union queryparam_value {
    struct {
        const char *value;
        queryparam_value *next;
    } list;

    const char *value;
} queryparam_value;

typedef struct queryparam {
    queryparam_value_type type;
    queryparam_value value;
} queryparam;

/////////// REQUEST
typedef struct httprequest {
    vector *httpheaders;
    vector *queryparams;
    httppath_head *path;
    httpmethod method;
    nt_addrstr host_addr;
    const char *body;
} httprequest;

#endif