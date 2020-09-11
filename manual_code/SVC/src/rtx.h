/* @brief: rtx.h User API header file. Do not modify
 * @author: Yiqing Huang
 * @date: 2020/09/03
 */
#ifndef _RTX_H_
#define _RTX_H_

/*----- Includes -----*/
#include "common.h"

#define __SVC_0  __svc_indirect(0)


extern int k_mem_init(size_t blk_size, int algo);
#define mem_init(blk_size, algo) _mem_init((U32)k_mem_init, blk_size, algo);
extern int _mem_init(U32 p_func, size_t blk_size, int algo) __SVC_0;

extern void *k_mem_alloc(size_t size);
#define mem_alloc(size) _mem_alloc((U32)k_mem_alloc, size);
extern void *_mem_alloc(U32 p_func, size_t size) __SVC_0;

extern int k_mem_dealloc(void *);
#define mem_dealloc(ptr) _mem_dealloc((U32)k_mem_dealloc, ptr)
extern int _mem_dealloc(U32 p_func, void *ptr) __SVC_0;

extern int k_mem_count_extfrag(size_t size);
#define mem_count_extfrag(size) _mem_count_extfrag((U32)k_mem_count_extfrag, size);
extern int _mem_count_extfrag(U32 p_func, size_t size) __SVC_0;


#endif // !_RTX_H_
