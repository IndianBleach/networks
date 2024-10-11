#include <stddef.h>
struct shared_mem_t;
typedef int bool_t;

struct shared_mem_t shared_mem_new();

void shared_mem_delete(struct shared_mem_t* obj);

void shared_mem_ctor(struct shared_mem_t* obj,
    const char* name,
    size_t size);

void shared_mem_dtor(struct shared_mem_t* obj);

char* shared_mem_getptr(struct shared_mem_t* obj );

bool_t shared_mem_isowner(struct shared_mem_t* obj);

void shared_mem_setowner(struct shared_mem_t* obj, bool_t owner);

