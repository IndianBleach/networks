
#ifndef _HTTP_RESPONSE_
#define _HTTP_RESPONSE

#include "../include/http/httpinfo.h"
#include "../include/core/str.h"

typedef struct httpresponse httpresponse;

string* response_get_content(httpresponse* r);
http_statuscode response_get_statuscode(httpresponse* r);

#endif


// init
// 