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
 * @file        k_inc.h
 * @brief       Kernel Macros and Data Structure Header file
 *              l2
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @note        all kernel .c files include this one
 *
 *****************************************************************************/

#ifndef K_INC_H_
#define K_INC_H_

#include "LPC17xx.h"
#include "lpc1768_mem.h"
#include "common.h"
#include "rtx_errno.h"
#include "uart_polling.h"
#include "printf.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */

#define NUM_TASKS 3     // only supports three tasks in the starter code 
                        // due to limited user stack space
/*
 *===========================================================================
 *                             STRUCTURES
 *===========================================================================
 */
/**
 * @brief TCB data structure definition to support two kernel tasks.
 * @note  You will need to modify this data structure!!!
 */
// The following offset macros needs to be modified if you modify
// the positions of msp field in the TCB structure
#define TCB_MSP_OFFSET  8       // TCB.msp offset 

typedef struct tcb {
    struct tcb *prev;         /**< prev tcb, not used in the starter code     */
    struct tcb *next;         /**< next tcb, not used in the starter code     */
    U32        *msp;          /**< kernel sp of the task, TCB_MSP_OFFSET = 8  */
    U8          priv;         /**< = 0 unprivileged, =1 privileged,           */    
    U8          tid;          /**< task id                                    */
    U8          prio;         /**< scheduling priority                        */
    U8          state;        /**< task state                                 */
} TCB;

/*
 *===========================================================================
 *                             GLOBAL VARIABLES 
 *===========================================================================
 */
extern int  errno;      // defined in k_rtx_init.c file

// Memory related globals are defined in k_mem.c
// kernel stack size
extern const U32 g_k_stack_size;    // kernel stack size
extern const U32 g_p_stack_size;    // process stack size

// task kernel stacks are statically allocated inside the OS image
extern U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));

// process stack for tasks, statically allocated inside the OS image  */
//extern U32 g_p_stacks[MAX_TASKS][PROC_STACK_SIZE >> 2] __attribute__((aligned(8)));
extern U32 g_p_stacks[NUM_TASKS][PROC_STACK_SIZE >> 2] __attribute__((aligned(8)));

// task related globals are defined in k_task.c
extern TCB *gp_current_task;    // always point to the current RUNNING task

// TCBs are statically allocated inside the OS image
extern TCB g_tcbs[MAX_TASKS];
extern TASK_INIT g_null_task_info;
extern U32 g_num_active_tasks;	// number of non-dormant tasks */

#endif  // !K_INC_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
