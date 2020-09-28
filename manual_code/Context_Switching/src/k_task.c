/**
 * @file:   k_task.c  
 * @brief:  task management C file
 * @author: Yiqing Huang
 * @date:   2020/09/24
 * NOTE: The example code shows one way of implementing context switching.
 *       The code only has minimal sanity check. There is no stack overflow check.
 *       The implementation assumes only two simple user task and NO HARDWARE INTERRUPTS. 
 *       The purpose is to show how context switch could be done under stated assumptions. 
 *       These assumptions are not true in the required RTX Project!!!
 *       You need to understand the assumptions and the limitations of the code. 
 */

#include <LPC17xx.h>
#include "uart_polling.h"
#include "k_task.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* ----- Global Variables ----- */
TCB *gp_current_task = NULL;    /* always point to the current RUN process */

/* TCBs and Kernel stacks are statically allocated and is inside the OS image */
TCB g_tcbs[MAX_TASKS];
U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2];

/*---------------------------------------------------------------------------
The meory map of the OS image may look like the following:

                    0x10008000+---------------------------+ High Address
                              |                           |
                              |                           |
                              |    Free memory space      |
                              | (Alloc user stacks here)  |
                              |                           |
                              |                           |
                              |                           |
&Image$$RW_IRAM1$$ZI$$Limit-->|---------------------------|-----+-----
                              |         ......            |     ^
                              |---------------------------|     |
                              |                           |     | 
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |     |                
             g_k_stacks[15]-->|---------------------------|     |
                              |                           |     |
                              |     other kernel stacks   |     |                              
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |  OS Image
              g_k_stacks[2]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |                      
              g_k_stacks[1]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |
              g_k_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |---------------------------|     |
                              |        TCBs               |     |
                      g_tcbs->|---------------------------|     |
                              |        global vars        |     |
                              |---------------------------|     |
                              |                           |     |          
                              |                           |     |
                              |                           |     |
                              |                           |     V
                    0x10000000+---------------------------+ Low Address
    
---------------------------------------------------------------------------*/ 
/**
 * @biref: initialize all tasks in the system
 * @param: RTX_TASK_INFO *task_info, an array of initial tasks
 * @param: num_tasks, number of elements in the task_info array
 * @return: none
 * PRE: memory has been properly initialized
 */
int k_tsk_init(RTX_TASK_INFO *task_info, int num_tasks) 
{
    int i;
    U32 *sp;
    RTX_TASK_INFO *p_taskinfo = task_info;
  
    /* initilize exception stack frame (i.e. initial context) for each task */
    for ( i = 0; i < num_tasks; i++ ) {
        int j;
        TCB *p_tcb = &g_tcbs[i+1];
        p_tcb ->tid = i+1;
        p_tcb->state = NEW;
        sp = g_k_stacks[i+1] + (KERN_STACK_SIZE >> 2) ; /* stacks grows down, so get the high addr. */
        *(--sp)  = INITIAL_xPSR;    /* task initial xPSR  */
        *(--sp)  = (U32)(p_taskinfo->ptask); /* PC contains the entry point of the task */
        for ( j = 0; j < 6; j++ ) { /*R0-R3, R12, LR */
            *(--sp) = 0x0;
        }
        p_tcb->msp = sp;
        if ( p_taskinfo->priv == 0 ) { /* unpriviledged task */ 
            /* allocate user stack, not implemented */
        }
        p_taskinfo++;
    }
    return RTX_OK;
}

/*@brief: scheduler, pick the tid of the next to run task
 *@return: TCB pointer of the next to run task
 *         NULL if error happens
 *POST: if gp_current_task was NULL, then it gets set to tcb[1].
 *      No other effect on other global variables.
 */

TCB *dummy_scheduler(void)
{
    if (gp_current_task == NULL) {
        gp_current_task = &g_tcbs[1]; 
        return &g_tcbs[1];
    }

    if ( gp_current_task == &g_tcbs[1] ) {
        return &g_tcbs[2];
    } else if ( gp_current_task == &g_tcbs[2] ) {
        return &g_tcbs[1];
    } else {
        return NULL;
    }
}

/*@brief: switch out old tcb (p_tcb_old), run the new tcb (gp_current_task)
 *@param: p_tcb_old, the old tcb that was in RUN
 *@return: RTX_OK upon success
 *         RTX_ERR upon failure
 *PRE:  p_tcb_old and gp_current_task are pointing to valid TCBs.
 */
int task_switch(TCB *p_tcb_old) 
{
    U8 state;
    
    state = gp_current_task->state;

    if (state == NEW) {
        if (gp_current_task != p_tcb_old && p_tcb_old->state != NEW) {
            p_tcb_old->state = READY;
            p_tcb_old->msp = (U32 *) __get_MSP();
        }
        gp_current_task->state = RUNNING;
        __set_MSP((U32) gp_current_task->msp);
        __rte();  /* pop exception stack frame from the stack for a new task */
    } 
    
    /* The following will only execute if the if block above is FALSE */

    if (gp_current_task != p_tcb_old) {
        if (state == READY){         
            p_tcb_old->state = READY; 
            p_tcb_old->msp = (U32 *) __get_MSP(); // save the old process's sp
            gp_current_task->state = RUNNING;
            __set_MSP((U32) gp_current_task->msp); //switch to the new proc's stack    
        } else {
            gp_current_task = p_tcb_old; // revert back to the old proc on error
            return RTX_ERR;
        } 
    }
    return RTX_OK;
}
/**
 * @brief yield the processor. The caller becomes READY and the scheduler picsk the next ready to run task.
 * @return RTX_ERR on error and zero on success
 * POST: gp_current_task gets updated to next to run process
 */
int k_tsk_yield(void)
{
    TCB *p_tcb_old = NULL;
    
    p_tcb_old = gp_current_task;
    gp_current_task = dummy_scheduler();
    
    if ( gp_current_task == NULL  ) {
        gp_current_task = p_tcb_old; // revert back to the old task
        return RTX_ERR;
    }
    if ( p_tcb_old == NULL ) {
        p_tcb_old = gp_current_task;
    }
    task_switch(p_tcb_old);
    return RTX_OK;
}

int k_tsk_create(task_t *task, void (*task_entry)(void), U8 prio, U16 stack_size)
{
#ifdef DEBUG_0
    printf("k_tsk_create: entering...\n\r");
    printf("task = 0x%x, task_entry = 0x%x, prio=%d, stack_size = %d\n\r", task, task_entry, prio, stack_size);
#endif /* DEBUG_0 */
    return RTX_OK;

}

void k_tsk_exit(void) 
{
#ifdef DEBUG_0
    printf("k_tsk_exit: entering...\n\r");
#endif /* DEBUG_0 */
    return;
}

int k_tsk_set_prio(task_t task_id, U8 prio) 
{
#ifdef DEBUG_0
    printf("k_tsk_set_prio: entering...\n\r");
    printf("task_id = %d, prio = %d.\n\r", task_id, prio);
#endif /* DEBUG_0 */
    return RTX_OK;    
}

int k_tsk_get(task_t task_id, RTX_TASK_INFO *buffer)
{
#ifdef DEBUG_0
    printf("k_tsk_get: entering...\n\r");
    printf("task_id = %d, buffer = 0x%x.\n\r", task_id, buffer);
#endif /* DEBUG_0 */    
    if (buffer == NULL) {
        return RTX_ERR;
    }
    /* The code fills the buffer with some fake task information. 
       You should fill the buffer with correct information    */
    buffer->tid = task_id;
    buffer->prio = 99;
    buffer->state = MEDIUM;
    buffer->priv = 0;
    buffer->ptask = 0x0;
    buffer->k_sp = 0xBEEFDEAD;
    buffer->k_stack_size = KERN_STACK_SIZE;
    buffer->u_sp = 0xDEADBEEF;
    buffer->u_stack_size = 0x200;

    return RTX_OK;     
}
