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
 * @file        ae.c
 * @brief       Automated Evaluation (AE) Framework Source File
 *
 * @version     V1.2021.01.lab2
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @attention
 * @note
 * @details
 *
 *****************************************************************************/

#include "ae.h"

/**************************************************************************//**
 * @brief   	ae_init
 * @return		RTX_OK on success and RTX_ERR on failure
 * @param[out]  sys_info system initialization struct AE writes to
 * @param[out]	task_info boot-time tasks struct array AE writes to
 *
 *****************************************************************************/

int ae_init(RTX_SYS_INFO *sys_info, RTX_TASK_INFO *task_info, int num_tasks) {
    if (ae_set_sys_info(sys_info) != RTX_OK) {
        return RTX_ERR;
    }

    ae_set_task_info(task_info, num_tasks);
    return RTX_OK;
}

/**************************************************************************//**
 * @brief       fill the sys_info struct with system configuration info.
 * @return		RTX_OK on success and RTX_ERR on failure
 * @param[out]  sys_info system initialization struct AE writes to
 *
 *****************************************************************************/
int ae_set_sys_info(RTX_SYS_INFO *sys_info) {
    if (sys_info == NULL) {
        return RTX_ERR;
    }

    // Scheduling sys info set up, only do DEFAULT in lab2
    sys_info->sched = DEFAULT;

    /************* NOT USED in LAB2 ********************
    struct timeval_rt budget;
    struct timeval_rt period;

    budget.sec = 0;
    budget.usec = MIN_RTX_QTM * 10;

    period.sec = 0;
    period.usec = MIN_RTX_QTM * 100;


    sys_info->rtx_time_qtm = 10 * MIN_RTX_QTM;

    sys_info->server.b_n = budget;
    sys_info->server.p_n = period;
    ****************************************************/
    return RTX_OK;
}

/**************************************************************************//**
 * @brief       fill the tasks array with information
 * @param[out]  tasks 		An array of RTX_TASK_INFO elements to write to
 * @param[in]	num_tasks	The length of tasks array
 * @return		None
 *****************************************************************************/

// priviledged task for test cases  
void priv_task_test1(void)
{
    // will create the task_create user task
    // then yield to let these tasks run 
    RTX_TASK_INFO task_info;
    task_t tid;
    task_t t_set_id_1;
    task_t t_set_id_2; 

    // create task will be of equal priority 
    SER_PutStr("Test Case 1 - Creating and Terminating User-Mode Tasks And Scheduling With Same Priority\r\n"); 
    // create my medium prio task 
    // should not switch since it is of the same priority 
    k_tsk_create(&tid, &task_create, MEDIUM, 0x200);
    // check the info for the task 
    k_tsk_get(tid, &task_info); 
    
    // yield the task to let the user task run and eventually terminate 
    SER_PutStr("User Task Created, Now Yielding\r\n");
    ret_val = k_tsk_yield(); 

    // yielding should now put this task to the end of the behind the task_create task 

    // when the user task is terminated, this task should resume 
    SER_PutStr("Test Case 2 - Scheduling Tasks with Different Priority with Preeumption\r\n")
    // will loop and creating 4 tasks of different priorities 
    // will check to make sure that the scheduler sorts and runs the tasks in the proper order on prio
    // will first make 2 tasks of low priority (shouldn't preeumpt) 
    SER_PutStr("Creating Low Prio Task 1\r\n")
    k_tsk_create(&t_set_id_1, &task_low_prio, LOW, 0x100); 
    SER_PutStr("Creating Another Low Prio Task 2\r\n")
    k_tsk_create(&t_set_id_2, &task_low_prio, LOW, 0x100); 
    // next create the high prio task (higher prio, should preeumpt)
    SER_PutStr("Creating High Prio Task\r\n")
    k_tsk_create(&tid, &task_high_prio, HIGH, 0x100); 
    // next create the medium task (equal prio, shouldn't preeumpt)
    SER_PutStr("Creating Medium Prio Task\r\n")
    k_tsk_create(&tid, &task_medium_prio, MEDIUM, 0x100); 

    // yield to let the medium prio task run 
    SER_PutStr("User Task Created, Now Yielding\r\n");
    ret_val = k_tsk_yield();

    // now in the queue should be the current task, the first low prio task, the second low prio task, and the null task
    // yield again to make sure that since the other tasks in the queue are of lower prio, yielding should just make this task run again
    SER_PutStr("Yielding again, but this shouldn't do anything\r\n");
    ret_val = k_tsk_yield();
    // will now set the prio task of a low task to high to preeumpt again
    SER_PutStr("Setting Low Prio Task 1 to High Prio\r\n");
    k_tsk_set_prio(tid_set_id_1, HIGH);
    // will preeumpt 
    // when it returns set the prio of the low task to medium (equal) so it shouldn't preeumpt
    SER_PutStr("Setting Low Prio Task 2 to Medium Prio")
    k_tsk_set_prio(tid_set_id_2, MEDIUM);

    // yield the task to let the Low Prio Task 2 (now medium) task run and eventually terminate 
    SER_PutStr("Yielding...\r\n");
    ret_val = k_tsk_yield(); 

    // all prio tasks have been terminated so now only this task and null task are in the ready queue
    SER_PutStr("Test Case 3 - Memory Deallocation Testing\r\n");
    // allocate some memory
    // create a task that will try to deallocate said memory 
    // is supposed to fail because that task does not own the memory 

    void *p = k_mem_alloc(8);
    // create task with high priority to preeumpt 
    k_tsk_create(&tid, &task_dealloc, HIGH);
    // task return to running state
    // finished all tasks
    for (int i = 0;; i++) {
        // run infinite loop now
    }
}

// user tasks for test case 1
void task_create(void)
{
    SER_PutStr("Entering Task_Create Task\r\n");
    task_t tid;
    RTX_TASK_INFO task_info[10];
    // create a simple task a bunch of tasks that basically just terminate 
    for (int i = 0; i < 10; i++) {
        // create 10 tasks that will get just terminate when they run
        SER_PutStr("creating sub user-task\r\n")
        tsk_create(&tid, &task_terminate, MEDIUM, 0x100); 
        tsk_get(tid, &task_info[i]); 
    }
    // after creating all the tests, you can exit this task to run the terminate tasks as well
    tsk_exit(); 
}
void task_terminate(void)
{
    SER_PutStr("task created...\r\n"); 
    // allocate your data
    void *p; 
    SER_PutStr("allocating 8 bytes...\r\n"); 
    p = mem_alloc(8); 
    // deallocate your data 
    SER_PutStr("deallocating 8 bytes...\r\n"); 
    if (mem_dealloc(p) != 0) {
        SER_PutStr("deallocation failed!");
    }
    // exit
    SER_PutStr("task exiting...\r\n"); 
    tsk_exit(); 
}

// user tasks for test case 2
void task_low_prio(void)
{
    SER_PutStr("low prio task running...\r\n"); 
    tsk_exit();
}
void task_medium_prio(void)
{
    SER_PutStr("medium prio task running...\r\n");
    tsk_ext(); 
}
void task_high_prio(void)
{
    SER_PutStr("high prio task running...\r\n"); 
    tsk_exit();
}

// user task for test case 3 
void task_dealloc((void*)p)
{
    SER_PutStr("Dealloc Task Running...\r\n"); 
    if (mem_dealloc(p) == -1) {
        SER_PutStr("Task unable to deallocate memory it doesn't own...\r\n"); 
    }
    tsk_exit(); 
}


void ae_set_task_info(RTX_TASK_INFO *tasks, int num_tasks) {

    if (tasks == NULL) {
        return;
    }

    for (int i = 0; i < num_tasks; i++ ) {
        tasks[i].u_stack_size = 0x0;
        tasks[i].prio = MEDIUM;
        tasks[i].priv = 1;
    }
    tasks[0].ptask = &priv_task_test1;

    return;
}

/* only used in LAB1 */
#ifdef AE_LAB1
int ae_start(void) {
    return test_mem();
}
#endif

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
