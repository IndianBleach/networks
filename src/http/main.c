#include <stdio.h>
#include "logging/logger.h"

int main() {
    
    log_err("some error", "global");
    printf("temp\n");

    return 0;
}
