
#ifndef _SOCKETS_NTSOCKET
#define _SOCKETS_NTSOCKET

#include "../include/sockets/ntconfig.h"
#include "../include/types.h"

int ntsock_listen(ntnode_config *config);

void ntsock_io_run(ntnode_config *config);

void ntsock_io_up(ntnode_config *config, tid *tids, int count);

#endif