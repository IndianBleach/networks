#include "parser.h"

const char *methods_lookup[END] = {
    [GET] = "GET",
    [POST] = "POST",
    [UNKNWN] = "UNKNWN",
};

void parse_req(http_req *sr, char *req) {
    parse_req_line(sr, &req);
}

int parse_req_line(http_req *sr, char **req) {
    return 0;
}
