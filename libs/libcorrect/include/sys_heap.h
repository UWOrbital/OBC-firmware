#ifndef COMMON_INCLUDE_OBC_HEAP_H_
#define COMMON_INCLUDE_OBC_HEAP_H_

#include <stddef.h>
#include <stdint.h>

// TODO: Remove this file once libcorrect is updated to not use malloc/free

/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 * 
 * @param size Size of memory to be allocated
 * @return void* Pointer to the beginning of the block of memory
 */
void *sysMalloc(size_t size);

/**
 * @brief Frees a block of memory previously allocated by a call to sysMalloc
 * 
 * @param ptr Pointer to the beginning of the block of memory to be freed
 */
void sysFreeMem(void *ptr);

#endif /* COMMON_INCLUDE_OBC_HEAP_H_ */
