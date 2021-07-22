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
#define TID_WCLCK           (MAX_TASKS - 3)
                                    /* reserved Task ID for the wall clock display real-time task */
#define TID_CON             (MAX_TASKS - 2)  
                                    /* reserved Task ID for console display task */
#define TID_KCD             (MAX_TASKS - 1)
                                    /* reserved Task ID for KCD task */
#define TID_TIMER           0xFD    /* reserved TID for TIMER IRQ handler which is not a task */
#define TID_UART            0xFE    /* reserved TID for UART IRQ handler which is not a task */
#define TID_UNK             0xFF    /* reserved TID for unknown tasks */

/* Real-time Task Priority. Lower the number is, higher the priority is. */
#define PRIO_RT_LB          0       /* real-time task priority level lower bound */
#define PRIO_RT_UB          (MAX_TASKS -1)    
#define PRIO_RT             PRIO_RT_LB
                                    /* real-time task priority level */
                                    /* real-time task priority level upper bound */
/* Non-Real-time Task Priorities. */
#define HIGH                0x80
#define MEDIUM              0x81
#define LOW                 0x82
#define LOWEST              0x83
#define PRIO_NULL           0xFF    /* hidden priority for the null task */

/* Task States */
#define DORMANT             0       /* terminated task state */
#define READY               1       /* A ready to run task   */
#define RUNNING             2       /* Executing */
#define BLK_SEND            3       /* blocked on a full mailbox on send */
#define BLK_RECV            4       /* blocked on an empty emailbox on receive */
#define SUSPENDED           5       /* Suspended task */

/* Message Passing Macros */
/* Message Types */
#define DEFAULT             0       /* a general purpose message */
#define KCD_REG             1       /* a command registration message */
#define KCD_CMD             2       /* a message that contains a command */
#define DISPLAY             3       /* a message that contains chars to be displayed to the RTX console */
#define KEY_IN              4       /* keyboard input from console */

/* Mailbox Sizes */
#define MSG_HDR_SIZE        sizeof(RTX_MSG_HDR)      
                                    /* rtx_msg_hdr struct size */
#define MIN_MSG_SIZE        MSG_HDR_SIZE       
                                    /* minimum message size in bytes */
#define KCD_MBX_SIZE        0x200   /* KCD mailbox size */
#define CON_MBX_SIZE        0x80    /* consolde display mailbox size */
#define UART_MBX_SIZE       0x80    /* UART interrupt handler mailbox size */

/* System Task Macros */
#define KCD_CMD_BUF_SIZE    0x40    /* max command length bufferred by KCD */

/* Time Macros */
#define RTX_TICK_SIZE       500     /* Time granularity of RTX Clock in microseconds */
#define MIN_PERIOD          5       /* minimum period of a task is RTX_TICK_SIZE * MIN_PERIOD */

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
#define SVC_TSK_LS          0x0A
#define SVC_MBX_CREATE      0x0B
#define SVC_MBX_SEND        0x0C
#define SVC_MBX_SEND_NB     0x0D
#define SVC_MBX_RECV        0x0E
#define SVC_MBX_RECV_NB     0x0F
#define SVC_MBX_LS          0x10
#define SVC_MBX_GET         0x11
#define SVC_RT_TSK_SET      0x12
#define SVC_RT_TSK_SUSP     0x13
#define SVC_RT_TSK_GET      0x14

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
typedef unsigned char       task_t;     // task ID type
typedef signed char         mpool_t;    // memory pool descriptor type
typedef signed char         mbx_t;      // mailbox descriptor type


/*
 *===========================================================================
 *                             STRUCTURES
 *===========================================================================
 */
 

/* Timing structure */
typedef struct timeval {
    U32 sec;            /* seconds */
    U32 usec;           /* microoseconds */
} TIMEVAL; 

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

/* message header struct */
typedef __packed struct rtx_msg_hdr {
    U32         length;             /**< length of the mssage buffer including the message header size */
    task_t      sender_tid;         /**< sending task tid */
    U8          type;               /**< type of the message */      
} RTX_MSG_HDR;

#if 0
/* Real-time task information structure */
typedef struct task_rt {
    TIMEVAL     p_n;                /**< period = relative deadline    */
    size_t      rt_mbx_size;        /**< mailbox capacity in bytes     */
} TASK_RT;
#endif


#endif // ! COMMON_H_
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
