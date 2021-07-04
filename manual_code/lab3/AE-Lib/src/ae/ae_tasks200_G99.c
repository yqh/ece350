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
 * @file        ae_tasks200.c
 * @brief       Lab2 public testing suite 200
 *
 * @version     V1.2021.06
 * @authors     Yiqing Huang
 * @date        2021 Jun
 * *
 *****************************************************************************/

#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae.h"
#include "ae_util.h"
#include "ae_tasks_util.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */
    
#define NUM_TESTS       2       // number of tests
#define NUM_INIT_TASKS  2       // number of tasks during initialization
/*
 *===========================================================================
 *                             GLOBAL VARIABLES 
 *===========================================================================
 */

TASK_INIT   g_init_tasks[NUM_INIT_TASKS];
const char   PREFIX[]      = "G99-TS200";
const char   PREFIX_LOG[]  = "G99-TS200-LOG";
const char   PREFIX_LOG2[] = "G99-TS200-LOG2";

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num)
{
    *p_num = NUM_INIT_TASKS;
    *pp_tasks = g_init_tasks;
    set_ae_tasks(*pp_tasks, *p_num);
}

// initial task configuration
void set_ae_tasks(TASK_INIT *tasks, int num)
{
    for (int i = 0; i < num; i++ ) {                                                 
        tasks[i].u_stack_size = PROC_STACK_SIZE;    
        tasks[i].prio = HIGH + i;
        tasks[i].priv = 1;
    }
    tasks[0].priv  = 1;
    tasks[0].ptask = &priv_task1;
    tasks[1].priv  = 0;
    tasks[1].ptask = &task1;
    
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
    // bits[0:1] for two tsk_create, [2:5] for 4 tsk_yield return value checks
    g_tsk_cases[test_id].p_ae_case->num_bits = 6;  
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 16; // assign a value no greater than MAX_LEN_SEQ
    g_tsk_cases[test_id].pos_expt = 6;
       
    g_ae_xtest.test_id = test_id;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests_run++;
}

void gen_req1(int test_id)
{
    //bits[0:3] pos check, bits[4:9] for exec order check
    g_tsk_cases[test_id].p_ae_case->num_bits = 10;  
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 0;       // N/A for this test
    g_tsk_cases[test_id].pos_expt = 0;  // N/A for this test
    
    update_ae_xtest(test_id);
}

int test0_start(int test_id)
{
    int ret_val = 10;
    task_t tid1;
    task_t tid2;
    
    gen_req0(test_id);
    
    U8      *p_index    = &(g_ae_xtest.index);
    int     sub_result  = 0;
    
    //test 0-[0]
    *p_index = 0;
    strcpy(g_ae_xtest.msg, "creating a HIGH prio task that runs task2 function");
    ret_val = tsk_create(&tid1, &task2, HIGH, 0x200);  /*create a user task */
    sub_result = (ret_val == RTX_OK) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);    
    if ( ret_val != RTX_OK ) {
        sub_result = 0;
        test_exit();
    }
    
    //test 0-[1]
    (*p_index)++;
    strcpy(g_ae_xtest.msg, "creating a HIGH prio task that runs task3 function");
    ret_val = tsk_create(&tid2, &task3, HIGH, 0x200);  /*create a user task */
    sub_result = (ret_val == RTX_OK) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);
    if ( ret_val != RTX_OK ) {
        test_exit();
    }

    task_t *p_seq_expt = g_tsk_cases[test_id].seq_expt;
    for ( int i = 0; i < 6; i += 3 ) {
        p_seq_expt[i]   = tsk_gettid();;
        p_seq_expt[i+1] = tid1;
        p_seq_expt[i+2] = tid2;
    }
    
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
    
    //test 1-[4:9]
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
 * @brief       a task that prints AAAAA, BBBBB on each line.
 *              it yields the cpu after each line
 *****************************************************************************/

void priv_task1(void)
{
    task_t tid = tsk_gettid();
    int test_id = 0;
    U8      *p_index    = &(g_ae_xtest.index);
    int     sub_result  = 0;
    
    printf("%s: TID = %d, priv_task1: starting test0\r\n", PREFIX_LOG2, tid);
    strcpy(g_ae_xtest.msg, "Four same priority tasks yielding cpu test");
    test0_start(test_id);
    
    for ( int i = 0; i < 2; i++) {
        char out_char = 'A' + i%26;
        for ( int j = 0; j < 5; j++) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\r\n");       
        printf("%s: TID = %d, priv_task1: yielding cpu\r\n", PREFIX_LOG2, tid );
        update_exec_seq(test_id, tid);
        int ret = tsk_yield();
        // 0-[2:3]
        sprintf(g_ae_xtest.msg, "priv_task1 calling tsk_yield - %d", i+1);
        sub_result = ( ret == RTX_OK ) ? 1 : 0;
        (*p_index)++;
        process_sub_result(test_id, *p_index, sub_result);
    }
    tsk_yield();
    tsk_yield();

    test1_start(test_id + 1, test_id);
}

/**************************************************************************//**
 * @brief:      a task that prints 00000, 11111 and 22222 on each line.
 *              It yields the cpu every after each line is printed.
 *****************************************************************************/

void task1(void)
{
    int test_id = 0;
    U8      *p_index    = &(g_ae_xtest.index);
    int     sub_result  = 0;
    
    task_t tid = tsk_gettid();
    printf("%s: TID = %d, task1: should not run! Exiting... \r\n", PREFIX_LOG2, tid);
    
    test1_start(1, 0);
    
    for ( int i = 0; i < 2; i++) {
        char out_char = '1' + i%26;
        for ( int j = 0; j < 5; j++) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\r\n");
        printf("%s: TID = %d, task1: yielding cpu\r\n", PREFIX_LOG2, tid);
        update_exec_seq(test_id, tid);
        int ret = tsk_yield();
        sprintf(g_ae_xtest.msg, "task1 calling tsk_yield - %d", i+1);
        sub_result = ( ret == RTX_OK ) ? 1 : 0;
        (*p_index)++;
        process_sub_result(test_id, *p_index, sub_result);
    }
    
    tsk_set_prio(tid, MEDIUM);
    printf("%s: TID = %d, task1: I should not run \r\n", PREFIX_LOG2, tid);
    update_exec_seq(test_id, tid);

    test1_start(test_id + 1, test_id);
}

/**
 * @brief: a dummy task2
 */
void task2(void)
{
    task_t tid = tsk_gettid();
    int    test_id = 0;
    U8     *p_index    = &(g_ae_xtest.index);
    int    sub_result  = 0;
    
    printf("%s: TID = %d, task2: entering\r\n", PREFIX_LOG2, tid);
    for ( int i = 0; i < 2; i++) {
        printf("%s: TID = %d, task2: yielding cpu \r\n", PREFIX_LOG2, tid);
        update_exec_seq(test_id, tid);
        tsk_yield();
        int ret = tsk_yield();
        // 0-[4]
        sprintf(g_ae_xtest.msg, "task2 calling tsk_yield - %d", i+1);
        sub_result = ( ret == RTX_OK ) ? 1 : 0;
        (*p_index)++;
        process_sub_result(test_id, *p_index, sub_result);
    }
    tsk_set_prio(tid, LOW);
    printf("%s: TID = %d, task2: I should not run \r\n", PREFIX_LOG2, tid);
    update_exec_seq(test_id, tid);
    tsk_yield();

    test1_start(test_id + 1, test_id);
}

/**
 * @brief: a dummy task3
 */
void task3(void)
{
    int    test_id = 0;
    task_t tid = tsk_gettid();
    U8     *p_index    = &(g_ae_xtest.index);
    int    sub_result  = 0;
    
    printf("%s: TID = %d, task3: entering \r\n", PREFIX_LOG2, tid);
    
    for ( int i = 0; i < 2; i++) {
        printf("%s: TID = %d, task3: yielding cpu \r\n", PREFIX_LOG2, tid);
        update_exec_seq(test_id, tid);
        tsk_yield();
        int ret = tsk_yield();
        // 0-[5]
        sprintf(g_ae_xtest.msg, "task3 calling tsk_yield - %d", i+1);
        sub_result = ( ret == RTX_OK ) ? 1 : 0;
        (*p_index)++;
        process_sub_result(test_id, *p_index, sub_result);
    }
    tsk_set_prio(tid, LOWEST);
    printf("%s: TID = %d, task3: I should not run \r\n", PREFIX_LOG2, tid);
    update_exec_seq(test_id, tid);
    tsk_yield();

    test1_start(test_id + 1, test_id);
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
