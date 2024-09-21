#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STRING
char *cstr_init(int len) {
    char *p = (char *) malloc(sizeof(char) * (len + 1));
    p[len] = '\0';
    return p;
}

char *cstr_new(int len, char *buff) {
    char *p = (char *) malloc(sizeof(char) * (len + 1));
    p[len] = '\0';

    strncpy(p, buff, len);
    return p;
}

// FIND
int find_str(void **beginptr, void **endptr, int len, char *target, int (*compr)(void *, char *)) {
    for (int i = 0; i < len; i++) {
        void *ref = (beginptr[i]);
        printf("p=%p\n", ref);
    }

    return -1;
};
