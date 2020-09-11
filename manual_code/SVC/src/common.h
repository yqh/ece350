/* @brief: common defines and structs for both kernel and user 
 * @file: common.h 
 * @author: Yiqing Huang
 * @date: 2020/09/11
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
#define NUM_PROCS 6     /* maximum number tasks the OS supports */
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

/* Pre-defined Task ID */
#define PID_NULL 0

/* Process Priority. The bigger the number is, the lower the priority is*/
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3

/* task state macro */
#define DORMANT        0    
#define READY          1
#define RUNNING        2
#define BLK_MEM        3 
#define BLK_SEM        4
#define BLK_ISR        5

/* Structures */

/* common data structures in both kernel and user spaces */
/* Task information structure */
typedef struct rtx_task_info {
  void   (*ptask)();          /* Task entry address                      */ 
  U32    stack_pointer;       /* The current stack pointer value         */
  U32    stack_hi;            /* stack starting addr. (high addr.)       */
  U16    stack_size;          /* The stack size in bytes                 */
  U8     state;               /* Task state                              */
  U8     prio;                /* Execution priority                      */
  U8     task_id;             /* Task ID                                 */
  /* note 3 padding bytes will be added by the compiler to this structure*/
} RTX_TASK_INFO;

#endif // _COMMON_H_
