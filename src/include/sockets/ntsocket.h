
#ifndef _SOCKETS_NTSOCKET
#define _SOCKETS_NTSOCKET

#include "include/sockets/ntconfig.h"
#include "include/types.h"

int sock_listen(ntnode_config *config);

void sock_up_single(ntnode_config *config);

void sock_up_parallel(ntnode_config *config, tid *tids, int count);

#endif