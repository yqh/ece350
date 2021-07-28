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
/* 
 * The following arrays can also be dynamic allocated to reduce ZI-data size
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
 *              This task does not have mailbox 
 *****************************************************************************/

void priv_task1(void)
{
    int i = 0;
    task_t tid = tsk_gettid();
    g_tasks[0] = tid;
    TIMEVAL tv; 
    
    tv.sec  = 1;
    tv.usec = 0;
    
    printf("priv_task1: TID =%d, setting its realtime properties...\r\n", tid);
    printf("period = %u sec, %u usec,\r\n", tv.sec, tv.usec);
    
    
    
    /*------------------------------------------------------------------------------
     * call this function after finishing initial real time task set up
     * this function elevates the task to a real-time task 
     *-----------------------------------------------------------------------------*/
    rt_tsk_set(&tv);  
    
    /*-------------------------------------------------------------------------------
     * Enter a loop to do periodic operations.
     * When a job is done, call the rt_tsk_susp() to wait its next periodic operation turn
     * The loop does not have to be an infinite loop, though in practice it usually is.
     * If you want to do finite number of periodic operations, 
     * then loop finite number of times and call tsk_exit() to terminate.
     * If you do not call tsk_exit, this will make the job taking over the cpu and
     * makes all other tasks starved. This is an application porgramming error if
     * you do not intend to create such a starvation. 
     *-------------------------------------------------------------------------------*/
    while (1) {
        char out_char = 'A' + i%26;
        for (int j = 0; j < 5; j++ ) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\n\r");
        for ( int x = 0; x < DELAY; x++); // some artifical delay 
 
        if ( (++i)%6 == 0 ) {
            uart1_put_string("priv_task1 before calling rt_tsk_susp.\n\r");
            rt_tsk_susp();      // wait till its next period
        }
    }
}

/**************************************************************************//**
 * @brief:  a periodic unpriviliged task that prints 
 *          00000, 11111, 22222,....,99999 on each line
 *          It prints six of these lines every 2 seconds
 *          It processes %G command
 *              
 *****************************************************************************/

void task1(void)
{

    int ret_val = 10;
    task_t tid = tsk_gettid();
    g_tasks[1] = tid;
    TIMEVAL tv;    
    
    tv.sec  = 2;
    tv.usec = 0;
    
    printf("task1: TID =%d\r\n", tid); 
    
    U8  *buf = &g_buf1[0];                  // buffer is allocated by the caller */
    struct rtx_msg_hdr *ptr = (void *)buf;
    ptr->length = MSG_HDR_SIZE + 1;         // set the message length
    ptr->type = KCD_REG;                    // set message type
    ptr->sender_tid = tid;                  // set sender id 
    buf += MSG_HDR_SIZE;                        
    *buf = 'G';                             // set message data
    send_msg(TID_KCD, (void *)ptr);         // blocking send

    // create two tasks;
    tsk_create(g_tasks+2, &task2, LOW, PROC_STACK_SIZE);
    tsk_create(g_tasks+2, &task3, LOW, PROC_STACK_SIZE);
    
    // create a maiilbox
    ret_val = mbx_create(BUF_LEN);
    if ( ret_val == RTX_ERR ) {
        printf("task1: failed to create a mailobx, terminating task...");
        tsk_exit();
    }
    
    /*------------------------------------------------------------------------------
    * call this function after finishing initial real time task set up
    * this function elevates the task to a real-time task 
    *-----------------------------------------------------------------------------*/
    rt_tsk_set(&tv); 
    
    for (int i = 0; i < 30 ;i++) {
        char out_char = '0' + i%10;
        for (int j = 0; j < 5; j++ ) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\n\r");
        for ( int x = 0; x < DELAY; x++); // some artifical delay 
        
        ret_val = recv_msg_nb(g_buf2, BUF_LEN);
        if ( ret_val == RTX_OK ) {
            struct rtx_msg_hdr *ptr = (void *)g_buf2;
            if ( ptr->type == KCD_CMD ) {
                char *p_cmd = (char *)(g_buf2 + MSG_HDR_SIZE);
                if ( *p_cmd == 'G' ) {
                    // do something about the data if there is any, maybe print out system information
                    printf("G received\r\n");
                    //dump_tasks(g_tasks, 2);
                    for ( int k = 0; k < 2; k++) {
                        rt_tsk_get(g_tasks[k], &tv );
                        printf("TID = %u, period = %u sec, %u usec\r\n", tv.sec, tv.usec);
                    }
                    // do the command processing here
                }
            }  // ignore other types of messages
        }  // do nothing if nothing received this period
        
        if ( i%6 == 0 ) {
            uart1_put_string("task1 before calling rt_tsk_susp.\n\r");
            rt_tsk_susp();      // wait till its next period
        }
    }
    tsk_exit(); // finished five perioidc operations, terminates 
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
    printf("task3: TID = %u, entering \n\r", tsk_gettid());
    while(1) {
        printf("task3: TID = %u, running \n\r", tsk_gettid());
        for ( int x = 0; x < DELAY; x++); // some artifical delay           
    }
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
