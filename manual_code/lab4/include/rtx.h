/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2022 Yiqing Huang
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
 * @version     V1.2022.05
 * @authors     Yiqing Huang
 * @date        2022 MAY 
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

/*
 *===========================================================================
 *                             FUNCTION PROTOTYPES
 *===========================================================================
 */
 
extern int   k_pre_rtx_init     (void *args);

__svc(SVC_RTX_INIT)     int     rtx_init(RTX_SYS_INFO *sys_info, TASK_INIT *tasks, int num_tasks);
__svc(SVC_MEM_ALLOC)    void   *mem_alloc(size_t size);
__svc(SVC_MEM_DEALLOC)  int     mem_dealloc(void *ptr);
__svc(SVC_MEM_DUMP)     int     mem_dump(void);
__svc(SVC_TSK_CREATE)   int     tsk_create(task_t *task, void (*task_entry)(void), U8 prio, U32 stack_size);
__svc(SVC_TSK_EXIT)     void    tsk_exit(void);
__svc(SVC_TSK_YIELD)    int     tsk_yield(void);
__svc(SVC_TSK_SET_PRIO) int     tsk_set_prio(task_t task_id, U8 prio);
__svc(SVC_TSK_GET)      int     tsk_get(task_t task_id, RTX_TASK_INFO *buffer);
__svc(SVC_TSK_GETTID)   task_t  tsk_gettid(void);
__svc(SVC_TSK_LS)       int     tsk_ls(task_t *buf, size_t count);
__svc(SVC_MBX_CREATE)   int     mbx_create(size_t size);
__svc(SVC_MBX_SEND)     int     send_msg(task_t tid, const void* buf);
__svc(SVC_MBX_SEND_NB)  int     send_msg_nb(task_t tid, const void* buf);
__svc(SVC_MBX_RECV)     int     recv_msg(void *buf, size_t len);
__svc(SVC_MBX_RECV_NB)  int     recv_msg_nb(void *buf, size_t len);
__svc(SVC_MBX_LS)       int     mbx_ls(task_t *buf, size_t count);
__svc(SVC_MBX_GET)      int     mbx_get(task_t tid);
__svc(SVC_RT_TSK_SET)   int     rt_tsk_set(TIMEVAL *p_tv);
__svc(SVC_RT_TSK_SUSP)  int     rt_tsk_susp(void);
__svc(SVC_RT_TSK_GET)   int     rt_tsk_get(task_t task_id, TIMEVAL *buffer);
#endif // !_RTX_H_


#ifdef ECE350_P1
__svc(SVC_MEM2_ALLOC)    void   *mem2_alloc(size_t size);
__svc(SVC_MEM2_DEALLOC)  int     mem2_dealloc(void *ptr);
__svc(SVC_MEM2_DUMP)     int     mem2_dump(void);
#endif

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

