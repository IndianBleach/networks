#include "../include/http/workers.h"

#include "../include/sockets/epollfd.h"


void http_send_response(httprequest *request, int fd_epoll, int fd_client, SSL *ssl);
void http_receive_request(int fd_epoll, int fd_client, SSL *ssl);

void http_handle_in(int fd_epoll, int fd_client, SSL *ssl) {
    printf("[http_handle_in]: IN; FD=%i\n", fd_epoll);
    if (fork() == 0) {
        // children

        string *rstring = StringLarge("");
        String_clear(rstring);

        fdssl_read(fd_epoll, fd_client, rstring, ssl);

        String_append(rstring, "\n----------------------------\n");

        printf("READ=%s\n", String_c_str(rstring));
        String_dstr(rstring);

        // fdssl_read
        // fdssl_write

        //SSL_free(ssl);

        exit(1);
    }

    printf("[http_handle_in]: ret\n");
}