/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
 *                          All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice and the following disclaimer.
 *
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************
 */

/**************************************************************************//**
 * @file        rtx.h
 * @brief       RTX User API header file
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 * @see         common.h
 * @see         common_ext.h
 *
 * @attention   DO NOT MODIFY
 *
 *****************************************************************************/

#ifndef _RTX_H_
#define _RTX_H_

#include "common.h"
#include "common_ext.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */
#define __SVC_0  __svc_indirect(0)

/*
 *===========================================================================
 *                             FUNCTION PROTOTYPES
 *===========================================================================
 */

/*------------------------------------------------------------------------*
 * Memory Management Functions - LAB1
 *------------------------------------------------------------------------*/

/* __SVC_0 can be put at the end of the function declaration */
/* memory management */
extern int k_mem_init(void);
#define mem_init() _mem_init((U32)k_mem_init)
extern int _mem_init(U32 p_func) __SVC_0;

extern void *k_mem_alloc(size_t size);
#define mem_alloc(size) _mem_alloc((U32)k_mem_alloc, size)
extern void *_mem_alloc(U32 p_func, size_t size) __SVC_0;

extern int k_mem_dealloc(void *);
#define mem_dealloc(ptr) _mem_dealloc((U32)k_mem_dealloc, ptr)
extern int _mem_dealloc(U32 p_func, void *ptr) __SVC_0;

extern int k_mem_count_extfrag(size_t size);
#define mem_count_extfrag(size) _mem_count_extfrag((U32)k_mem_count_extfrag, size)
extern int _mem_count_extfrag(U32 p_func, size_t size) __SVC_0;

/*------------------------------------------------------------------------*
 * System Initialization Function(s) - LAB2, LAB4, LAB5
 *------------------------------------------------------------------------*/

/* Note __SVC_0 can also be put in the front of the function name*/
/*task management */
extern int k_rtx_init(RTX_TASK_INFO *tsk_info, int num_tasks);
#define rtx_init(tsk_info, num_tasks) _rtx_init((U32)k_rtx_init, tsk_info, num_tasks)
extern int __SVC_0 _rtx_init(U32 p_func, RTX_TASK_INFO *tsk_info, int num_tasks);

extern int k_rtx_init_rt(RTX_SYS_INFO *sys_info, RTX_TASK_INFO *task_info, int num_tasks);
#define rtx_init_rt(sys_info, task_info, num_tasks) _rtx_init_rt((U32)k_rtx_init_rt, sys_info, task_info, num_tasks)
extern int __SVC_0 _rtx_init_rt(U32 p_func, RTX_SYS_INFO *sys_info, RTX_TASK_INFO *task_info, int num_tasks);

extern int k_get_sys_info(RTX_SYS_INFO *buffer);
#define get_sys_info(buffer) _get_sys_info((U32)k_get_sys_info, buffer)
extern int __SVC_0 _get_sys_info(U32 p_func, RTX_SYS_INFO *buffer);

/*------------------------------------------------------------------------*
 * Task Management Functions - LAB2, LAB4, LAB5
 *------------------------------------------------------------------------*/

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

/*------------------------------------------------------------------------*
 * Real-Time Task Functions - LAB4, LAB5
 *------------------------------------------------------------------------*/

extern int k_tsk_create_rt(task_t *tid, TASK_RT *task, RTX_MSG_HDR *msg_hdr, U32 num_msgs);
#define tsk_create_rt(tid, task, msg_hdr, num_msgs) _tsk_create_rt((U32)k_tsk_create_rt, tid, task, msg_hdr, num_msgs)
extern int __SVC_0 _tsk_create_rt(U32 p_func, task_t *tid, TASK_RT *task, RTX_MSG_HDR *msg_hdr, U32 num_msgs);

extern void k_tsk_done_rt(void);
#define tsk_done_rt() _tsk_done_rt((U32)k_tsk_done_rt)
extern void __SVC_0 _tsk_done_rt(U32 p_func);

extern void k_tsk_suspend(struct timeval_rt *tv);
#define tsk_suspend(tv) _tsk_suspend((U32) k_tsk_suspend, tv)
extern void __SVC_0 _tsk_suspend(U32 p_func, struct timeval_rt *tv);


/*------------------------------------------------------------------------*
 * Interprocess Communication Functions - LAB3, LAB4
 *------------------------------------------------------------------------*/

extern int k_mbx_create(size_t size);
#define mbx_create(size) _mbx_create((U32)k_mbx_create, size)
extern int __SVC_0 _mbx_create(U32 p_func, size_t size);

extern int k_send_msg(task_t tid, const void* buf);
#define send_msg(tid, buf) _send_msg((U32)k_send_msg, tid, buf)
extern int __SVC_0 _send_msg(U32 p_func, task_t tid, const void *buf);

extern int k_recv_msg(task_t *tid, void *buf, size_t len);
#define recv_msg(tid, buf, len) _recv_msg((U32)k_recv_msg, tid, buf, len)
extern int __SVC_0 _recv_msg(U32 p_func, task_t *tid, void *buf, size_t len);

extern int k_recv_msg_nb(task_t *tid, void *buf, size_t len);
#define recv_msg_nb(tid, buf, len) _recv_msg_nb((U32)k_recv_msg_nb, tid, buf, len)
extern int __SVC_0 _recv_msg_nb(U32 p_func, task_t *tid, void *buf, size_t len);

extern int k_mbx_ls(task_t *buf, int count);
#define mbx_ls(buf, count) _mbx_ls((U32)k_mbx_ls, buf, count);
extern int __SVC_0 _mbx_ls(U32 p_func, task_t *buf, int count);

/*------------------------------------------------------------------------*
 * Timing Service Functions - LAB4
 *------------------------------------------------------------------------*/

extern int k_get_time(struct timeval_rt *tv);
#define get_time(tv) _get_time((U32)k_get_time, tv)
extern int __SVC_0 _get_time(U32 p_func, struct timeval_rt *tv);


#endif // !_RTX_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
