#ifndef COMMON_INCLUDE_OBC_HEAP_H_
#define COMMON_INCLUDE_OBC_HEAP_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 * 
 * @param size Size of memory to be allocated
 * @return void* Pointer to the beginning of the block of memory
 */
void *obcMalloc(size_t size);

/**
 * @brief Frees a block of memory previously allocated by a call to obcMalloc
 * 
 * @param ptr Pointer to the beginning of the block of memory to be freed
 */
void obcFree(void *ptr);

#endif /* COMMON_INCLUDE_OBC_HEAP_H_ */
