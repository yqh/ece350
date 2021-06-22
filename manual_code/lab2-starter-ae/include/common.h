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
 * @file        common.h
 * @brief       Common macros and structures for both kernel and user
 *
 * @version     V1.2021.04
 * @authors     Yiqing Huang
 * @date        2021 APR
 * @see         rtx.h 
 * @see         common_ext.h  
 * @attention   DO NOT MODIFY
 *
 * @note        If you want to define your own macros and data structures,
 *              add them to common_ext.h.
 *              Do not let your newly-added code cause conflicts with
 *              the existing code in this file.
 *              For example, if you want to define new message types,
 *              use a value starting from 10.
 *              Do not overwrite existing message type macro values.
 *
 *****************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_

#include "common_ext.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                (!FALSE)
#endif

#ifndef NULL
#define NULL                0
#endif

#define RTX_ERR            -1
#define RTX_OK              0

/* Memory Allocator Algorithms */
#define FIXED_POOL          0       /* fixed size memory pool    */
#define FIRST_FIT           1       /* linear first fit search   */
#define BEST_FIT            2       /* linear best fit search    */
#define WORST_FIT           3       /* linear worst fit search   */
#define NEXT_FIT            4       /* linear next fit search    */
#define BUDDY               5       /* binary buddy system       */ 

#define MIN_BLK_SIZE        32      /* minimum memory block size in bytes */
#define MIN_BLK_SIZE_LOG2   5       /* log2(MIN_BLK_SIZE) */
#define MAX_MPOOLS          2       /* maximum number of memory pools */
#define MPID_IRAM1          0       /* IRAM1 memory pool ID */
#define MPID_IRAM2          1       /* IRAM2 memory pool ID */

/* Main Scheduling Algorithms */
#define DEFAULT             0       /* preemptive priority scheduler, FCFS within each priority */
#define RM_PS               10      /* rate-monotonic scheduling with polling server */
#define RM_NPS              11      /* rate-Monotonic scheduling without polling server */
#define EDF                 12      /* earliest-deadline-first scheduling */


#define MAX_TASKS           0x10    /* maximum number of tasks in the system */
#define KERN_STACK_SIZE     0x300   /* task kernel stack size in bytes */
#define PROC_STACK_SIZE     0x200   /* minimum task user stack size in bytes */
#define TID_NULL            0x0     /* reserved Task ID for the null task */
#define TID_KCD             (MAX_TASKS -1)     
                                    /* reserved Task ID for KCD task */
#define TID_UART_IRQ        0xFD    /* reserved TID for UART IRQ handler which is not a task */
#define TID_KERN            0xFE    /* reserved TID for kernel ownership */
#define TID_UNK             0xFF    /* reserved TID for unknown ownership */

/* Real-time Task Priority. Highest in the system*/
#define PRIO_RT_LB          0       /* real-time task priority level lower bound */
#define PRIO_RT_UB          MAX_TASKS     
                                    /* real-time task priority level upper bound */
#define HIGH                0x80
#define MEDIUM              0x81
#define LOW                 0x82
#define LOWEST              0x83
#define PRIO_NULL           0xff    /* hidden priority for the null task */

/* Task States */
#define DORMANT             0       /* terminated task state */
#define READY               1       /* A ready to run task   */
#define RUNNING             2       /* Executing */
#define BLK_MEM             3       /* blocked on memory */
#define BLK_MBX             4       /* blocked on mailbox availablility */
#define SUSPENDED           5       /* Suspended task */

/* Message Passing Macros */
/* Message Types */
#define DEFAULT             0       /* a general purpose message */
#define KCD_REG             1       /* a command registration message */
#define KCD_CMD             2       /* a message that contains a command */
#define DISPLAY             3       /* a message that contains chars to be displayed to the RTX console */
#define KEY_IN              4       /* keyboard input from console */

/* Message Data Sizes */
#define MIN_MSG_SIZE        1       /* minimum message size in bytes */

/* Mailbox Sizes */
#define MIN_MBX_SIZE        1       /* minimum mailbox size in bytes */
#define KCD_MBX_SIZE        0x200   /* kcd task's mailbox size in bytes */

/* Time Macros */
#define MIN_RTX_QTM         100     /* minimum time granularity of RTX in microseconds */

/* TRAP NUMBERS */
#define SVC_RTX_INIT        0x00
#define SVC_MEM_ALLOC       0x01
#define SVC_MEM_DEALLOC     0x02
#define SVC_MEM_DUMP        0x03
#define SVC_TSK_CREATE      0x04
#define SVC_TSK_EXIT        0x05
#define SVC_TSK_YIELD       0x06
#define SVC_TSK_SET_PRIO    0x07
#define SVC_TSK_GET         0x08
#define SVC_TSK_GETTID      0x09

/*
 *===========================================================================
 *                             TYPEDEFS
 *===========================================================================
 */

typedef signed char         S8;
typedef unsigned char       U8;
typedef short               S16;
typedef unsigned short      U16;
typedef int                 S32;
typedef unsigned int        U32;
typedef unsigned char       BOOL;
typedef unsigned int        size_t;
typedef signed int          ssize_t;
typedef unsigned char       task_t;
typedef signed char         mpool_t;


/*
 *===========================================================================
 *                             STRUCTURES
 *===========================================================================
 */

/**
 * @brief RTX system configuration structure
 */
typedef struct rtx_sys_info 
{
    int         mem_algo;           /**< memory allocator algorithm */
    int         sched;              /**< scheduling algorithm       */
} RTX_SYS_INFO;

typedef struct task_init 
{  
    void        (*ptask)();         /**< task entry address                 */
    U32         u_stack_size;       /**< user stack size in bytes           */
    task_t      tid;                /**< task id, output param, deprecated  */
    U8          prio;               /**< execution priority                 */
    U8          priv;               /**< = 0 unprivileged, =1 privileged    */    
} TASK_INIT;

/**
 * @brief Task information structure
 * @note  The highest location used by the stack is the first word below the stack base
 *        The stack pointer is the address the last value written to the stack (pushed)
 */
typedef struct rtx_task_info 
{    
    void        (*ptask)();         /**< task entry address                 */
    U32         k_stack_size;       /**< kernel stack size in bytes         */
    U32         k_sp;               /**< top of kernel stack                */
    U32         k_sp_base;          /**< kernel stack base (high addr.)     */
    U32         u_stack_size;       /**< user stack size in bytes           */
    U32         u_sp;               /**< top of user stack                  */
    U32         u_sp_base;          /**< user stack base addr. (high addr.) */
    task_t      tid;                /**< task id, output param              */
    U8          prio;               /**< execution priority                 */
    U8          priv;               /**< = 0 unprivileged, =1 privileged    */   
    U8          state;              /**< task state                         */
} RTX_TASK_INFO;

#endif // ! COMMON_H_
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
