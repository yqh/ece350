/* @brief: rtx.h User API header file. Do not modify
 * @author: Yiqing Huang
 * @date: 2020/10/05
 */
#ifndef _RTX_H_
#define _RTX_H_

/*----- Includes -----*/
#include "common.h"

#define __SVC_0  __svc_indirect(0)

/* __SVC_0 can be put at the end of the function declaration */
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

/* Note __SVC_0 can also be put in the front of the function name*/
/*task manamgement */
extern void k_rtx_init(size_t blk_size, int algo, RTX_TASK_INFO *tsk_info, int num_tasks);
#define rtx_init(blk_size, algo, tsk_info, num_tasks) _rtx_init((U32)k_rtx_init, blk_size, algo, tsk_info, num_tasks)
extern void __SVC_0 _rtx_init(U32 p_func, size_t blk_size, int algo, RTX_TASK_INFO *tsk_info, int num_tasks);

extern int k_tsk_yield(void);
#define tsk_yield() _tsk_yield((U32)k_tsk_yield)
extern int __SVC_0 _tsk_yield(U32 p_func);

extern int k_tsk_create(task_t *task, void (*task_entry)(void), U8 prio, U16 stack_size);
#define tsk_create(task, task_entry, prio, stack_size) _tsk_create((U32)k_tsk_create, task, task_entry, prio, stack_size)
extern int __SVC_0 _tsk_create(U32 p_func, task_t *task, void (*task_entry)(void), U8 prio, U16 stack_size);

extern void k_tsk_exit(void);
#define tsk_exit() _tsk_exit((U32)k_tsk_exit)
extern void __SVC_0 _tsk_exit(U32 p_func);

extern int k_tsk_set_prio(task_t task_id, U8 prio);
#define tsk_set_prio(task_id, prio) _tsk_set_prio((U32)k_tsk_set_prio, task_id, prio)
extern int __SVC_0 _tsk_set_prio(U32 p_func, task_t task_id, U8 prio);

extern int k_tsk_get(task_t task_id, RTX_TASK_INFO *buffer);
#define tsk_get(task_id, buffer) _tsk_get((U32)k_tsk_get, task_id, buffer)
extern int __SVC_0 _tsk_get(U32 p_func, task_t task_id, RTX_TASK_INFO *buffer);

extern int k_tsk_ls(task_t *buf, int count);
#define tsk_ls(buf, count) _tsk_ls((U32)k_tsk_ls, buf, count);
extern int __SVC_0 _tsk_ls(U32 p_func, task_t *buf, int count);



/* message passing */
extern int k_mbx_create(ssize_t size);
#define mbx_create(size) _mbx_create((U32)k_mbx_create, size)
extern int __SVC_0 _mbx_create(U32 p_func, size_t size);

extern int k_send_msg(task_t tid, const void* buf);
#define send_msg(tid, buf) _send_msg((U32)k_send_msg, tid, buf)
extern int __SVC_0 _send_msg(U32 p_func, task_t tid, const void *buf);

extern int k_recv_msg(task_t *tid, void *buf, size_t len);
#define recv_msg(tid, buf, len) _recv_msg((U32)k_recv_msg, tid, buf, len)
extern int __SVC_0 _recv_msg(U32 p_func, task_t *tid, void *buf, size_t len);

extern int k_mbx_ls(task_t *buf, int count);
#define mbx_ls(buf, count) _mbx_ls((U32)k_mbx_ls, buf, count);
extern int __SVC_0 _mbx_ls(U32 p_func, task_t *buf, int count);
#endif // !_RTX_H_
