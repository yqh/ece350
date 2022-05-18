/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE350 RTX LAB  
 *
 *                     Copyright 2020-2022 Yiqing Huang
 *                          All rights reserved.
 *---------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------*/
 

/**************************************************************************//**
 * @file        HAL.c
 * @brief       Hardware Abstraction Layer 
 *              
 * @version     V1.2022.05
 * @authors     Yiqing Huang
 * @date        2022 MAY
 * @note        This file contains embedded assembly. 
 *              The code borrowed some ideas from ARM RL-RTX source code
 *
 *****************************************************************************/

#include "k_rtx.h"
#include "k_inc.h"

/**************************************************************************//**
 * @brief   	pop off exception stack frame from the stack
 * @pre         PSP is used in thread mode before entering any exception
 *              SVC_Handler is configured as the highest interrupt priority
 *****************************************************************************/
__asm void __rte(void)
{
    PRESERVE8
    EXPORT  SVC_RTE
SVC_RTE
    MVN     LR, #:NOT:0xFFFFFFFD    // set EXC_RETURN value, Thread mode, PSP
    BX      LR    
    ALIGN
}

/**************************************************************************//**
 * @brief   	SVC Handler
 * @pre         PSP is used in thread mode before entering SVC Handler
 *              SVC_Handler is configured as the highest interrupt priority
 *****************************************************************************/

void SVC_Handler(void)
{
    
    U8   svc_number;
    U32  ret  = RTX_OK;                 // default return value of a function
    U32 *args = (U32 *) __get_PSP();    // read PSP to get stacked args
    
    svc_number = ((S8 *) args[6])[-2];  // Memory[(Stacked PC) - 2]
    switch(svc_number) {
        case SVC_RTX_INIT:
            ret = k_rtx_init((RTX_SYS_INFO*) args[0], (TASK_INIT *) args[1], (int) args[2]);
            break;
        case SVC_MEM_ALLOC:
            ret = (U32) k_mpool_alloc(MPID_IRAM1, (size_t) args[0]);
            break;
        case SVC_MEM_DEALLOC:
            ret = k_mpool_dealloc(MPID_IRAM1, (void *)args[0]);
            break;
        case SVC_MEM_DUMP:
            ret = k_mpool_dump(MPID_IRAM1);
            break;
        case SVC_TSK_CREATE:
            ret = k_tsk_create((task_t *)(args[0]), (void (*)(void))(args[1]), (U8)(args[2]), (U32) (args[3]));
            break;
        case SVC_TSK_EXIT:
            k_tsk_exit();
            break;
        case SVC_TSK_YIELD:
            ret = k_tsk_yield();
            break;
        case SVC_TSK_SET_PRIO:
            ret = k_tsk_set_prio((task_t) args[0], (U8) args[1]);
            break;
        case SVC_TSK_GET:
            ret = k_tsk_get((task_t ) args[0], (RTX_TASK_INFO *) args[1]);
            break;
        case SVC_TSK_GETTID:
            ret = k_tsk_gettid();
            break;
        case SVC_TSK_LS:
            ret = k_tsk_ls((task_t *) args[0], (size_t) args[1]);
            break;
        case SVC_MBX_CREATE:
            ret = k_mbx_create((size_t) args[0]);
            break;
        case SVC_MBX_SEND:
            ret = k_send_msg((task_t) args[0], (const void *) args[1]);
            break;
        case SVC_MBX_SEND_NB:
            ret = k_send_msg_nb((task_t) args[0], (const void *) args[1]);
            break;
        case SVC_MBX_RECV:
            ret = k_recv_msg((void *) args[0], (size_t) args[1]);
            break;
        case SVC_MBX_RECV_NB:
            ret = k_recv_msg_nb((void *) args[0], (size_t) args[1]);
            break;
        case SVC_MBX_LS:
            ret = k_mbx_ls((task_t *) args[0], (size_t) args[1]);
            break;
        case SVC_MBX_GET:
            ret = k_mbx_get((task_t) args[0]);
            break;
        case SVC_RT_TSK_SET:
            ret = k_rt_tsk_set((TIMEVAL*) args[0]);
            break;
        case SVC_RT_TSK_SUSP:
            ret = k_rt_tsk_susp();
            break;
        case SVC_RT_TSK_GET:
            ret = k_rt_tsk_get((task_t) args[0], (TIMEVAL *) args[1]);
            break;
#ifdef ECE350_P1
        // The following are only for P1 memory testing purpose
        // Future deliverables do not provide the following sys calls to tasks
        case SVC_MEM2_ALLOC:
            ret = (U32) k_mpool_alloc(MPID_IRAM2, (size_t) args[0]);
            break;
        case SVC_MEM2_DEALLOC:
            ret = k_mpool_dealloc(MPID_IRAM2, (void *)args[0]);
            break;
        case SVC_MEM2_DUMP:
            ret = k_mpool_dump(MPID_IRAM2);
            break;
#endif
        default:
            ret = (U32) RTX_ERR;
    }
    
    args[0] = ret;      // return value saved onto the stacked R0
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

