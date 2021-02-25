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
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 * @see         rtx.h
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


/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */

#define TRUE                1
#define FALSE               0
#define NULL                0
#define RTX_ERR            -1
#define RTX_OK              0

/* Memory Algorithms */
#define FIXED_POOL          0
#define FIRST_FIT           1       /* only requires to implement this one */
#define BEST_FIT            2
#define WORST_FIT           3

/* Main Scheduling Algorithms */
#define DEFAULT             0       /* strict-priority, FCFS scheduling */
#define RM_PS               10      /* rate-monotonic scheduling with polling server */
#define RM_NPS              11      /* rate-Monotonic scheduling without polling server */
#define EDF                 12      /* earliest-deadline-first scheduling */

#define TID_NULL            0       /* pre-defined Task ID for null task */
#define TID_KCD             15      /* pre-defined Task ID for KCD task */
#define TID_DISPLAY         14      /* pre-defined Task ID for DISPLAY task */
#define TID_UART_IRQ        0xFF    /* reserved TID for UART IRQ handler which is not a task */
#define TID_TIMER_IRQ       0xFE    /* reserved TID for Timer IRQ handler, which is not a task */
#define MAX_TASKS           16      /* maximum number of tasks in the system */
#define KERN_STACK_SIZE     0x200   /* task kernel stack size in bytes */
#define PROC_STACK_SIZE     0x200   /* task proc space stack size in bytes */

/* Real-time Task Priority. Highest in the system*/
#define PRIO_RT             0       /* priority level for real-time tasks */
#define HIGH                100
#define MEDIUM              101
#define LOW                 102
#define LOWEST              103
#define PRIO_NULL           255     /* hidden priority for null task */

/* Task States */
#define DORMANT             0       /* terminated task state */
#define READY               1       /* A ready to run task that has been executed */
#define RUNNING             2       /* Executing */
#define BLK_MEM             3       /* blocked on requesting memory, not used in labs 1-5 */
#define BLK_MSG             4       /* blocked on receiving a message */
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
#define KCD_MBX_SIZE        0x200   /* kcd task's stack size in bytes */

/* Time Macros */
#define MIN_RTX_QTM         100     /* minimum time granularity of RTX in microseconds */


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
typedef long long           S64;
typedef unsigned long long  U64;
typedef unsigned char       BIT;
typedef unsigned int        BOOL;
typedef unsigned int        size_t;
typedef signed int          ssize_t;
typedef unsigned char       task_t;


/*
 *===========================================================================
 *                             STRUCTURES
 *===========================================================================
 */

/**
 * @brief Timing structure
 */
struct timeval_rt {
    U32                 sec;                /**> seconds                           */
    U32                 usec;               /**> microoseconds                     */
};

/**
 * @brief scheduling server structure
 */
typedef struct polling_server {
    struct timeval_rt   p_n;                /**< period of the server              */
    struct timeval_rt   b_n;                /**< budget of the server              */
} POLLING_SERVER;

/**
 * @brief RTX system configuration structure
 */
typedef struct rtx_sys_info {
    U32                 rtx_time_qtm;       /**< time granularity in microseconds  */
    POLLING_SERVER      server;             /**< scheduling server for non-real-time tasks */
    U8                  sched;              /**< scheduler                         */
} RTX_SYS_INFO;

/**
 * @brief message buffer header struct
 */
typedef struct rtx_msg_hdr {
    U32                 length;             /**> length of the mssage including the message header size */
    U32                 type;               /**> type of the message */
} RTX_MSG_HDR;

/**
 * @brief Task information structure
 */
typedef struct rtx_task_info {
    void                (*ptask)();         /**> task entry address                 */
    U32                 k_sp;               /**> current kernel stack pointer       */
    U32                 k_stack_hi;         /**> kernel stack base (high addr.)     */
    U32                 u_sp;               /**> current user stack pointer         */
    U32                 u_stack_hi;         /**> user stack base addr. (high addr.) */
    U16                 k_stack_size;       /**> kernel stack size in bytes         */
    U16                 u_stack_size;       /**> user stack size in bytes           */
    task_t              tid;                /**> task ID                            */
    U8                  prio;               /**> execution priority                 */
    U8                  state;              /**> task state                         */
    U8                  priv;               /**> = 0 unprivileged, =1 privileged    */
    struct timeval_rt   tv_cpu;             /**> task execution cpu time            */
    struct timeval_rt   tv_wall;            /**> task execution wall clock time     */
    
    /* The following only applies to real-time tasks */
    struct timeval_rt   p_n;                /**> period in seconds and microseconds */
    RTX_MSG_HDR        *msg_hdr;            /**> real-time task message header      */
    U32                 num_msgs;           /**> real-time task mailbox capacity    */
} RTX_TASK_INFO;

/**
 * @brief Real-time task information structure
 */
typedef struct task_rt {
    struct timeval_rt   p_n;                /**> period in seconds and microseconds */
    void                (*task_entry)();    /**> task entry address                 */
    U16                 u_stack_size;       /**> user stack size in bytes           */
    U8                  priv;               /**> = 0 unprivileged, =1 privileged    */
} TASK_RT;

#endif // ! COMMON_H_
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
