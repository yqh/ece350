/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2022 Yiqing Huang
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
 * @file        ae_dummy_tasks_p1.c
 * @brief       dummy tasks for P1
 *
 * @version     V1.2022.05
 * @authors     Yiqing Huang
 * @date        2022 MAY
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

/**
 * @brief Testing on IRAM2
 */

#ifdef ECE350_P1

int test_mem1()
{
    return 0;
}

int test_mem2(void) {
    static void *p[4];
    
    for ( int i = 0; i < 4; i++ ) {
        p[i] = (void *)0x1;
    }

    U32 result = 0;

    p[0] = mem2_alloc(0x4000);

    if (p[0] != NULL) {
        result |= BIT(0);
    }

    p[1] = mem2_alloc(0x4000);

    if (p[1] != NULL && p[1] != p[0]) {
        result |= BIT(1);
    }

    p[2] = mem2_alloc(0x4000);
    if  (p[2] == NULL) {
        result |= BIT(2);
    }
    
    if (mem2_dump() == 0 ) {
        result |=BIT(3);
    }
    
    mem2_dealloc(p[0]);
    p[0] = NULL;
    if ( mem2_dump() == 1 ) {
        result |= BIT(4);
    }
    
    p[0] = mem2_alloc(0x2000);
    
    if ( mem2_dump() == 1 ) {
        result |= BIT(5);
    }
    
    p[2] = mem2_alloc(0x2000 - 2);
    
    if ( mem2_dump() == 0 ) {
        result |= BIT(6);
    }
    
    if (p[0] != p[2] ) {
        result |= BIT(7);
    }
    
    printf("END: 8 cases, result = 0x%x\r\n", result);
    return result;
}
#endif // ECE350_P1

/**************************************************************************//**
 * @brief       a task that prints AAAAA, BBBBB, CCCCC,...., ZZZZZ on each line.
 *              It yields the cpu every 6 lines are printed.
 *****************************************************************************/

#ifdef ECE350_P1
void priv_task1(void) {

    task_t tid = tsk_gettid();
    printf("priv_task1: TID =%d\r\n", tid);
    test_mem1();    // test on IRAM1 
    test_mem2();    // test on IRAM2
}
#elif ECE350_P2
void priv_task1(void)
{
    int i = 0;
    int j = 0;
    long int x = 0;
    
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
            int ret_val = 10;
            ret_val = tsk_yield();
            uart1_put_string("priv_task1 after yielding cpu.\n\r");
            printf("priv_task1: tid = %d, ret_val=%d\n\r", tid, ret_val);
        }
    }
}
#elif ECE350_P3
void priv_task1(void) {
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
#elif ECE350_P4 

void priv_task1(void) {
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
#else
void priv_task1(void) {
    while (1) {
        printf("priv_task1: executing\r\n");
        for ( int x = 0; x < DELAY; x++); // some artificial delay
    }
}
#endif


#ifdef ECE350_P2
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
            int ret_val = 10;
            ret_val = tsk_yield();
            uart1_put_string("task1 after yielding cpu.\n\r");
            printf("task1: tid = %d, ret_val=%d\n\r", tid, ret_val);
        }
    }
}
#elif ECE350_P3
/**************************************************************************//**
 * @brief:      a task that prints 00000, 11111, 22222,....,99999 on each line.
 *              It yields the cpu every 6 lines are printed
 *              before printing these lines indefinitely, it does the following:
 *              - creates a mailbox
 *              - registers %G command with KCD 
 *              - blocking receive 
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
    *buf = 'G';                             // set message data
    send_msg(TID_KCD, (void *)ptr);         // blocking send
    recv_msg(g_buf2, BUF_LEN);              // blocking recv
    
    // code to process received messages omitted
    
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
#elif ECE350_P4
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

#else
void task1(void) {
    while (1) {
        uart1_put_string("task1: executing\r\n");
        for ( int x = 0; x < DELAY; x++); // some artificial delay
    }
}
#endif


#ifdef ECE350_P2
/**
 * @brief: a dummy task2
 */
void task2(void)
{
    task_t tid;
    RTX_TASK_INFO task_info;
    
    uart1_put_string("task2: entering \r\n");
    /* do something */
    tsk_create(&tid, &task2, LOW, 0x200);  /*create a user task */
    tsk_get(tid, &task_info);
    tsk_set_prio(tid, LOWEST);
    /* terminating */
    tsk_exit();
}
#elif defined(ECE350_P3) || defined(ECE350_P4)
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
#else 
void task2(void)
{
    while (1) {
        uart1_put_string("task2: executing\r\n");
        for ( int x = 0; x < DELAY; x++); // some artificial delay
    }
}
#endif

/**
 * @brief: a dummy task3
 */
void task3(void)
{
    uart1_put_string("task3: entering \r\n");
    /* do something */
    /* terminating */
    tsk_exit();
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
