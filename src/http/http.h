#define MAX_REQ_LINE_SZ 128

enum http_method { GET, POST, PUT, END };

enum http_ver { VER_1_1, VER_2 };

// extern enum http_method methods_lookup[K_M_FIN];

typedef struct {
    enum http_method method;
    char *path;
    enum http_ver ver;
    char *headers;

} http_req;
