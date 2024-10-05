#ifndef _HTTP_WORKERS_
#define _HTTP_WORKERS_

#include "../include/http/request.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

// epoll_fd, client_fd, rstring, cssl

void http_send_response(httprequest* request, int fd_epoll, int fd_client, SSL* ssl);
void http_receive_request(int fd_epoll, int fd_client, SSL* ssl);
void http_handle_in(int fd_epoll, int fd_client, SSL* ssl);

#endif