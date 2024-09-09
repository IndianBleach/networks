

#ifndef _COREDEF_H
#define _COREDEF_H

#define OBJ_NEW(struct_name) (struct_name *) malloc(sizeof(struct_name))
#define OBJ_NEWC(struct_name, count) (struct_name *) malloc(sizeof(struct_name) * count)

char *cstr_init(int len);
char *cstr_new(int len, char *buff);

int find_str(void **beginptr, void **endptr, int len, char *target, int (*compr)(void *, char *));

#endif
