/* @brief: rtx.h User API header file. Do not modify
 * @author: Yiqing Huang
 * @date: 2020/09/03
 */
#ifndef _RTX_H_
#define _RTX_H_

/*----- Includes -----*/
#include "common.h"

#define __SVC_0  __svc_indirect(0)

/* memory management */
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

/*task manamgement */
extern void k_rtx_init(size_t blk_size, int algo, RTX_TASK_INFO *tsk_info);
#define rtx_init(blk_size, algo, tsk_info) _rtx_init((U32)k_rtx_init, blk_size, algo, tsk_info)
extern void __SVC_0 _rtx_init(U32 p_func, size_t blk_size, int algo, RTX_TASK_INFO *tsk_info);

extern int k_tsk_yield(void);
#define tsk_yield() _tsk_yield((U32)k_tsk_yield)
extern int __SVC_0 _tsk_yield(U32 p_func);


extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

extern void *k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void *_request_memory_block(U32 p_func) __SVC_0;
/* __SVC_0 can also be put at the end of the function declaration */

extern int k_release_memory_block(void *);
#define release_memory_block(p_mem_blk) _release_memory_block((U32)k_release_memory_block, p_mem_blk)
extern int _release_memory_block(U32 p_func, void *p_mem_blk) __SVC_0;

#endif // !_RTX_H_
