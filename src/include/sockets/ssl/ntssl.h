#ifndef _SOCKETS_NTSSL_H
#define _SOCKETS_NTSSL_H

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

void ssl_init();

void ssl_dstr();

void ssl_shutdown(SSL *ssl);

#endif