

#define OBJ_NEW(struct_name) (struct_name*)malloc(sizeof(struct_name))
#define OBJ_NEWC(struct_name, count) (struct_name*)malloc(sizeof(struct_name) * count)