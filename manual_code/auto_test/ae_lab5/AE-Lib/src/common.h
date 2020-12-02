/* @brief: common defines and structs for both kernel and user 
 * @file: common.h 
 * @author: Yiqing Huang
 * IMPORTANT: DO NOT MODIFY
   If you want to define your own macros and data structures, 
   add them to common_ext.h and do not let your
   newly-added code cause conflicts with the existing code in this file.
   For example, if you want to define new message types, 
   use a value starting from 10. Do not overwrite existing message type macro values.
 */

#ifndef _COMMON_H_
#define _COMMON_H_

/* ----- Types ----- */
typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long long       S64;
typedef unsigned long long U64;
typedef unsigned char   BIT;
typedef unsigned int    BOOL;
typedef unsigned int    size_t;
typedef signed int      ssize_t;
typedef unsigned char   task_t;

/* ----- Definitions ----- */
#define TRUE 1
#define FALSE 0
#define NULL 0
#define RTX_ERR -1
#define RTX_OK 0

/* Memory Algorithms */
#define FIXED_POOL 0
#define FIRST_FIT  1    /* only requies to implement this one */
#define BEST_FIT   2
#define WORST_FIT  3

/* Main Scheduling Algorithms */
#define DEFAULT 0          /* stric-priority, FCFS scheduling */
#define RM_PS   10         /* rate-monotonic scheduling with polling server */
#define RM_NPS  11         /* rate-Monotonic scheduling without polling server */
#define EDF     12         /* earliest-deadline-first scheduling */

#define PID_NULL 0         /* pre-defined Task ID for null task */
#define TID_KCD  15        /* pre-defined Task ID for KCD task */
#define TID_DISPLAY 14     /* pre-defined Task ID for DISPLAY task */
#define TID_UART_IRQ 0xFF  /* reserved TID for UART IRQ handler which is not a task */
#define TID_TIMER_IRQ 0xFE /* reserved TID for Timer IRQ handler, which is not a task */
#define MAX_TASKS 16       /* maximum number of tasks in the system */
#define KERN_STACK_SIZE 0x200 /* task kernel stack size in bytes */

/* Real-time Task Priority. Highest in the system*/
#define PRIO_RT 0    /* priority level for real-time tasks */ 
#define HIGH    100
#define MEDIUM  101
#define LOW     102
#define LOWEST  103
#define PRIO_NULL 255    /* hidden priority for null task */

/* task state macro */
#define DORMANT        0  /* terminated task state */
#define READY          1  /* A ready torun task that has been executed */
#define RUNNING        2  /* Executing */
#define BLK_MEM        3  /* blocked on requesting memory, not used in labs 1-5 */
#define BLK_MSG        4  /* blocked on receiving a message */
#define SUSPENDED      5  /* Suspended task */
#define NEW            15 /* A ready to run task that has never been executed */

/* message passing macros */
/* message types */
#define DEFAULT 0  /* a general purpose message */
#define KCD_REG 1  /* a command registration message */
#define KCD_CMD 2  /* a message that contains a command */
#define DISPLAY 3  /* a message that contains chars to be displayed to the RTX console */
#define KEY_IN  4  /* keyboard input from console */


/* message data sizes */
#define MIN_MSG_SIZE 1  /* minimum message data size in bytes */

/* mailbox sizes */
#define MIN_MBX_SIZE 1  /* minimum mailbox size in bytes */

/* time macros */
#define MIN_RTX_QTM 100 /* min time granularity of RTX in microseconds */

/* Structures */
/* Timing structure */
struct timeval_rt {
    U32 sec;            /* seconds */
    U32 usec;           /* microoseconds */
};

/* RTX system struct */

/* scheduling server struct */
typedef struct polling_server {
    struct timeval_rt p_n; /* period of the server */
    struct timeval_rt b_n; /* budget of the server */
} POLLING_SERVER;

typedef struct rtx_sys_info {
    size_t mem_blk_size;   /* min memory block size manageable  */
    int mem_algo;          /* memory allocator algorithm */
    U32 rtx_time_qtm;      /* time granularity in microseconds */
    POLLING_SERVER server;   /* scheduling server for non-real-time tasks */
    U8  sched;             /* scheduler    */
} RTX_SYS_INFO;

/* message buffer header struct */
typedef struct rtx_msg_hdr {
    U32 length;   /* length of the mssage including the message header size */
    U32 type;     /* type of the message */
} RTX_MSG_HDR;

/* common data structures in both kernel and user spaces */
/* Task information structure */
typedef struct rtx_task_info {
    void   (*ptask)();   /* Task entry address                      */ 
    U32    k_sp;         /* The task current kernel stack pointer   */
    U32    k_stack_hi;   /* The kernel stack starting addr. (high addr.)*/
    U32    u_sp;         /* The task current user stack pointer     */
    U32    u_stack_hi;   /* The user stack starting addr. (high addr.) */
    U16    k_stack_size; /* The task kernel total stack space in bytes */
    U16    u_stack_size; /* The task user total stack space in bytes*/
    task_t tid;          /* Task ID                                 */
    U8     prio;         /* Execution priority                      */
    U8     state;        /* Task state                              */
    U8     priv;         /* = 0 unprivileged, =1 priviliged         */  
    struct timeval_rt tv_cpu;  /* task execution cpu time */
    struct timeval_rt tv_wall; /* task execution wall clock time */
    
    /* The following only applies to real-time tasks */
    struct timeval_rt p_n;    /* period in seconds and microseconds */
    RTX_MSG_HDR *msg_hdr;     /* real-time task message header      */
    U32    num_msgs;          /* real-time task mailbox capacity    */
} RTX_TASK_INFO;

/* Real-time task information structure */
typedef struct task_rt {
    struct timeval_rt p_n;  /* periodic in seconds and microseconds    */
    void (*task_entry)();   /* Task entry address                      */ 
    U16  u_stack_size;      /* The task user total stack space in bytes*/
    U8   priv;              /* = 0 unprivileged, =1 priviliged         */  
} TASK_RT;

#endif // _COMMON_H_
