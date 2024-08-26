

#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


log_err(const char* msg, const char* tag) {
    time_t now;
    time(&now);
    printf("[%s ERR] (%s): %s", ctime(&now), tag, msg);
}

log_warn(const char* msg, const char* tag) {
    time_t now;
    time(&now);
    printf("[%s WRN] (%s): %s", ctime(&now), tag, msg);
}

log_info(const char* msg, const char* tag) {
    time_t now;
    time(&now);
    printf("[%s INFO] (%s): %s", ctime(&now), tag, msg);
}



