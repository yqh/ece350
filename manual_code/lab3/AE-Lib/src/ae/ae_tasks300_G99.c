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
 * @file        ae_tasks300.c
 * @brief       lab3 Test Suite 300  - Basic Non-blocking Message Passing
 *
 * @version     V1.2021.07
 * @authors     Yiqing Huang
 * @date        2021 Jul
 *
 * @note        Each task is in an infinite loop. These Tasks never terminate.
 *
 *****************************************************************************/

#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae_util.h"
#include "ae_tasks_util.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */
    
#define     NUM_TESTS       2       // number of tests
#define     NUM_INIT_TASKS  1       // number of tasks during initialization
#define     BUF_LEN         128     // receiver buffer length
#define     MY_MSG_TYPE     100     // some customized message type

/*
 *===========================================================================
 *                             GLOBAL VARIABLES 
 *===========================================================================
 */
const char   PREFIX[]      = "G99-TS300";
const char   PREFIX_LOG[]  = "G99-TS300-LOG";
const char   PREFIX_LOG2[] = "G99-TS300-LOG2";
TASK_INIT    g_init_tasks[NUM_INIT_TASKS];

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

/* The following arrays can also be dynamic allocated to reduce ZI-data size
 *  They do not have to be global buffers (provided the memory allocator has no bugs)
 */
 
U8 g_buf1[BUF_LEN];
U8 g_buf2[BUF_LEN];
task_t g_tasks[MAX_TASKS];
task_t g_tids[MAX_TASKS];

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
        tasks[i].priv = 0;
    }

    tasks[0].ptask = &task0;
    
    init_ae_tsk_test();
}

void init_ae_tsk_test(void)
{
    g_ae_xtest.test_id = 0;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests = NUM_TESTS;
    g_ae_xtest.num_tests_run = 0;
    
    for ( int i = 0; i< NUM_TESTS; i++ ) {
        g_tsk_cases[i].p_ae_case = &g_ae_cases[i];
        g_tsk_cases[i].p_ae_case->results  = 0x0;
        g_tsk_cases[i].p_ae_case->test_id  = i;
        g_tsk_cases[i].p_ae_case->num_bits = 0;
        g_tsk_cases[i].pos = 0;  // first avaiable slot to write exec seq tid
        // *_expt fields are case specific, deligate to specific test case to initialize
    }
    printf("%s: START\r\n", PREFIX);
}

void update_ae_xtest(int test_id)
{
    g_ae_xtest.test_id = test_id;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests_run++;
}

void gen_req0(int test_id)
{
    g_tsk_cases[test_id].p_ae_case->num_bits = 12;  
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 16; // assign a value no greater than MAX_LEN_SEQ
    g_tsk_cases[test_id].pos_expt = 9;
       
    update_ae_xtest(test_id);
}

void gen_req1(int test_id)
{
    //bits[0:3] pos check, bits[4:12] for exec order check
    g_tsk_cases[test_id].p_ae_case->num_bits = 13;  
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 0;       // N/A for this test
    g_tsk_cases[test_id].pos_expt = 0;  // N/A for this test
    
    update_ae_xtest(test_id);
}

int test0_start(int test_id)
{
    int ret_val = 10;
    
    gen_req0(test_id);
    //g_tids[0] = tsk_gettid();

    U8   *p_index    = &(g_ae_xtest.index);
    int  sub_result  = 0;
    
    //test 0-[0]
    *p_index = 0;
    strcpy(g_ae_xtest.msg, "task0: creating a HIGH prio task that runs task1 function");
    ret_val = tsk_create(&g_tids[1], &task1, HIGH, 0x200);  /*create a user task */
    sub_result = (ret_val == RTX_OK) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);    
    if ( ret_val != RTX_OK ) {
        sub_result = 0;
        test_exit();
    }
    
    //test 0-[1]
    (*p_index)++;
    strcpy(g_ae_xtest.msg, "task0: creating a HIGH prio task that runs task2 function");
    ret_val = tsk_create(&g_tids[2], &task2, HIGH, 0x200);  /*create a user task */
    sub_result = (ret_val == RTX_OK) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);
    if ( ret_val != RTX_OK ) {
        test_exit();
    }
    
    //test 0-[2]
    (*p_index)++;
    sprintf(g_ae_xtest.msg, "task0: creating a mailbox of size %u Bytes", BUF_LEN);
    mbx_t mbx_id = mbx_create(BUF_LEN);  // create a mailbox for itself
    sub_result = (mbx_id >= 0) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);
    if ( ret_val != RTX_OK ) {
        test_exit();
    }
    
    task_t  *p_seq_expt = g_tsk_cases[test_id].seq_expt;
    for ( int i = 0; i < 6; i += 3 ) {
        p_seq_expt[i]   = g_tids[0];
        p_seq_expt[i+1] = g_tids[1];
        p_seq_expt[i+2] = g_tids[2];
    }
    p_seq_expt[6] = g_tids[0];
    p_seq_expt[7] = g_tids[1];
    p_seq_expt[8] = g_tids[0];
    
    
    return RTX_OK;
}

/**
 * @brief   task yield exec order test
 * @param   test_id, the test function ID 
 * @param   ID of the test function that logs the testing data
 * @note    usually test data is logged by the same test function,
 *          but some time, we may have multiple tests to check the same test data
 *          logged by a particular test function
 */
void test1_start(int test_id, int test_id_data)
{  
    gen_req1(1);
    
    U8      pos         = g_tsk_cases[test_id_data].pos;
    U8      pos_expt    = g_tsk_cases[test_id_data].pos_expt;
    task_t  *p_seq      = g_tsk_cases[test_id_data].seq;
    task_t  *p_seq_expt = g_tsk_cases[test_id_data].seq_expt;
       
    U8      *p_index    = &(g_ae_xtest.index);
    int     sub_result  = 0;
    
    // output the real execution order
    printf("%s: real exec order: ", PREFIX_LOG);
    int pos_len = (pos > MAX_LEN_SEQ)? MAX_LEN_SEQ : pos;
    for ( int i = 0; i < pos_len; i++) {
        printf("%d -> ", p_seq[i]);
    }
    printf("NIL\r\n");
    
    // output the expected execution order
    printf("%s: expt exec order: ", PREFIX_LOG);
    for ( int i = 0; i < pos_expt; i++ ) {
        printf("%d -> ", p_seq_expt[i]);
    }
    printf("NIL\r\n");
    
    int diff = pos - pos_expt;
    
    // test 1-[0]
    *p_index = 0;
    strcpy(g_ae_xtest.msg, "checking execution shortfalls");
    sub_result = (diff < 0) ? 0 : 1;
    process_sub_result(test_id, *p_index, sub_result);
    
    //test 1-[1]
    (*p_index)++;
    strcpy(g_ae_xtest.msg, "checking unexpected execution once");
    sub_result = (diff == 1) ? 0 : 1;
    process_sub_result(test_id, *p_index, sub_result);
    
    //test 1-[2]
    (*p_index)++;
    strcpy(g_ae_xtest.msg, "checking unexpected execution twice");
    sub_result = (diff == 2) ? 0 : 1;
    process_sub_result(test_id, *p_index, sub_result);
    
    //test 1-[3]
    (*p_index)++;
    strcpy(g_ae_xtest.msg, "checking correct number of executions");
    sub_result = (diff == 0) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);
    
    //test 1-[4:12]
    for ( int i = 0; i < pos_expt; i ++ ) {
        (*p_index)++;
        sprintf(g_ae_xtest.msg, "checking execution sequence @ %d", i);
        sub_result = (p_seq[i] == p_seq_expt[i]) ? 1 : 0;
        process_sub_result(test_id, *p_index, sub_result);
    }
        
    
    test_exit();
}


int update_exec_seq(int test_id, task_t tid) 
{

    U8 len = g_tsk_cases[test_id].len;
    U8 *p_pos = &g_tsk_cases[test_id].pos;
    task_t *p_seq = g_tsk_cases[test_id].seq;
    p_seq[*p_pos] = tid;
    (*p_pos)++;
    (*p_pos) = (*p_pos)%len;  // preventing out of array bound
    return RTX_OK;
}

/**************************************************************************//**
 * @brief   The first task to run in the system
 *****************************************************************************/

void task0(void)
{
    int ret_val = 10;
    //mbx_t mbx_id = -1;
    task_t tid = tsk_gettid();
    g_tids[0] = tid;
    int     test_id    = 0;
    U8      *p_index   = &(g_ae_xtest.index);
    int     sub_result = 0;

    printf("%s: TID = %u, task0 entering\r\n", PREFIX_LOG2, tid);
    update_exec_seq(test_id, tid);
    
    ret_val = test0_start(test_id);
    
    if ( ret_val == RTX_OK ) {
        printf("%s: TID = %u, task0: calling tsk_yield()\r\n", PREFIX_LOG2, tid);     
        tsk_yield();    // let task2 run to create its mailbox
        update_exec_seq(test_id, tid);
        
        RTX_MSG_HDR *buf1 = mem_alloc(sizeof(RTX_MSG_HDR));   
        if ( buf1 == NULL ) {
            printf("task0, mem_alloc failed, terminating test\r\n");
            test_exit();
        }
        buf1->length = sizeof(RTX_MSG_HDR);
        buf1->type = MY_MSG_TYPE;
        buf1->sender_tid = tid;
        
        (*p_index)++;
        sprintf(g_ae_xtest.msg, "task0: send_msg_nb to tid(%u)", g_tids[2]);
        ret_val = send_msg_nb(g_tids[2], buf1);      // no-blocking send to task2, a mesage with no data field 
        sub_result = (ret_val == RTX_OK) ? 1 : 0;
        process_sub_result(test_id, *p_index, sub_result);    
        
        mem_dealloc(buf1);
    } 
    printf("%s: TID = %u, task0: calling tsk_yield()\r\n", PREFIX_LOG2, tid);    
    tsk_yield(); // let the other two tasks to run
    update_exec_seq(test_id, tid);
    
    printf("%s: TID = %u, task0: allocating recv buffer\r\n", PREFIX_LOG2, tid);
    char *buf = mem_alloc(BUF_LEN);
    if ( buf == NULL ) {
        printf("failed to allocating recv buffer, terminating test\r\n");
        test_exit();
    }
    
    while (recv_msg_nb(buf, BUF_LEN) != RTX_OK) {   // non-blocking receive    
        printf("%s:, TID = %u, task0: calling tsk_yield() after recv_msg_nb() \r\n", PREFIX_LOG2, tid);      
        tsk_yield();
        update_exec_seq(test_id, tid);
    }
    
    (*p_index)++;
    strcpy(g_ae_xtest.msg, "task0: checking received message length and type from task3");
    RTX_MSG_HDR *p_buf = (RTX_MSG_HDR *) buf;
    if ( p_buf->length == MSG_HDR_SIZE && p_buf->type == MY_MSG_TYPE  ) {
        sub_result = 1;
    } else {
        sub_result = 0;
    }
    process_sub_result(test_id, *p_index, sub_result);
    mem_dealloc(buf);
 
    printf("%s: TID = %u, task0: calling tsk_yield()\r\n", PREFIX_LOG2, tid);
    tsk_yield(); // let the other two tasks to run
    update_exec_seq(test_id, tid);
    test1_start(test_id + 1, test_id);
}

/**************************************************************************//**
 * @brief:  send a message to task2 and receives the message sent back from task2
 *****************************************************************************/

void task1(void)
{
    
    int ret_val = 10;
    mbx_t  mbx_id;
    task_t tid = tsk_gettid();
    int    test_id = 0;
    U8     *p_index    = &(g_ae_xtest.index);
    int    sub_result  = 0;
    
    size_t msg_hdr_size = sizeof(struct rtx_msg_hdr);
    U8  *buf = &g_buf1[0];                  // buffer is allocated by the caller */
    struct rtx_msg_hdr *ptr = (void *)buf;
   
    printf("%s: TID = %u, task1: entering\r\n", PREFIX_LOG2, tid);   
    update_exec_seq(test_id, tid);
    
    (*p_index)++;
    sprintf(g_ae_xtest.msg, "task1: creating a mailbox of size %u Bytes", BUF_LEN);
    mbx_id = mbx_create(BUF_LEN);  // create a mailbox for itself
    sub_result = (mbx_id >= 0) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);
    
    if ( mbx_id < 0 ) {     
        printf("%s: TID = %u, task2: failed to create a mailbox, terminating tests\r\n", PREFIX_LOG2, tid);
        test_exit();
    }
    
    ptr->length = msg_hdr_size + 1;         // set the message length
    ptr->type = DEFAULT;                    // set message type
    ptr->sender_tid = tid;                  // set sender id 
    buf += msg_hdr_size;                        
    *buf = 'A';                             // set message data
    
    while ( ret_val != RTX_OK ) {
        ret_val = send_msg_nb(g_tids[2], (void *)ptr);    // non-blocking send 
        printf("%s: TID = %d, task1: yielding cpu \r\n", PREFIX_LOG2, tid);    
        tsk_yield();
        update_exec_seq(test_id, tid);
    }
    
    while ( (ret_val = recv_msg_nb(g_buf2, BUF_LEN)) != RTX_OK ) {     
        printf("%s: TID = %d, task1: yielding cpu after calling recvv_msg_nb()\r\n", PREFIX_LOG2, tid);     
        tsk_yield();
        update_exec_seq(test_id, tid);
    }
    
    // check ret_val and then do something about the message, code omitted
    (*p_index)++;
    strcpy(g_ae_xtest.msg, "task1: test received message type and sender");
    RTX_MSG_HDR *p_buf = (RTX_MSG_HDR *) g_buf2;
    if ( p_buf->type == DEFAULT && p_buf->sender_tid == g_tids[2] ) {
        sub_result = 1;
    } else {
        sub_result = 0;
    }
    process_sub_result(test_id, *p_index, sub_result);
    
    (*p_index)++;
    strcpy(g_ae_xtest.msg, "task1: test received message length");
    sub_result = ( p_buf->length == 7) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);
    
    if (sub_result) {
        (*p_index)++;
        strcpy(g_ae_xtest.msg, "task1: test received message data");
        sub_result = (g_buf2[6] == 'A') ? 1 : 0;
        process_sub_result(test_id, *p_index, sub_result);
    }
    printf("%s: TID = %d, task1: yielding cpu after calling recvv_msg_nb()\r\n", PREFIX_LOG2, tid);
    tsk_yield();
    update_exec_seq(test_id, tid);
    test1_start(test_id + 1, test_id);
    
}

/**
 * @brief: a dummy task2
 */
void task2(void)
{
    int     ret_val;
    U8      *buf        = NULL;
    task_t  tid         = tsk_gettid();
    int     test_id     = 0;
    U8      *p_index    = &(g_ae_xtest.index);
    int     sub_result  = 0;
    
    printf("%s: TID = %u, task2: entering\r\n", PREFIX_LOG2, tid);  
    update_exec_seq(test_id, tid);
    
    buf = mem_alloc(BUF_LEN);
    if ( buf == NULL ) {
        printf ("%s, TID = %u, task2: failed to get memory\r\n", PREFIX_LOG2, tid);
        tsk_exit();
    }
    
    (*p_index)++;
    sprintf(g_ae_xtest.msg, "task2: creating a mailbox of size %u Bytes", BUF_LEN);
    ret_val = mbx_create(BUF_LEN);  // create a mailbox for itself
    sub_result = (ret_val >= 0) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);
     
    if ( sub_result ) {  // mbx created OK, try to receive two messages 
        int count = 0;
        while (count < 2) {
            printf("%s: TID = %u, task2: caling recv_msg_nb\r\n", PREFIX_LOG2, tid);
            ret_val = recv_msg_nb(buf, BUF_LEN);  // non-blocking receive
            if ( ret_val == RTX_OK )  {
                RTX_MSG_HDR *ptr = (RTX_MSG_HDR *)buf;
                task_t recv_tid = ptr->sender_tid;
                ptr->sender_tid = tid;
                (*p_index)++;
                strcpy(g_ae_xtest.msg, "task2: sending message back to sender");
                sub_result = (send_msg_nb(recv_tid, buf) == RTX_OK) ? 1 : 0;// send it back to sender
                process_sub_result(test_id, *p_index, sub_result);
                count++;
            } else {
                printf("%s: TID = %u, task2: calling tsk_yield\r\n", PREFIX_LOG2, tid);          
                tsk_yield();    // let task0 and task 1 to run and send a messaage
                update_exec_seq(test_id, tid);
            }
        }
    }   
    printf("%s: TID = %u, task2: calling mem_dealloc\r\n", PREFIX_LOG2, tid);
    mem_dealloc(buf);   // free the buffer space
    
    /********* ADDED fro SOLN, not in starter code*****************/
    //tsk_set_prio(g_tids[1], HIGH);
    
    printf("%s: TID = %u, task2: exiting...\r\n", PREFIX_LOG2, tid);
    tsk_exit();         // terminating the task
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
