#include "../include/http/httpinfo.h"

#include "../include/core/coredef.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

httppath_segment *pathsegment_new(int value_size, char *value) {
    //httppath_segment *seg = OBJ_NEW(httppath_segment);
    httppath_segment *seg = (httppath_segment *) malloc(sizeof(httppath_segment));
    seg->next = NULL;
    seg->value = OBJ_NEWC(char, (value_size + 1));
    seg->value[value_size] = '\0';
    //seg->value = strdup(value);
    printf("pathsegment_new: size=%i new=%s\n", value_size, value);
    //printf("pathsegment_new: value=%s size=%i ptr=%p\n", value, value_size, seg->value);
    strncpy(seg->value, value, value_size);

    //printf("pathsegment_new=%s ptr=%p\n", seg->value, seg->value);

    return seg;
};
