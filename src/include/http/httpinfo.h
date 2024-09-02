

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

enum http_method {
    POST,
    GET,
    DELETE,
    PUT
};

// HTTP_PATH
typedef struct http_pathnode http_pathnode;
struct http_pathnode {
    struct http_pathnode* next;
    bool is_file;
    const char* value;
};

// IP
typedef struct nt_ip nt_ip;
struct nt_ip {
    unsigned int seg1;
    unsigned int seg2;
    unsigned int seg3;
    unsigned int seg4;
};

// PORT
typedef unsigned int nt_port;

// ADDR
typedef struct nt_hostaddr nt_hostaddr;
struct nt_hostaddr {
    nt_ip ip;
    nt_port port;
};

// VERSION
typedef struct nt_version nt_version;
struct nt_version {
    const char* value;
};

// TYPE
namespace http_token_value {
    enum type {
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
        word,
        ip,
        port,

        number,
        number_float,
        version
    };
}

// TOKEN
struct http_token {
    http_token_value::type type;
    http_token* next;
    const char* value;
};