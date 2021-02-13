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
 * @file        priv_tasks.c
 * @brief       Two privileged tasks: priv_task1 and priv_task2
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @note        Each task is in an infinite loop. These Tasks never terminate.
 *
 *****************************************************************************/

#include "ae_priv_tasks.h"
#include "ae_usr_tasks.h"
#include "Serial.h"
#include "printf.h"

/**************************************************************************//**
 * @brief       a task that prints AAAAA, BBBBB, CCCCC,...., ZZZZZ on each line.
 *              It yields the cpu every 6 lines are printed.
 *****************************************************************************/

void *p; 

void priv_task1(void)
{
    int i = 0;
    int j = 0;
    long int x = 0;
    int ret_val = 10;
    
    while (1) {
        char out_char = 'A' + i%26;
        for (j = 0; j < 5; j++ ) {
            SER_PutChar(out_char);
        }
        SER_PutStr("\n\r");
        
        for ( x = 0; x < DELAY; x++); // some artificial delay
        if ( (++i)%6 == 0 ) {
            SER_PutStr("priv_task1 before yielding cpu.\n\r");
            ret_val = k_tsk_yield();
            SER_PutStr("priv_task1 after yielding cpu.\n\r");
            printf("priv_task1: ret_val=%d\n\r", ret_val);
#ifdef DEBUG_0
            printf("priv_task1: ret_val=%d\n\r", ret_val);
#endif /* DEBUG_0 */
        }
    }
}

/**************************************************************************//**
 * @brief:      a task that prints 00000, 11111, 22222,....,99999 on each line.
 *              It yields the cpu every 6 lines are printed
 *              before printing these lines indefinitely, it creates a
 *              new task and and obtains the task information. It then
 *              changes the newly created task's priority.
 *****************************************************************************/

void priv_task2(void)
{
    long int x = 0;
    int ret_val = 10;
    int i = 0;
    int j = 0;
    RTX_TASK_INFO task_info;
    task_t tid;

    k_tsk_create(&tid, &task1, LOW, 0x200);  /*create a user task */
    k_tsk_get(tid, &task_info);
    k_tsk_set_prio(tid, LOWEST);


    for (i = 1;;i++) {
        char out_char = '0' + i%10;
        for (j = 0; j < 5; j++ ) {
            SER_PutChar(out_char);
        }
        SER_PutStr("\n\r");
        
        for ( x = 0; x < DELAY; x++); // some artifical delay
        if ( i%6 == 0 ) {
            SER_PutStr("priv_task2 before yielding cpu.\n\r");
            ret_val = k_tsk_yield();
            SER_PutStr("priv_task2 after yielding cpu.\n\r");
            printf("priv_task2: ret_val=%d\n\r", ret_val);
#ifdef DEBUG_0
            //printf("priv_task2: ret_val=%d\n\r", ret_val);
#endif /* DEBUG_0 */
        }
    }
}

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
    // yield for the 10 tasks created by the user task to run and terminate
    ret_val = k_tsk_yield();

    // when the user task is terminated, this task should resume 
    SER_PutStr("Test Case 2 - Scheduling Tasks with Different Priority with Preeumption\r\n")j
    // will loop and creating 4 tasks of different priorities 
    // will check to make sure that the scheduler sorts and runs the tasks in the proper order on prio
    // will first make 2 tasks of low priority (shouldn't preeumpt) 
    SER_PutStr("Creating Low Prio Task 1\r\n");
    k_tsk_create(&t_set_id_1, &task_low_prio, LOW, 0x200); 
    SER_PutStr("Creating Another Low Prio Task 2\r\n")
    k_tsk_create(&t_set_id_2, &task_low_prio, LOW, 0x200); 
    // next create the medium task (equal prio, shouldn't preeumpt)
    SER_PutStr("Creating Medium Prio Task\r\n")
    k_tsk_create(&tid, &task_medium_prio, MEDIUM, 0x200); 
    // next create the high prio task (higher prio, should preeumpt)
    SER_PutStr("Creating High Prio Task\r\n");
    k_tsk_create(&tid, &task_high_prio, HIGH, 0x200); 

    // yield to let the medium prio task run 
    SER_PutStr("User Tasks Created, Now Yielding\r\n");
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
    SER_PutStr("Setting Low Prio Task 2 to Medium Prio");
    k_tsk_set_prio(tid_set_id_2, MEDIUM);

    // yield the task to let the Low Prio Task 2 (now medium) task run and eventually terminate 
    SER_PutStr("Yielding...\r\n");
    ret_val = k_tsk_yield(); 

    // all prio tasks have been terminated so now only this task and null task are in the ready queue
    SER_PutStr("Test Case 3 - Memory Deallocation Testing\r\n");
    // allocate some memory
    // create a task that will try to deallocate said memory 
    // is supposed to fail because that task does not own the memory 
    p = k_mem_alloc(8);
    // create task with high priority to preeumpt 
    k_tsk_create(&tid, &task_dealloc, HIGH);
    // task return to running state
    // finished all tasks
    for (int i = 0;; i++) {
        // run infinite loop now
    }
}

// user task for test case 3 
void task_dealloc(void)
{
    SER_PutStr("Dealloc Task Running...\r\n"); 
    if (mem_dealloc(p) == -1) {
        SER_PutStr("Task unable to deallocate memory it doesn't own...\r\n"); 
    }
    tsk_exit(); 
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
