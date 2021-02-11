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
 * @file        k_task.c
 * @brief       task management C file
 *              l2
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @attention   assumes NO HARDWARE INTERRUPTS
 * @details     The starter code shows one way of implementing context switching.
 *              The code only has minimal sanity check.
 *              There is no stack overflow check.
 *              The implementation assumes only two simple privileged task and
 *              NO HARDWARE INTERRUPTS.
 *              The purpose is to show how context switch could be done
 *              under stated assumptions.
 *              These assumptions are not true in the required RTX Project!!!
 *              Understand the assumptions and the limitations of the code before
 *              using the code piece in your own project!!!
 *
 *****************************************************************************/

//#include "VE_A9_MP.h"
#include "Serial.h"
#include "k_task.h"
#include "k_rtx.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/*
 *==========================================================================
 *                            GLOBAL VARIABLES
 *==========================================================================
 */

TCB             *gp_current_task = NULL;	// the current RUNNING task
TCB             g_tcbs[MAX_TASKS];			// an array of TCBs
RTX_TASK_INFO   g_null_task_info;			// The null task info
U32             g_num_active_tasks = 0;		// number of non-dormant tasks

/*---------------------------------------------------------------------------
The memory map of the OS image may look like the following:

                       RAM_END+---------------------------+ High Address
                              |                           |
                              |                           |
                              |    Free memory space      |
                              |   (user space stacks      |
                              |         + heap            |
                              |                           |
                              |                           |
                              |                           |
 &Image$$ZI_DATA$$ZI$$Limit-->|---------------------------|-----+-----
                              |         ......            |     ^
                              |---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
             g_p_stacks[15]-->|---------------------------|     |
                              |                           |     |
                              |  other kernel proc stacks |     |
                              |---------------------------|     |
                              |      PROC_STACK_SIZE      |  OS Image
              g_p_stacks[2]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[1]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |                           |  OS Image
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
                              |        TCBs               |  OS Image
                      g_tcbs->|---------------------------|     |
                              |        global vars        |     |
                              |---------------------------|     |
                              |                           |     |          
                              |                           |     |
                              |                           |     |
                              |                           |     V
                     RAM_START+---------------------------+ Low Address
    
---------------------------------------------------------------------------*/ 

/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

struct Queue {
  TCB *front;
  TCB *rear;
};

struct Queue *ready_queue; 

void queue_add(TCB* t_block) {
  // start at the head of the ready queue; 
  TCB* t_comp = ready_queue-> front;

  // first can compare with the head of the queue
  if (t_block-> prio <  ready_queue-> front-> prio) {
    // new created task has a higher prio than the current task 
    t_block-> next = t_comp;
    // the front of the ready queue should now point at the head 
    ready_queue-> front = t_block; 
  } else {
    // the first task (current task) has higher prio, so now compare with the inner tasks 
    TCB* prev = ready_queue->front; 
    while(t_comp != NULL) {
      // move to the next task in the queue 
      t_comp = -> t_comp-> next; 
      // compare the priorities of the task to be added and the current task in the queue
      // 0 Prio == HIGHEST 
      if(t_block-> prio < t_comp-> prio) {
        // if the prio is the less, then we have found where to insert the task in the ready queue
        // the previous task that has a higher priority will now have next point to the new added task
        // new added task next will now point to t_comp since t_comp has a lower priority 
        prev-> next = t_block; 
        t_block-> next = t_comp;
        break; 
      }
      prev = prev-> next; 
    }
    if (t_comp == NULL) {
      // technically should never get here since the last task in the queue should always be the PRIO NULL TASK 
      // we cannot add a task with prio >= 255
      printf("OOPS, WE SHOULDN'T BE HERE")
    }
  }
}

/**************************************************************************//**
 * @brief   scheduler, pick the TCB of the next to run task
 *
 * @return  TCB pointer of the next to run task
 * @post    gp_curret_task is updated
 *
 *****************************************************************************/
TCB *scheduler(void)
{
  // returns highest-priority task among all runable tasks (ready plus currently-running)
  // runable tasks are scheduled based ona simple strict-priority scheduling algorithm
  // ready queue should for processor should maintain a sorted list of ready tasks based on their priorities
  // same priority - first come first serve
  // when the scheduler is called, that means the queue has been modified 
  // have to iterate through the queue to make sure the order is correct 
  // note that this scheduler will always be called after only 1 ITEM IN THE QUEUE HAS BEEN MODIFIED
  // either 1 task is removed, 1 task prio is changed
  TCB *t_comp = ready_queue-> front;
  TCB *prev = ready_queue-> front;
  if (t_comp-> state == DORMANT) {
    // the first task has exited, so all tasks are going to be bumped up
    ready_queue-> front = t_comp-> next; 
    // return the new highest priority task in the queue 
    return ready_queue-> front; 
  }
  while (prev != NULL) {
    // check if any have been modified
    // set compare pointer to next task
    t_comp = t_comp-> next; 
    if (prev->prio > t_comp->prio) {      
      // the task in front has a lower priority, so I can essentially remove the node and reinsert it into its proper order
      prev-> next = t_comp-> next; 
      queue_add(t_comp); 
      // now that the task is added correctly, I can return the first item in the queue 
      return ready_queue-> front; 
    }
    // set prev pointer to next task
    prev = prev-> next; 
  }  
  // if we reach this, that means that the order of the tasks has not changed
  // can be because we change the priority of the task, but the queue maintains proper order
  // simply return the front task of the queue 
  return ready_queue-> front; 
}

/**************************************************************************//**
 * @brief       initialize all boot-time tasks in the system,
 *
 *
 * @return      RTX_OK on success; RTX_ERR on failure
 * @param       task_info   boot-time task information structure pointer
 * @param       num_tasks   boot-time number of tasks
 * @pre         memory has been properly initialized
 * @post        none
 *
 * @see         k_tsk_create_new
 *****************************************************************************/

int k_tsk_init(RTX_TASK_INFO *task_info, int num_tasks)
{
    extern U32 SVC_RESTORE;

    RTX_TASK_INFO *p_taskinfo = &g_null_task_info;
    g_num_active_tasks = 0;

    if (num_tasks > MAX_TASKS) {
    	return RTX_ERR;
    }

    // create the first task
    TCB *p_tcb = &g_tcbs[0];
    p_tcb->prio     = PRIO_NULL;
    p_tcb->priv     = 1;
    p_tcb->tid      = TID_NULL;
    p_tcb->state    = RUNNING;
    g_num_active_tasks++;
    gp_current_task = p_tcb;
    // update the schedule ready queue by adding the new task in the appropriate order
    // this is the first task, so obviously this will be the highest priority task 
    // only task in the ready queue, so there will be no tasks after it
    p_tcb-> next = NULL; 
    ready_queue-> front = ready_queue-> rear = p_tcb; 

    // create the rest of the tasks
    p_taskinfo = task_info;
    for ( int i = 0; i < num_tasks; i++ ) {
        TCB *p_tcb = &g_tcbs[i+1];
        // set the priority of each tcb to be medium
        p_tcb-> prio = MEDIUM;
        p_tcb-> next = NULL;
        if (k_tsk_create_new(p_taskinfo, p_tcb, i+1) == RTX_OK) {
          // update the schedule ready queue by adding the new task in the appropriate order
          // the front of the queue is the highest prio task, which should be the current task
          // add the task to the end of the queue 
          queue_add(p_tcb);
          g_num_active_tasks++;
          // run scheduler and may have to premptive run new task
          k_tsk_run_new(); 
        }
        p_taskinfo++;
    }

    // set the rest of the tcbs to be dormant 
    for (int i = num_tasks; i < MAX_TASKS; i++) {
      g_tcbs[i].state = DORMANT;
    }

    k_mem_init();
    return RTX_OK;
}
/**************************************************************************//**
 * @brief       initialize a new task in the system,
 *              one dummy kernel stack frame, one dummy user stack frame
 *
 * @return      RTX_OK on success; RTX_ERR on failure
 * @param       p_taskinfo  task information structure pointer
 * @param       p_tcb       the tcb the task is assigned to
 * @param       tid         the tid the task is assigned to
 *
 * @details     From bottom of the stack,
 *              we have user initial context (xPSR, PC, SP_USR, uR0-uR12)
 *              then we stack up the kernel initial context (kLR, kR0-kR12)
 *              The PC is the entry point of the user task
 *              The kLR is set to SVC_RESTORE
 *              30 registers in total
 *
 *****************************************************************************/
int k_tsk_create_new(RTX_TASK_INFO *p_taskinfo, TCB *p_tcb, task_t tid)
{
    extern U32 SVC_RESTORE;
    extern U32 K_RESTORE;

    U32 *sp;

    if (p_taskinfo == NULL || p_tcb == NULL)
    {
        return RTX_ERR;
    }

    p_tcb ->tid = tid;
    p_tcb->state = READY;

    /*---------------------------------------------------------------
     *  Step1: allocate kernel stack for the task
     *         stacks grows down, stack base is at the high address
     * -------------------------------------------------------------*/

    ///////sp = g_k_stacks[tid] + (KERN_STACK_SIZE >> 2) ;
    sp = k_alloc_k_stack(tid);

    // 8B stack alignment adjustment
    if ((U32)sp & 0x04) {   // if sp not 8B aligned, then it must be 4B aligned
        sp--;               // adjust it to 8B aligned
    }

    /*-------------------------------------------------------------------
     *  Step2: create task's user/sys mode initial context on the kernel stack.
     *         fabricate the stack so that the stack looks like that
     *         task executed and entered kernel from the SVC handler
     *         hence had the user/sys mode context saved on the kernel stack.
     *         This fabrication allows the task to return
     *         to SVC_Handler before its execution.
     *
     *         16 registers listed in push order
     *         <xPSR, PC, uSP, uR12, uR11, ...., uR0>
     * -------------------------------------------------------------*/

    // if kernel task runs under SVC mode, then no need to create user context stack frame for SVC handler entering
    // since we never enter from SVC handler in this case
    // uSP: initial user stack
    if ( p_taskinfo->priv == 0 ) { // unprivileged task
        // xPSR: Initial Processor State
        *(--sp) = INIT_CPSR_USER;
        // PC contains the entry point of the user/privileged task
        *(--sp) = (U32) (p_taskinfo->ptask);

        //********************************************************************//
        //*** allocate user stack from the user space, not implemented yet ***//
        //********************************************************************//

        *(--sp) = (U32) p_tcb->u_sp;


        // uR12, uR11, ..., uR0
        for ( int j = 0; j < 13; j++ ) {
            *(--sp) = 0x0;
        }
    }


    /*---------------------------------------------------------------
     *  Step3: create task kernel initial context on kernel stack
     *
     *         14 registers listed in push order
     *         <kLR, kR0-kR12>
     * -------------------------------------------------------------*/
    if ( p_taskinfo->priv == 0 ) {
        // user thread LR: return to the SVC handler
        *(--sp) = (U32) (&SVC_RESTORE);
    } else {
        // kernel thread LR: return to the entry point of the task
        *(--sp) = (U32) (p_taskinfo->ptask);
    }

    // kernel stack R0 - R12, 13 registers
    for ( int j = 0; j < 13; j++) {
        *(--sp) = 0x0;
    }

    p_tcb->msp = sp;

    return RTX_OK;
}

/**************************************************************************//**
 * @brief       switching kernel stacks of two TCBs
 * @param:      p_tcb_old, the old tcb that was in RUNNING
 * @return:     RTX_OK upon success
 *              RTX_ERR upon failure
 * @pre:        gp_current_task is pointing to a valid TCB
 *              gp_current_task->state = RUNNING
 *              gp_crrent_task != p_tcb_old
 *              p_tcb_old == NULL or p_tcb_old->state updated
 * @note:       caller must ensure the pre-conditions are met before calling.
 *              the function does not check the pre-condition!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * @attention   CRITICAL SECTION
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *****************************************************************************/
__asm void k_tsk_switch(TCB *p_tcb_old)
{
        PUSH    {R0-R12, LR}
        STR     SP, [R0, #TCB_MSP_OFFSET]   ; save SP to p_old_tcb->msp
K_RESTORE
        LDR     R1, =__cpp(&gp_current_task);
        LDR     R2, [R1]
        LDR     SP, [R2, #TCB_MSP_OFFSET]   ; restore msp of the gp_current_task

        POP     {R0-R12, PC}
}


/**************************************************************************//**
 * @brief       run a new thread. The caller becomes READY and
 *              the scheduler picks the next ready to run task.
 * @return      RTX_ERR on error and zero on success
 * @pre         gp_current_task != NULL && gp_current_task == RUNNING
 * @post        gp_current_task gets updated to next to run task
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * @attention   CRITICAL SECTION
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *****************************************************************************/
int k_tsk_run_new(void)
{
    TCB *p_tcb_old = NULL;
    
    if (gp_current_task == NULL) {
    	return RTX_ERR;
    }

    p_tcb_old = gp_current_task;
    gp_current_task = scheduler();
    
    if ( gp_current_task == NULL  ) {
        gp_current_task = p_tcb_old;        // revert back to the old task
        return RTX_ERR;
    }

    // at this point, gp_current_task != NULL and p_tcb_old != NULL
    if (gp_current_task != p_tcb_old) {
        gp_current_task->state = RUNNING;   // change state of the to-be-switched-in  tcb
        p_tcb_old->state = READY;           // change state of the to-be-switched-out tcb
        k_tsk_switch(p_tcb_old);            // switch stacks
    }

    return RTX_OK;
}

/**************************************************************************//**
 * @brief       yield the cpu
 * @return:     RTX_OK upon success
 *              RTX_ERR upon failure
 * @pre:        gp_current_task != NULL &&
 *              gp_current_task->state = RUNNING
 * @post        gp_current_task gets updated to next to run task
 * @note:       caller must ensure the pre-conditions before calling.
 *****************************************************************************/
int k_tsk_yield(void)
{
    // Example: running task E calls tsk_yield
    // if the priority of hgihest-priority task in teh ready queue, F, is STRICTLY less than that of E, then E continues
    // otherwise, F is scheduled and E is added to the back of the ready queue (last task among tasks with same priority)

    // at the beginning of the function, the front of the priority queue will be the running task 
    // compare this task with the priority of the method after it 
    if (ready_queue-> front-> prio < read_queue-> front-> next-> prio) {
      // the priority of the current running task is strictly higher than the next task
      // that means the current running task will continue to run
      return RTX_OK; 
    } else {
      // the priority of the current running task is either of a priority equal to or less than the next task
      // I can "delete" the task and re-add it to the back of the ready of the queue in order
      TCB* tmp = ready_queue-> front; 
      ready_queue-> front = tmp-> next; 
      queue_add(tmp); 
      // the queue has been resorted via the "scheduler" since queue_add is a method of the scheduler technically
      return k_tsk_run_new();
    }
}


/*
 *===========================================================================
 *                             TO BE IMPLEMETED IN LAB2
 *===========================================================================
 */

int k_tsk_create(task_t *task, void (*task_entry)(void), U8 prio, U16 stack_size)
{
#ifdef DEBUG_0
    printf("k_tsk_create: entering...\n\r");
    printf("task = 0x%x, task_entry = 0x%x, prio=%d, stack_size = %d\n\r", task, task_entry, prio, stack_size);
#endif /* DEBUG_0 */
    if (task == NULL) {
      retunr RTX_ERR;
    }
    // If all tcbs are in use, return -1
    if (g_num_active_tasks == MAX_TASKS){
      return RTX_ERR;
    }
    // return error if the stack size is too small
    if (stack_size < PROC_STACK_SIZE) {
      return RTX_ERR;
    }
    // stack size has to be a multiple of 8
    if (stack_size % 8) {
      return RTX_ERR;
    }
    // return error if prio is invalid
    if (prio != PRIO_RT || prio != HIGH || prio != MEDIUM || prio != LOW || prio != LOWEST || prio != PRIO_NULL) {
      return RTX_ERR;
    }
    // Initialize p_taskinfo to pass into k_tsk_create_new as parameter
    RTX_TASK_INFO *p_taskinfo;
    U8 tid = 0;

    for (int i = 1; i < MAX_TASKS; i++){
      if (g_tcbs[i].state == DORMANT){
        tid = i;
        break;
      }
    }
    if (tid == 0) {
      return RTX_ERR;
    }

    p_taskinfo-> ptask = task_entry;
    p_taskinfo-> tid = tid;
    p_taskinfo-> priv = 0;
    p_taskinfo-> prio = prio; 

    // Set some values for the tcb
    TCB *p_tcb = &g_tcbs[tid];
    p_tcb->prio = prio;
    p_tcb->priv = 0;
    p_tcb->u_stack_size = stack_size;
    p_tcb->next = NULL;

    // Call the k_tsk_create_new function to allocate kernel stack
    if (k_tsk_create_new(p_taskinfo, p_tcb, tid) == RTX_OK) {
      // add the task to the ready queue 
        queue_add(p_tcb);
        g_num_active_tasks++;
        *task = tid
        // run scheduler and may have to premptive run new task
        k_tsk_run_new(); 
    } else {
      return RTX_ERR;
    };
    return RTX_OK;
}

// stops and deletes the currently running task
// once task is terminated, its state becomes DORMANT if its TCB data structure still exists in teh system
// if running test terminates, RTX should schedule another ready task to run 
void k_tsk_exit(void) 
{
#ifdef DEBUG_0
    printf("k_tsk_exit: entering...\n\r");
#endif /* DEBUG_0 */
    //Check if current task is not NULL, and not the null task, and is RUNNING
    if (gp_current_task != NULL && gp_current_task->tid != TID_NULL && gp_current_task->state == RUNNING) {
        gp_current_task->state = DORMANT;
        gp_current_task->k_sp = NULL;
        k_mem_dealloc(gp_current_task->u_sp); //deallote user stack memory, assuming kernel stack memory will be overwritten by the next task
        gp_current_task->u_sp = NULL;
        g_num_active_tasks--;
        k_tsk_run_new();
    return;
}

int k_tsk_set_prio(task_t task_id, U8 prio) 
{
#ifdef DEBUG_0
    printf("k_tsk_set_prio: entering...\n\r");
    printf("task_id = %d, prio = %d.\n\r", task_id, prio);
#endif /* DEBUG_0 */
    //Check if task_id is valid, not null task, and not equal to the priority for null task
    if (task_id != NULL; task_id > MAX_TASKS)
    {
        return RTX_ERR;
    }
    if (prio != PRIO_RT || prio != HIGH || prio != MEDIUM || prio != LOW || prio != LOWEST) {
      return RTX_ERR; 
    }
    // cannot change the priority of the null task
    if (task_id == 0) {
      return RTX_ERR; 
    }
    // cannot change the priority of a task to prio_null
    if (prio == PRIO_NULL) {
      return RTX_ERR; 
    }
    //Check if the current task is a user task, which cannot change the prio of a kernel task
    if (gp_current_task->priv == 0 && g_tcb[task_id]->priv == 1)
    {
        return RTX_ERR;
    }
    //Change the task_id's priority from g_tcb to prio
    g_tcb[task_id]->prio = prio;
    // have to rearrange the queue in case the priorities have changed
    // rearranged task mayb preempt the current task that called this function and run immediately 
    k_tsk_run_new();
    return RTX_OK;
}

// obtain task status information from the kernel identified with task_id
int k_tsk_get(task_t task_id, RTX_TASK_INFO *buffer)
{
#ifdef DEBUG_0
    printf("k_tsk_get: entering...\n\r");
    printf("task_id = %d, buffer = 0x%x.\n\r", task_id, buffer);
#endif /* DEBUG_0 */    
    if (buffer == NULL) {
        return RTX_ERR;
    }
    // return error for invalid task_id
    // TID is an integer between 0 and MAX_TASKS - 1
    if (task_id < 0 || task_id > MAX_TASKS-1) {
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

int k_tsk_ls(task_t *buf, int count){
#ifdef DEBUG_0
    printf("k_tsk_ls: buf=0x%x, count=%d\r\n", buf, count);
#endif /* DEBUG_0 */
    return 0;
}

/*
 *===========================================================================
 *                             TO BE IMPLEMETED IN LAB4
 *===========================================================================
 */

int k_tsk_create_rt(task_t *tid, TASK_RT *task, RTX_MSG_HDR *msg_hdr, U32 num_msgs)
{
    return 0;
}

void k_tsk_done_rt(void) {
#ifdef DEBUG_0
    printf("k_tsk_done: Entering\r\n");
#endif /* DEBUG_0 */
    return;
}

void k_tsk_suspend(struct timeval_rt *tv)
{
#ifdef DEBUG_0
    printf("k_tsk_suspend: Entering\r\n");
#endif /* DEBUG_0 */
    return;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
