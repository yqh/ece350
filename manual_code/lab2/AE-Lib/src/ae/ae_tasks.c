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
 * @version     V1.2021.05
 * @authors     Yiqing Huang
 * @date        2021 MAY
 *
 * @note        Each task is in an infinite loop. These Tasks never terminate.
 *
 *****************************************************************************/

#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"

void set_ae_tasks(TASK_INIT *tasks, int num)
{
    for (int i = 0; i < num; i++ ) {                                                 
        tasks[i].u_stack_size = PROC_STACK_SIZE;    
        tasks[i].prio = HIGH;
        tasks[i].priv = 1;
    }
    tasks[0].priv  = 1;
    tasks[0].ptask = &priv_task1;
    tasks[1].priv  = 0;
    tasks[1].ptask = &task1;
}

int dump_task_info(RTX_TASK_INFO *ptr) 
{

    if ( ptr == NULL ) {
        return RTX_ERR;
    }
    
    printf("tid: %d\r\n", ptr->tid);
    printf("state: %d\r\n", ptr->state);
    printf("prio: %d\r\n", ptr->prio);
    printf("k_sp_base: 0x%x\r\n", ptr->k_sp_base);
    printf("k_sp: 0x%x\r\n", ptr->k_sp);
    printf("k_stack_size: 0x%x\r\n", ptr->k_stack_size);
    printf("u_sp_base: 0x%x\r\n", ptr->u_sp_base);
    printf("u_sp 0x%x\r\n", ptr->u_sp);
    printf("u_stack_size: 0x%x\r\n", ptr->u_stack_size);
    
    return RTX_OK;
}

/**************************************************************************//**
 * @brief       a task that prints AAAAA, BBBBB, CCCCC,...., ZZZZZ on each line.
 *              It yields the cpu every 6 lines are printed.
 *****************************************************************************/

void priv_task1(void)
{
    int i = 0;
    int j = 0;
    long int x = 0;
    int ret_val = 10;
    task_t tid = tsk_gettid();
    
    printf("priv_task1: TID =%d\r\n", tid);
    while (1) {
        char out_char = 'A' + i%26;
        for (j = 0; j < 5; j++ ) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\n\r");
        
        for ( x = 0; x < DELAY; x++); // some artificial delay
        if ( (++i)%6 == 0 ) {
            uart1_put_string("priv_task1 before yielding cpu.\n\r");
            ret_val = tsk_yield();
            uart1_put_string("priv_task1 after yielding cpu.\n\r");
            printf("priv_task1: ret_val=%d\n\r", ret_val);
#ifdef DEBUG_0
            printf("priv_task1: tid = %d, ret_val=%d\n\r", tid, ret_val);
#endif /* DEBUG_0 */
        }
    }
}

/**************************************************************************//**
 * @brief:      a task that prints 00000, 11111, 22222,....,99999 on each line.
 *              It yields the cpu every 6 lines are printed
 *              before printing these lines indefinitely, it does the following:
 *              - creates a new task
 *              - obtains the task information. 
 *              - changes the newly created task's priority.
 *              - allocates some memory
 *              - deallocates some memory
 *              - calls memory dump function
 *****************************************************************************/

void task1(void)
{
    long int x = 0;
    int ret_val = 10;
    int i = 0;
    int j = 0;
    static RTX_TASK_INFO task_info; /* our stack space is small, so make it static local */
    task_t tid;

    
    tsk_create(&tid, &task2, LOW, 0x200);  /*create a user task */
    tsk_get(tid, &task_info);
    dump_task_info(&task_info);
    tsk_set_prio(tid, LOWEST);

    int *ptr = mem_alloc(sizeof(int));
    printf("ptr = 0x%x\r\n", ptr); 
    mem_dealloc(ptr);
    mem_dump();

    tid = tsk_gettid();
    printf("task1: TID =%d\r\n", tid); 
    for (i = 1;;i++) {
        char out_char = '0' + i%10;
        for (j = 0; j < 5; j++ ) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\n\r");
        
        for ( x = 0; x < DELAY; x++); // some artifical delay
        if ( i%6 == 0 ) {
            uart1_put_string("task1 before yielding cpu.\n\r");
            ret_val = tsk_yield();
            uart1_put_string("task1 after yielding cpu.\n\r");
            printf("task1: ret_val=%d\n\r", ret_val);
#ifdef DEBUG_0
            printf("task1: tid = %d, ret_val=%d\n\r", tid, ret_val);
#endif /* DEBUG_0 */
        }
    }
}

/**
 * @brief: a dummy task2
 */
void task2(void)
{
    task_t tid;
    RTX_TASK_INFO task_info;
    
    uart1_put_string("task2: entering \n\r");
    /* do something */
    tsk_create(&tid, &task2, LOW, 0x200);  /*create a user task */
    tsk_get(tid, &task_info);
    tsk_set_prio(tid, LOWEST);
    /* terminating */
    tsk_exit();
}

/**
 * @brief: a dummy task3
 */
void task3(void)
{
    uart1_put_string("task3: entering \n\r");
    /* do something */
    /* terminating */
    tsk_exit();
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
