#ifndef _K_MEM_H_
#define _K_MEM_H_
#include "common.h"

#define IRAM1_END 0x10008000

int k_mem_init(size_t blk_size, int algo);
void *k_mem_alloc(size_t size);
void k_mem_dealloc(void *ptr);
int k_mem_count_extfrag(size_t size);
#endif // ! _K_MEM_H_
