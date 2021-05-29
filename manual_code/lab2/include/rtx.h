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
 * @version     V1.2021.04
 * @authors     Yiqing Huang
 * @date        2021 APR 
 * @see         common.h
 * @see         rtx_ext.h
 *
 * @attention   DO NOT MODIFY
 *
 *****************************************************************************/

#ifndef _RTX_H_
#define _RTX_H_

#include "rtx_ext.h"
#include "common.h"
#include "lpc1768_mem.h"

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
 * Memory Management Functions - LAB2
 *------------------------------------------------------------------------*/
 
/* __SVC_0 can be put at the end of the function declaration */
/* memory management */

extern void *k_mpool_alloc(mpool_t mpid, size_t size);
#define mem_alloc(size) _mem_alloc((U32)k_mpool_alloc, MPID_IRAM1, size)
extern void *_mem_alloc(U32 p_func, mpool_t mpid, size_t size) __SVC_0;

extern int   k_mpool_dealloc(mpool_t mpid, void *ptr);
#define mem_dealloc(ptr) _mem_dealloc((U32)k_mpool_dealloc, MPID_IRAM1, ptr)
extern int _mem_dealloc(U32 p_func, mpool_t mpid, void *ptr) __SVC_0;

extern int   k_mpool_dump(mpool_t mpid);
#define mem_dump() _mem_dump((U32)k_mpool_dump, MPID_IRAM1)
extern int _mem_dump(U32 p_func, mpool_t mpid) __SVC_0;
 
/*------------------------------------------------------------------------*
 * System Initialization Function(s) - LAB2, LAB4, LAB5
 *------------------------------------------------------------------------*/

/* Note __SVC_0 can also be put in the front of the function name*/
/*task management */
extern int   k_rtx_init(RTX_SYS_INFO *sys_info, TASK_INIT *tasks, int num_tasks);
#define rtx_init(sys_info, tasks, num_tasks) _rtx_init((U32)k_rtx_init, sys_info, tasks, num_tasks)
extern int __SVC_0 _rtx_init(U32 p_func, RTX_SYS_INFO *sys_info, TASK_INIT *tasks, int num_tasks);

extern int k_get_sys_info(RTX_SYS_INFO *buffer);
#define get_sys_info(buffer) _get_sys_info((U32)k_get_sys_info, buffer)
extern int __SVC_0 _get_sys_info(U32 p_func, RTX_SYS_INFO *buffer);

extern int   k_pre_rtx_init     (void *args);
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

extern task_t k_tsk_gettid(void);
#define tsk_gettid() _tsk_gettid((U32)k_tsk_gettid)
extern task_t __SVC_0 _tsk_gettid(U32 p_func);

#endif // !_RTX_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

