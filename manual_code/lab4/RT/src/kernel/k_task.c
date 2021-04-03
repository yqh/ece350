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
#include "printf.h"
#ifdef DEBUG_0

#endif /* DEBUG_0 */

/*
 *==========================================================================
 *                            GLOBAL VARIABLES
 *==========================================================================
 */

TCB             *gp_current_task = NULL;  // the current RUNNING task
TCB             g_tcbs[MAX_TASKS];      // an array of TCBs
TMB             t_mailbox[MAX_TASKS];
RTX_TASK_INFO   g_null_task_info;     // The null task info
U32             g_num_active_tasks = 0;   // number of non-dormant tasks
U8              *uart_buffer;         // buffer for the command registration
STL             *suspended_queue;  // list for suspended tasks

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
};

struct Queue *ready_queue;

// function to add a t_block to the queue
// follows strict priority ordering and first come first serve
// (newer tasks will be placed behind older tasks of the same priority)
void queue_add(TCB* t_block) { 
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
      t_comp = t_comp-> next;
      // compare the priorities of the task to be added and the current task in the queue
      // 0 Prio == HIGHEST

      // should place new tasks in front of BLOCKED/SUSPENDED tasks of the same state
      // this way, when those tasks become ready again, we don't have to readd them back into the ready queue
      // since they are supposed to be placed at the back of their respective priority 
      if ((t_block-> prio == t_comp-> prio && t_comp-> prio == BLOCKED || t_comp-> prio == SUSPENDED) || (t_block-> prio < t_comp-> prio)) {
        // if the prio is the less, then we have found where to insert the task in the ready queue
        // the previous task that has a higher priority will now have next point to the new added task
        // new added task next will now point to t_comp since t_comp has a lower priority
        prev-> next = t_block;
        t_block-> next = t_comp;
        break;
      }
      prev = prev-> next;
      if (t_comp == NULL) {
        // technically should never get here since the last task in the queue should always be the PRIO NULL TASK
        // we cannot add a task with prio >= 255
        printf("OOPS, WE SHOULDN'T BE HERE");
      }
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

  while (t_comp-> next != NULL) {
    if (t_comp->state == READY || t_comp->state == RUNNING) {
      break;
    }
    // check if any have been modified
    // set compare pointer to next task
    t_comp = t_comp-> next;
  }
  // if we reach this, that means that the order of the tasks has not changed
  // can be because we change the priority of the task, but the queue maintains proper order
  // simply return the front task of the queue
  return t_comp;
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

  //initialize queue
  ready_queue = (struct Queue*)k_mem_alloc(sizeof(struct Queue));
  suspended_queue = (STL*)k_mem_alloc(sizeof(STL));

  suspended_queue-> front = NULL; 

  // allocate the memory for the uart buffer that will be used in the interrupt 
  uart_buffer = (U8*)k_mem_alloc(sizeof(RTX_MSG_HDR) + sizeof(char));

  RTX_TASK_INFO *p_taskinfo = &g_null_task_info;
  g_num_active_tasks = 0;

  if (num_tasks > MAX_TASKS - 1) {
    return RTX_ERR;
  }

  // create the first null task
  TCB *p_tcb = &g_tcbs[0];
  p_tcb->prio = PRIO_NULL;
  p_tcb->priv = 1;
  p_tcb->tid = TID_NULL;
  p_tcb->state = RUNNING;
  p_tcb-> num_msgs = 0; 
  // initialize fields for mailbox stuff
  TMB *t_mb = &t_mailbox[0];
  t_mb-> mb_capacity = 0;
  t_mb-> mb_buffer = NULL;
  t_mb-> mb_buffer_end = NULL;
  t_mb-> mb_head = NULL; 
  t_mb-> mb_tail = NULL;
  g_num_active_tasks++;
  gp_current_task = p_tcb;
  // update the schedule ready queue by adding the new task in the appropriate order
  // this is the first task, so obviously this will be the highest priority task
  // only task in the ready queue, so there will be no tasks after it
  p_tcb-> next = NULL;
  ready_queue-> front = p_tcb;

  // set the rest of the tcbs to be dormant
  for (int i = 1; i < MAX_TASKS; i++) {
    TCB *p_tcb_d = &g_tcbs[i];
    p_tcb_d-> state = DORMANT;
  }

  // create the rest of the tasks    
  p_taskinfo = task_info;
  for ( int i = 1; i <= num_tasks; i++ ) {
    // first check if the task is the kcd_task
    TCB *p_tcb_new = &g_tcbs[i];
    TMB *t_mb_new = &t_mailbox[i];
    if (p_taskinfo-> ptask == kcd_task) {
      p_tcb_new = &g_tcbs[TID_KCD];
      t_mb_new = &t_mailbox[TID_KCD];
    } else {
      // if the task is not the kcd_task, have to still reserve TID_KCD for the kcd_task
      if (i == TID_KCD) {
        // move to next iteration of for loop 
        continue;
      }
    }
    p_tcb_new-> prio = p_taskinfo-> prio;
    p_tcb_new-> next = NULL;
    p_tcb_new-> priv = p_taskinfo-> priv;
    p_tcb_new-> num_msgs = 0; 
    t_mb_new-> mb_capacity = 0; 
    t_mb_new-> mb_buffer = NULL;
    t_mb_new-> mb_buffer_end = NULL;
    t_mb_new-> mb_head = NULL; 
    t_mb_new-> mb_tail = NULL;

    int tmp = p_taskinfo-> ptask == kcd_task ? TID_KCD : i;

    // have extra checks if it's a real time task
    if (p_tcb_new-> prio == PRIO_RT) {
      p_tcb_new-> p_n = p_taskinfo-> p_n; 
      p_tcb_new-> rt_mbx_size = p_taskinfo-> rt_mbx_size;
      p_tcb_new-> suspended_time_remaining = 0; 
      p_tcb_new-> next_suspended = NULL;
      if (p_tcb_new-> p_n-> usec % 500) {
        return RTX_ERR; 
      }
      // requested time period is 0 
      if (p_tcb_new-> p_n-> usec == 0 || p_tcb_new-> p_n-> sec == 0) {
        return RTX_ERR;
      }
      // if mbx_size is non-zero create a mailbox for this task 
      if (p_tcb_new-> rt_mbx_size > 0) {
        if (k_mbx_create_task(tmp, p_tcb_new-> rt_mbx_size) == RTX_ERR) {
          // return error if cannot create the mailbox for the task 
          return RTX_ERR;
        }
      }
    }

    if (k_tsk_create_new(p_taskinfo, p_tcb_new, tmp) == RTX_OK) {
        // update the schedule ready queue by adding the new task in the appropriate order
        // the front of the queue is the highest prio task, which should be the current task
        // add the task to the end of the queue
      queue_add(p_tcb_new);
      g_num_active_tasks++;
    }
    p_taskinfo++;
  }
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
  if (p_tcb == NULL)
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

    // store the stack pointer in the k_sp
  p_tcb->k_sp = (U32)sp;
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

      *(--sp) = (U32) k_alloc_p_stack(tid);

//        U32 tmp = (U32) k_alloc_u_stack(tid, (size_t)p_tcb-> u_stack_size);
//        *(--sp) = tmp;
//        p_tcb->u_stack_hi = tmp;
//        p_tcb->u_sp = tmp;


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
    if (p_tcb_old->state != BLK_MSG){
        p_tcb_old->state = READY;           // change state of the to-be-switched-out tcb
      }
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

  if (gp_current_task != ready_queue-> front) {
    return k_tsk_run_new();
  }
  // Example: running task E calls tsk_yield
  // if the priority of hgihest-priority task in teh ready queue, F, is STRICTLY less than that of E, then E continues
  // otherwise, F is scheduled and E is added to the back of the ready queue (last task among tasks with same priority)

  TCB *t_comp = ready_queue-> front-> next;

  while (t_comp-> next != NULL && (t_comp-> state != DORMANT || t_comp-> state != BLK_MSG)) {
    // check if any have been modified
    // set compare pointer to next task
    t_comp = t_comp-> next;
  }

  // at the beginning of the function, the front of the priority queue will be the running task
  // compare this task with the priority of the method after it
  if (gp_current_task-> prio < t_comp-> prio) {
    // the priority of the current running task is strictly higher than the next task
    // that means the current running task will continue to run
    printf("%d, %d\n", ready_queue->front->tid, ready_queue->front->prio);
    return RTX_OK;
  } else {
    // the priority of the current running task is either of a priority equal to or less than the next task
    // I can "delete" the task and re-add it to the back of the ready of the queue in order
    TCB *p_tcb_old = gp_current_task;
    gp_current_task = t_comp;

    gp_current_task->state = RUNNING;   // change state of the to-be-switched-in  tcb
    if (p_tcb_old->state != BLK_MSG){
      p_tcb_old->state = READY;           // change state of the to-be-switched-out tcb
    }
    k_tsk_switch(p_tcb_old);            // switch stacks

    return RTX_OK;
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
    return RTX_ERR;
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
  if (prio != PRIO_RT && prio != HIGH && prio != MEDIUM && prio != LOW && prio != LOWEST && prio != PRIO_NULL) {
    return RTX_ERR;
  }
    // Initialize p_taskinfo to pass into k_tsk_create_new as parameter
  RTX_TASK_INFO *p_taskinfo;
  U8 tid = 0;
  for (int i = 1; i < MAX_TASKS; i++){
    TCB *p_tcb_d = &g_tcbs[i];
    if (p_tcb_d-> state == DORMANT){
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
  p_tcb-> num_msgs = 0;

  TMB *t_mb = &t_mailbox[tid];
  t_mb-> mb_capacity = 0;
  t_mb-> mb_buffer = NULL;
  t_mb-> mb_buffer_end = NULL;
  t_mb-> mb_head = NULL; 
  t_mb-> mb_tail = NULL; 
    // Call the k_tsk_create_new function to allocate kernel stack
  if (k_tsk_create_new(p_taskinfo, p_tcb, tid) == RTX_OK) {
      // add the task to the ready queue
    queue_add(p_tcb);
    g_num_active_tasks++;
    *task = tid;
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
     // deallocate the mailbox stuff
    TMB *t_mb = &t_mailbox[gp_current_task->tid];
    k_mem_dealloc(t_mb-> mb_buffer);
    gp_current_task-> num_msgs = 0; 
    t_mb-> mb_capacity = 0; 
    t_mb-> mb_buffer = NULL;
    t_mb-> mb_buffer_end = NULL;
    t_mb-> mb_head = NULL; 
    t_mb-> mb_tail = NULL; 
    // set the current task to null
    gp_current_task->state = DORMANT;
    // reduce the number of active tasks
    g_num_active_tasks--;
    // fix the ready queue 
    TCB *t_comp = ready_queue-> front;
    TCB *t_prev = ready_queue-> front;

    while (t_prev != NULL && t_comp != NULL) {
      t_comp = t_comp-> next;
      if (t_comp->tid == gp_current_task-> tid) {
        // since we are removing this task from the queue, we need to tie the task before the calling task to the task after the calling task
        t_prev->next = t_comp->next;
        break;
      }
      t_prev = t_prev->next;
    }

    // actually should run a modified version of k_tsk_run_new() because k_tsk_run_new() will set this task to ready instead of dormant

    // modified k_tsk_run_new()
    // grab the next priority task in the ready queue
    // initially the front of the queue is the current task that we just set to dormant so the ready queue should remove that from the queue and return the next in line
    TCB *p_tcb_old = gp_current_task;
    gp_current_task = scheduler();
    if (gp_current_task == NULL) {
        printf("SCHEDULER ERROR"); // should not get this, should at the very least return the null task
    }
    // compare to make sure the next task in the queue is not the same task as the current task that we just set dormant
    if (gp_current_task != p_tcb_old) {
      // set the state of the newe highest priority task to running
      gp_current_task-> state = RUNNING;
      // old task is already set state to dormant
      k_tsk_switch(p_tcb_old);
    }
  }
}

int k_tsk_set_prio(task_t task_id, U8 prio)
{
  #ifdef DEBUG_0
    printf("k_tsk_set_prio: entering...\n\r");
    printf("task_id = %d, prio = %d.\n\r", task_id, prio);
  #endif /* DEBUG_0 */
  //Check if task_id is valid, not null task, and not equal to the priority for null task
  if (task_id == NULL || task_id > MAX_TASKS || task_id == 0)
  {
    return RTX_ERR;
  }
  if (prio != PRIO_RT && prio != HIGH && prio != MEDIUM && prio != LOW && prio != LOWEST && prio > 255) {
    return RTX_ERR;
  }
  // cannot change the priority of a task to prio_null
  if (prio == PRIO_NULL) {
    return RTX_ERR;
  }
  //Check if the current task is a user task, which cannot change the prio of a kernel task
  if (gp_current_task->priv == 0 && g_tcbs[task_id].priv == 1)
  {
    return RTX_ERR;
  }
  // cannot change a real-time task's priority
  if (g_tcbs[task_id]-> prio == PRIO_RT) {
    return RTX_ERR; 
  }
  // cannot change priortiy to PRIO_RT
  if (prio == PRIO_RT) {
    return RTX_ERR; 
  }
  //Change the task_id's priority from g_tcb to prio

  g_tcbs[task_id]-> prio = prio;
  TCB *t_comp = ready_queue-> front;
  TCB *t_prev = ready_queue-> front;

  while (t_prev != NULL && t_comp != NULL) {
    t_comp = t_comp-> next;
    if (t_comp->tid == task_id) {
      t_prev->next = t_comp->next;
      queue_add(t_comp);
      break;
    }
    t_prev = t_prev->next;
  }

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
  if (task_id > MAX_TASKS)
  {
    return RTX_ERR;
  }

  // return error when called on a dormant task
  if (g_tcbs[(int)task_id].state == DORMANT) {
    return RTX_ERR;
  }

  /* The code fills the buffer with some fake task information.
     You should fill the buffer with correct information    */

  // note that each unpriviledged user-mode task has a user-space stack and a kernel stack
  // priviledged kernel task only has kernel stack;
  buffer->tid = task_id;
  buffer->prio = g_tcbs[(int)task_id].prio;
  buffer->state = g_tcbs[(int)task_id].state;
  buffer->priv = g_tcbs[(int)task_id].priv;
  buffer->ptask = g_tcbs[(int)task_id].ptask;
  if (g_tcbs[(int)task_id].prio == PRIO_RT) {
    // include real time tasks' period 
    buffer-> p_n = g_tcbs[(int)task_id].p_n;
    buffer-> rt_mbx_size = g_tcbs[(int)task_id].rt_mbx_size;
  }

  // k_tsk_create_new runs SVC_RESTORE which sotres the user stack pointer on top in R0, on top of the kernel stack
  if (g_tcbs[(int)task_id].priv == 1) {
    // only has kernel stack
    // user stuff can be set to NULL
    buffer-> u_sp = NULL;
    buffer-> u_stack_size = NULL;
  } else {
    // has kernel and user stack
    // the stack pointer was assigned during the mem_alloc call
    buffer-> u_sp = g_tcbs[(int)task_id].u_sp;
    buffer-> u_stack_size = g_tcbs[(int)task_id].u_stack_size;
  }
  // kernel stack info
  buffer-> k_sp = g_tcbs[(int)task_id].k_sp;
  buffer-> k_stack_size = KERN_STACK_SIZE;

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

int  k_tsk_create_rt(task_t *tid, TASK_RT *task);
{
  // create a real time task 
  // non-blocking function that creates a new real time task 
  // priority of the new task is set to PRIO_RT (0)
  // task argument defines the real-time task parameters 

  // possible causes of failure 
  // time specified by p_n is not a multiple of 500 ms 
  // TIMEVAL has sec and usec properties which are both type U32 (int)
  // just have to check the usec (microsecond property)
  if (task-> p_n-> usec % 500) {
    return RTX_ERR; 
  }
  // requested time period is 0 
  if (task-> p_n-> usec == 0 || task-> p_n-> sec == 0) {
    return RTX_ERR;
  }
  // if the number of tasks in system is maximum 
  if (g_num_active_tasks == MAX_TASKS) {
    return RTX_ERR; 
  }
  // requested user-space stack is too small or more than available free memory 
  if (task-> u_stack_size < PROC_STACK_SIZE) {
    return RTX_ERR; 
  }
  // stack size has to be a multiple of 8
  if (task-> u_stack_size % 8) {
    return RTX_ERR; 
  }
  // not enough memory to allocate to the mailbox

  RTX_TASK_INFO *p_taskinfo;
  U8 id = 0;
  for (int i = 1; i < MAX_TASKS; i++){
    TCB *p_tcb_d = &g_tcbs[i];
    if (p_tcb_d-> state == DORMANT){
      tid = i;
      break;
    }
  }

  if (id == 0) {
    return RTX_ERR;
  }

  p_taskinfo-> ptask = task-> task_entry;
  p_task-> tid = id; 
  p_taskinfo-> prio = PRIO_RT;

  // set some values for the tcb 
  TCB *p_tcb = &g_tcbs[id]; 
  p_tcb-> prio = PRIO_RT;
  p_tcb-> priv = 0; 
  p_tcb-> u_stack_size = task-> u_stack_size;
  p_tcb-> next = NULL;
  p_tcb-> num_msgs = 0; 
  p_tcb-> rt_mbx_size = task-> rt_mbx_size; 
  p_tcb-> suspended_time_remaining = 0; 
  p_tcb-> next_suspended = NULL;

  TMB *t_mb = &t_mailbox[id];
  t_mb-> mb_capacity = 0;
  t_mb-> mb_buffer = NULL;
  t_mb-> mb_buffer_end = NULL;
  t_mb-> mb_head = NULL; 
  t_mb-> mb_tail = NULL; 

  // if mbx_size is non-zero create a mailbox for this task 
  if (task-> rt_mbx_size > 0) {
    if (k_mbx_create_task(id, task-> rt_mbx_size) == RTX_ERR) {
      // return error if cannot create the mailbox for the task 
      return RTX_ERR;
    }
  }

  if (k_tsk_create_new(p_taskinfo, p_tcb, id) == RTX_OK) {
    // add the task to the ready queue
    queue_add(p_tcb); 
    g_num_active_tasks++;
    // store teh taskID of the newly-created task in the buffer pointed to by tid
    *tid = id;
    k_tsk_run_new(); 
  } else {
    return RTX_ERR;
  }

  return RTX_OK;
}

void k_tsk_done_rt(void) {
  // suspend a periodic task until its next period
  #ifdef DEBUG_0
    printf("k_tsk_done: Entering\r\n");
  #endif /* DEBUG_0 */
  // can only be used by real-time tasks to indicate end of execution of one job
  if (gp_current_task-> prio != PRIO_RT) {
    return;
  }
  // RTX resets the calling task's stack to its starting address 
  
  // also reset task's PC to its task_entry 
  // if finished job has met its deadline, suspend task (change state to SUSPENDED)

  return;
}

void k_tsk_suspend(struct timeval_rt *tv)
{
  // suspend the calling task for a specified time
  #ifdef DEBUG_0
    printf("k_tsk_suspend: Entering\r\n");
  #endif /* DEBUG_0 */
  // tv has to be a multiple of 500 usec
  // state of the task is set to SUSPENDED
  if (tv-> usec % 500) {
    // if the time is not a multiple of 500 usec, function returns immediately without suspending
    return; 
  } 
  if (tv-> usec == 0 && tv-> sec == 0) {
    // if time is 0, return immediately without suspending the calling task
    return; 
  }
  // change state to suspended 
  gp_current_task-> state = SUSPENDED; 

  /* suspended list stuff */ 
  // convert tv to purely nanoseconds
  U32 time = ((tv-> sec * 1000000) + tv-> usec) * 1000;
  gp_current_task-> suspended_time_remaining = time; 
  // add the task to the suspended list (ordered with ascending remaining suspended time)
  if (suspended_queue-> front == NULL) {
    // no other suspended tasks, so this should go first 
    suspended_queue-> front = gp_current_task; 
    gp_current_task-> next_suspended = NULL;
    // since no other suspended tasks, we have to start timer interrupt
    // note that this argument is in picoseconds so convert nanosecond to pico (x1000)
    config_hps_timer(1, time * 1000, 1, 0);
  } else {
    // there is at least 1 task suspended, compare the front first
    TCB* prev = suspended_queue-> front;
    if (time < prev-> suspended_time_remaining) {
      // calling task should suspend earlier, so this one goes infront 
      gp_current_task-> next_suspended = prev; 
      suspended_queue-> front = gp_current_task; 
    } else {
      // move to the next one 
      TCB* curr = suspended_queue-> front; 
      while(curr != NULL) {
        curr = curr-> next_suspended;
        if (curr == NULL) {
          prev-> next_suspended = gp_current_task;
          gp_current_task-> next_suspended = NULL; 
          break;
        } else {
          if (time < curr-> suspended_time_remaining) {
            // slot calling task between prev and curr
            prev-> next_suspended = gp_current_task;
            gp_current_task-> next_suspended = curr; 
            break; 
          }
        }
        // advance to next one
        prev = prev-> next_suspended; 
      }
    }
  }
  // kernel picks another ready-to-run task to execute after calling task is suspended 
  // after tv time elapsed, RTX wakes up suspending task by setting its state to READY 
  // this will be done in the timer interrupt
  k_tsk_run_new();
  return;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
