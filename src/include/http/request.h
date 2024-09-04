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

// REQUEST
/*
typedef struct httprequest httprequest;
struct httprequest_header {
    http_method http_method;
    http_pathnode http_path;
    nt_version http_version;
    nt_hostaddr host;
    http_token *connection;
    http_token *user_agent;
    http_token *accept;
    http_token *accept_language;
    http_token *content_type;
    http_token *content_length;
};

httprequest *httpreq_new();
void httpreq_dstr(httprequest *req);


// todo
void httpreq_init(httprequest *request, httpreq_buff *buff);
*/


#endif