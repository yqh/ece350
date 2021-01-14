/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
 *
 *          This software is subject to an open source license and
 *          may be freely redistributed under the terms of MIT License.
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
#if 0
#define TCB_MSP_OFFSET  4
#define TCB_PSP_OFFSET  8

/*
 *===========================================================================
 *                             STRUCTURES
 *===========================================================================
 */

/**
 * @brief TCB data structure definition to support two kernel tasks.
 * @note  You will need to add more fields to this structure.
 */
typedef struct tcb
{
    struct tcb *next;       /**> next tcb, not used in this example         */
    U32        *msp;        /**> msp of the task, TCB_MSP_OFFSET = 4        */
    U32        *psp;        /**> psp of the task, TCB_MSP_OFFSET = 8        */
    U8          tid;        /**> task id                                    */
    U8          prio;       /**> Execution priority                         */
    U8          state;      /**> task state                                 */
    U8          priv;       /**> = 0 unprivileged, =1 privileged            */
} TCB;

#endif
/*
 *==========================================================================
 *                   GLOBAL VARIABLES DECLARATIONS
 *==========================================================================
 */
// Memory related globals are defined in k_mem.c

// kernel stack size
extern const U32 g_k_stack_size;
// task kernel stacks are statically allocated inside the OS image

extern U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));

#if 0
/* process stack for tasks in SYS mode, statically allocated inside the OS image  */
extern U32 g_p_stacks[MAX_TASKS][PROC_STACK_SIZE >> 2] __attribute__((aligned(8)));
#endif

extern unsigned int Image$$ZI_DATA$$ZI$$Limit; 	// Linker defined symbol
												// See ARM Compiler User Guide 5.x
#if 0

// task related globals are defined in k_task.c
extern TCB *gp_current_task;  		// always point to the current RUNNING task */

// TCBs are statically allocated inside the OS image
extern TCB g_tcbs[MAX_TASKS];
extern RTX_TASK_INFO g_null_task_info;
extern U32 g_num_active_tasks;		// number of non-dormant tasks */

#endif

#endif // ! K_INC_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
