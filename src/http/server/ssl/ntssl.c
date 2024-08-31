#include "../include/sockets/ssl/ntssl.h"

void ssl_init() {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

void ssl_dstr() {
    ERR_free_strings();
    EVP_cleanup();
}

void ssl_shutdown(SSL *ssl) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
}