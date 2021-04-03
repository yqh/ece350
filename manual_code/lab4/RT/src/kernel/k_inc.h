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

#include "device_a9.h"
#include "common.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */

#define TCB_MSP_OFFSET  4

/*
 *===========================================================================
 *                             STRUCTURES
 *===========================================================================
 */

/**
 * @brief TCB data structure definition to support two kernel tasks.
 * @note  You will need to add more fields to this structure.
 */
typedef struct tcb {
    struct tcb          *next;   /**> next tcb, not used in this example         */
    U32                 *msp;    /**> msp of the task, TCB_MSP_OFFSET = 4        */
    void                (*ptask)();         /**> task entry address                 */
    U8                  prio;   /**> Execution priority                         */
    U32                 k_sp;               /**> current kernel stack pointer       */
    U8                  state;  /**> task state                                 */
    U32                 k_stack_hi;         /**> kernel stack base (high addr.)     */
    U8                  priv;   /**> = 0 unprivileged, =1 privileged            */
    U32                 u_sp;               /**> current user stack pointer         */
    U32                 u_stack_hi;         /**> user stack base addr. (high addr.) */
    U16                 k_stack_size;       /**> kernel stack size in bytes         */
    U16                 u_stack_size;       /**> user stack size in bytes           */
    task_t              tid;                /**> task ID                            */
    struct timeval_rt   tv_cpu;             /**> task execution cpu time            */
    struct timeval_rt   tv_wall;            /**> task execution wall clock time     */

    /* The following only applies to real-time tasks */
    struct timeval_rt   p_n;                /**> period in seconds and microseconds */
    RTX_MSG_HDR        *msg_hdr;            /**> real-time task message header      */
    U32                 num_msgs;           /**> real-time task mailbox capacity    */
    U32                 suspended_time_remaining; /* how much time left the task will be suspended for in nanoseconds */
    struct tcb         *next_suspended; 
} TCB;

typedef struct suspended_task_list {
    TCB *front;
} STL;

typedef struct msg {
    // meta data for the kernel to navigate and monitor the ring buffer
    struct msg 			*next;
    task_t 				sender_id;
} MSG;

typedef struct rtx_mailbox {
    //fields for mailbox
    unsigned int        *mb_buffer;         /* mailbox buffer */
    unsigned int        *mb_buffer_end;     /* end of mailbox buffer */
    int                 mb_capacity;       /* size of the mailbox */
    struct msg          *mb_head;           /* pointer to head of mailbox */
    struct msg          *mb_tail;           /* pointer to tial of mailbox */
} TMB;

/*
 *==========================================================================
 *                   GLOBAL VARIABLES DECLARATIONS
 *==========================================================================
 */
// Memory related globals are defined in k_mem.c
// kernel stack size
extern const U32 g_k_stack_size;    // kernel stack size
extern const U32 g_p_stack_size;    // process stack size for sys mode tasks

// task kernel stacks are statically allocated inside the OS image
extern U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));

// process stack for tasks in SYS mode, statically allocated inside the OS image  */
extern U32 g_p_stacks[MAX_TASKS][PROC_STACK_SIZE >> 2] __attribute__((aligned(8)));

extern unsigned int Image$$ZI_DATA$$ZI$$Limit; 	// Linker defined symbol
                                                // See ARM Compiler User Guide 5.x

// task related globals are defined in k_task.c
extern TCB *gp_current_task;    // always point to the current RUNNING task

// TCBs are statically allocated inside the OS image
extern TCB g_tcbs[MAX_TASKS];
extern RTX_TASK_INFO g_null_task_info;
extern U32 g_num_active_tasks;	// number of non-dormant tasks */

#endif // ! K_INC_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
