/* @brief: common defines and structs for both kernel and user 
 * @file: common.h 
 * @author: Yiqing Huang
 * @date: 2020/09/20
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
typedef unsigned char   task_t;

/* ----- Definitions ----- */
#define NUM_TASKS 2     /* maximum number tasks the OS supports, not includng the null task */
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

#define PID_NULL 0      /* pre-defined Task ID for null task */
#define MAX_TASKS 16    /* maximum number of tasks in the system */
#define KERN_STACK_SIZE 0x200 /* task kernel stack size in bytes */

/* Task Priority. The bigger the number is, the lower the priority is*/
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3
#define PRIO_NULL 4    /* hidden priority for null task */

/* task state macro */
#define DORMANT        0  /* terminated task state */
#define READY          1  
#define RUNNING        2
#define BLK_MEM        3 
#define BLK_SEM        4
#define BLK_ISR        5
#define NEW            15

/* Structures */

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
} RTX_TASK_INFO;

#endif // _COMMON_H_
