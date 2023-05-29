#include "obc_heap.h"
#include <stdlib.h>


/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 * 
 * @param size Size of memory to be allocated
 */
void *obcMalloc(size_t size) {
    return malloc(size);
}

/**
 * @brief Frees a block of memory previously allocated by a call to obcMalloc
 * 
 * @param ptr Pointer to the beginning of the block of memory to be freed
 */
void obcFree(void *ptr) {
    free(ptr);
}
