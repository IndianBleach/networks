#ifndef _HTTP_INFO_H_
#define _HTTP_INFO_H_

#include <stdbool.h>
#include <unistd.h>

typedef enum http_statuscode http_statuscode;

enum http_statuscode {
    CONTINUE = 100,

    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,

    TEMP_REDIRECT = 307,
    PERM_REDIRECT = 308,

    CLIENT_BAD_REQUEST = 400,
    CLIENT_UNAUTHORIZED = 401,
    CLIENT_FORBIDDEN = 403,
    NOT_FOUND = 404,

    ERR_INTERNAL = 500,
    ERR_BAD_GATEWAY = 502,
    ERR_SERVER_IS_DOWN = 521,
    ERR_CONNECTION_TIMEOUT = 522,
    ERR_SSL_HANDSHAKE = 525,
    ERR_SSL_INVALID_CERT = 526
};

typedef enum httpmethod {
    HTTP_GET,
    HTTP_POST,
    HTTP_DELETE,
    HTTP_PUT,
    HTTP_METHOD_UNDF = -1,
} httpmethod;

/////////// PATH
typedef struct httppath_segment {
    char *value;
    struct httppath_segment *next;
} httppath_segment;

httppath_segment *pathsegment_new(int value_size, char *value);

// PORT
typedef unsigned int nt_port;
typedef const char *nt_ipstr;

/////////// ADDR
typedef struct nt_addrstr {
    nt_ipstr ip;
    nt_port port;
} nt_addrstr;


// IP
typedef struct nt_ip nt_ip;

struct nt_ip {
    unsigned int seg1;
    unsigned int seg2;
    unsigned int seg3;
    unsigned int seg4;
};


// ADDR
typedef struct nt_hostaddr nt_hostaddr;

struct nt_hostaddr {
    nt_ip ip;
    nt_port port;
};

// VERSION
typedef struct nt_version nt_version;

struct nt_version {
    const char *value;
};


#endif