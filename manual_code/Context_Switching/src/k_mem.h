/**
 * @file:   k_mem.h
 * @brief:  kernel memory managment header file
 * @author: Yiqing Huang
 * @date:   2020/09/20
 */
 
#ifndef K_MEM_H_
#define K_MEM_H_

#include "k_rtx.h"

/* ----- Definitions ----- */
#define IRAM1_END 0x10008000

/* ----- Variables ----- */
/* This symbol is defined by linker (see ARM Linker User Guide in Arm Compiler 5 Documentation) */   
extern U32 Image$$RW_IRAM1$$ZI$$Limit;

/* ----- Functions ------ */
int k_mem_init(size_t blk_size, int algo);
void *k_mem_alloc(size_t size);
void k_mem_dealloc(void *ptr);
int k_mem_count_extfrag(size_t size);

#endif /* ! K_MEM_H_ */
