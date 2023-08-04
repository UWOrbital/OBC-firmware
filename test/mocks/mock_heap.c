#include <stdlib.h>

void *sysMalloc(size_t size) { return malloc(size); }

void sysFreeMem(void *ptr) { free(ptr); }
