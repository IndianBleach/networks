#include "../include/http/httpinfo.h"


// REQUEST BUFFER

typedef struct httpreq_buff httpreq_buff;
struct httpreq_buff {
    char* ptr;
    unsigned int capacity;
};

httpreq_buff*   reqbuff_crt(unsigned int cap);
void            reqbuff_dstr(httpreq_buff* buff);


// REQUEST
typedef struct httprequest httprequest;
struct httprequest {
    http_method http_method;
    http_pathnode http_path;
    nt_version http_version;
    nt_hostaddr host;
    http_token* connection;
    http_token* user_agent;
    http_token* accept;
    http_token* accept_language;
    http_token* content_type;
    http_token* content_length;
};

httprequest*    httpreq_crt();
void            httpreq_dstr(httprequest* req);