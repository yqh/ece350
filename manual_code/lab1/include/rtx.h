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
 *                             FUNCTION MACROS
 *===========================================================================
 */
 
/*------------------------------------------------------------------------*
 * Memory Management Functions - LAB1
 *------------------------------------------------------------------------*/
//#define mem_init(algo)          k_mem_init(algo)

#ifdef RTX_IRAM1
#define mem_alloc(size)        mem1_alloc(size_t)
#define mem_dealloc(ptr)       mem1_dealloc(ptr)
#define mem_dump()             mem1_dump()
#else
#define mem_alloc(size)        mem2_alloc(size)
#define mem_dealloc(ptr)       mem2_dealloc(ptr)
#define mem_dump()             mem2_dump()
#endif

#define mem1_alloc(size)       k_mpool_alloc(MPID_IRAM1, size)
#define mem1_dealloc(ptr)      k_mpool_dealloc(MPID_IRAM1, ptr)
#define mem1_dump()            k_mpoo1_dump(MPID_IRAM1)

#define mem2_alloc(size)       k_mpool_alloc(MPID_IRAM2, size)
#define mem2_dealloc(ptr)      k_mpool_dealloc(MPID_IRAM2, ptr)
#define mem2_dump()            k_mpool_dump(MPID_IRAM2)

/*------------------------------------------------------------------------*
 * System Initialization Function(s) - LAB1
 *------------------------------------------------------------------------*/

//#define pre_rtx_init(args) k_pre_rtx_init(arg)
#define rtx_init(sys_info, tsk_info, num_tasks) k_rtx_init(sys_info, tsk_info, num_tasks)
#define get_sys_info(buffer)    k_get_sys_info(buffer)


/*
 *===========================================================================
 *                             FUNCTION PROTOTYPES
 *===========================================================================
 */
extern void *k_mpool_alloc      (mpool_t mpid, size_t size);
extern int   k_mpool_dealloc    (mpool_t mpid, void *ptr);
extern int   k_mpool_dump       (mpool_t mpid);
extern int   k_pre_rtx_init     (void *args);
extern int   k_rtx_init         (RTX_SYS_INFO *sys_info, TASK_INIT *task, int num_tasks);
extern int   k_get_sys_info     (RTX_SYS_INFO *buffer);
#endif // !_RTX_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

