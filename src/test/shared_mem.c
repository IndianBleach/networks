#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

typedef int bool_t;

bool_t owner_process_set = FALSE;
bool_t owner_process = FALSE;

typedef struct {
    char *name;
    int shm_fd;
    void *map_ptr;
    char *ptr;
    size_t size;
} shared_mem_t;

shared_mem_t *shared_mem_new() { return (shared_mem_t *) malloc(sizeof(shared_mem_t)); }

void shared_mem_delete(shared_mem_t *obj) {
    free(obj->name);
    free(obj);
}

void shared_mem_ctor(shared_mem_t *obj, const char *name, size_t size) {
    obj->size = size;
    obj->name = (char *) malloc(strlen(name) + 1);
    strcpy(obj->name, name);
    obj->shm_fd = shm_open(obj->name, O_RDWR, 0600);
    if (obj->shm_fd >= 0) {
        if (!owner_process_set) {
            owner_process = FALSE;
            owner_process_set = TRUE;
        }
    }
}