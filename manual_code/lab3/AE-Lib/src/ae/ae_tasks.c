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
#include "ae_util.h"
#include "ae_tasks_util.h"

#define     BUF_LEN         128
#define     MY_MSG_TYPE     100     // some customized message type, better move it to common_ext.h
#define     NUM_INIT_TASKS  2       // number of tasks during initialization

TASK_INIT   g_init_tasks[NUM_INIT_TASKS];
/* The following arrays can also be dynamic allocated to reduce ZI-data size
 *  They do not have to be global buffers (provided the memory allocator has no bugs)
 */
 
U8 g_buf1[BUF_LEN];
U8 g_buf2[BUF_LEN];
task_t g_tasks[MAX_TASKS];

void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num)
{
    *p_num = NUM_INIT_TASKS;
    *pp_tasks = g_init_tasks;
    set_ae_tasks(*pp_tasks, *p_num);
}

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
    mbx_t mbx_id = -1;
    task_t tid = tsk_gettid();
    task_t tid1; 
    char *buf = NULL;           // this is a user dynamically allocated buffer
    RTX_MSG_HDR msg_hdr;
    
    printf("sizeof(RTX_MSG_HDR) = %u \r\n", sizeof(RTX_MSG_HDR));
    printf("sizeof(struct rtx_msg_hdr) = %u \r\n", sizeof(struct rtx_msg_hdr));
    printf("&msg_hdr = 0x%x, &(msg_hdr.sender_tid) = 0x%x, &(msg_hdr.type) = 0x%x\r\n",\
            &msg_hdr, &(msg_hdr.sender_tid), &(msg_hdr.type));
    
    printf("priv_task1: TID =%d\r\n", tid);
    
    buf = mem_alloc(BUF_LEN);
    mbx_id = mbx_create(BUF_LEN);  // create a mailbox for itself
    
    if ( mbx_id >= 0 ) {
        dump_mbx_info(tid);
    }
    
      
    ret_val = tsk_ls(g_tasks, MAX_TASKS);
    if (ret_val) {
        dump_tasks(g_tasks, ret_val);
    }
    ret_val = mbx_ls(g_tasks, MAX_TASKS);
    if (ret_val)
    {
        dump_mailboxes(g_tasks, ret_val);
    }
    
    
    if ( mbx_id >= 0 && buf != NULL ) {
        ret_val = recv_msg_nb(&g_buf1, BUF_LEN);  // non-blocking receive
        // check ret_val and then do something about the message, code omitted
        mem_dealloc(buf);
    }
    
    ret_val = tsk_create(&tid1, task2, HIGH, PROC_STACK_SIZE);
    
    tsk_set_prio(tid, MEDIUM);
    
    if ( ret_val == RTX_OK ) {
        RTX_MSG_HDR *buf1 = mem_alloc(sizeof(RTX_MSG_HDR));   
        buf1->length = sizeof(RTX_MSG_HDR);
        buf1->type = MY_MSG_TYPE;
        buf1->sender_tid = tid;
        ret_val = send_msg_nb(tid1, buf1);      // no-blocking send a mesage with no data field
    }
    
    
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
    task_t tid = tsk_gettid();;
    
    size_t msg_hdr_size = sizeof(struct rtx_msg_hdr);
    U8  *buf = &g_buf1[0];                  // buffer is allocated by the caller */
    struct rtx_msg_hdr *ptr = (void *)buf;
    
    mbx_create(BUF_LEN);                    // create a mailbox for itself
    ptr->length = msg_hdr_size + 1;         // set the message length
    ptr->type = KCD_REG;                    // set message type
    ptr->sender_tid = tid;                  // set sender id 
    buf += msg_hdr_size;                        
    *buf = 'W';                             // set message data
    send_msg(TID_KCD, (void *)ptr);         // blocking send
    recv_msg(g_buf2, BUF_LEN); // blocking recv
    
    buf = &g_buf1[0];

    int *ptr1 = mem_alloc(sizeof(int));
    printf("ptr = 0x%x\r\n", ptr1); 
    mem_dealloc(ptr1);
    mem_dump();

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
    int ret_val;
    U8 *buf = mem_alloc(BUF_LEN);
    
    uart1_put_string("task2: entering \n\r");
    
    ret_val = mbx_create(BUF_LEN);
    if ( ret_val == RTX_OK ) {
        ret_val = recv_msg(buf, BUF_LEN);  // blocking receive    
    }
    mem_dealloc(buf);   // free the buffer space
    
    tsk_exit();         // terminating the task
}

/**
 * @brief: a dummy task3
 */
void task3(void)
{
    
    
    uart1_put_string("task3: entering \n\r");
    
    /* do something */
    
    /* Terminate if you are not a daemon task.
       For a deamon task, it should be in an infinite loop and never terminate.
    */
    
    
    tsk_exit();
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
