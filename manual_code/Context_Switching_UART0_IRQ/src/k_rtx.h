/** 
 * @file:   k_rtx.h
 * @brief:  kernel deinitiation and data structure header file
 * @auther: Yiqing Huang
 * @date:   2020/09/21
 */
 
#ifndef K_RTX_H_
#define K_RTX_H_

#include "common.h"

/*----- Definitations -----*/


/*----- Types -----*/


/*
  TCB data structure definition to support two kernel tasks.
  You will need to add more fields to this structure.
*/

typedef struct tcb
{
    /*struct tcb *next;*/ /* next tcb, not used in this example */  
    U32 *msp;    /* msp of the task */
    U8  tid;     /* task id */
    U8  prio;    /* Execution priority */
    U8  state;   /* task state */  
    U8  priv;    /* = 0 unprivileged, =1 priviliged */
} TCB;

#endif // ! K_RTX_H_
