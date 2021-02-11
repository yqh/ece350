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

// We should print TCB array inside every function for testing?

//------KERNEL TASKS------//
// k_tsk_create
// k_tsk_get

// k_tsk_create until max_tasks (same priority)
// k_tsk_get for all TIDs
// k_tsk create > max_tasks should fail
// k_tsk_exit 
// k_tsk_create should reuse TID

// k_tsk_create
// tsk_create
// k_tsk_get
// k_tsk_set_prio for kernel task
// k_tsk_set_prio for user task
// k_tsk_get
// k_tsk_set_prio PRIO_NULL should fail
// k_tsk_set_prio invalid TID should fail
// k_tsk_set_prio dormant task should fail

U32 result = 1;
U32 numtests = 0;

void printResult(int passFail){
    numtests++;
    if(passFail == 1){
        SER_PutStr ("--- Success ---\n\r");
//        result << 1;
//        result += 1;
    } else {
        SER_PutStr ("--- !!! FAILURE !!! ---\n\r");
//        result << 1;
    }
}

void priv_task_entry(void){

    RTX_TASK_INFO task_info;
    task_t tid;

    SER_PutStr (">>>>>>>>>>>>>>>>>>>>> 1. tsk_create Failure Test:\n\r");
    SER_PutStr ("Stack Size Too Small:\n\r");
    printResult(k_tsk_create(&tid, &dumdum, LOW, 0x100) == RTX_ERR);
    SER_PutStr ("Stack Size Not 8b Aligned:\n\r");
    printResult(k_tsk_create(&tid, &dumdum, LOW, 0x401) == RTX_ERR);
    // SER_PutStr ("Stack Size Too Big:\n\r");
    // printResult(k_tsk_create(&tid, &dumdum, LOW, 0xFFFFFF00) == RTX_ERR);	 // U16 stack_size
    SER_PutStr ("Prio Invalid:\n\r");
    printResult(k_tsk_create(&tid, &dumdum, 99, 0x400) == RTX_ERR);
    SER_PutStr ("Prio Null:\n\r");
    printResult(k_tsk_create(&tid, &dumdum, PRIO_NULL, 0x400) == RTX_ERR);
    SER_PutStr ("Null tid:\n\r");
    printResult(k_tsk_create(NULL, &dumdum, LOW, 0x400) == RTX_ERR);
    SER_PutStr ("Null task:\n\r");
    printResult(k_tsk_create(&tid, NULL, LOW, 0x400) == RTX_ERR);

    if (result == 7){
    	printf(">>>>>>>>>> TESTS PASSED <<<<<<<<<<<< \n\r");
    }else{
    	printf(">>>>>>>>>> TESTS FAILED:  %d <<<<<<<<<<<< \n\r", 7 - result);
    }

    result = 1;

    SER_PutStr (">>>>>>>>>>>>>>>>>>>>> 2. tsk_create and tsk_get Function Test:\n\r");
    k_tsk_create(&tid, &dumdum, LOW, 0x400);
    SER_PutStr ("Did we get tid 2:\n\r");
    printResult(tid == 2);          // 0 = NULL_TASK, 1 = Priv Task, 2 = dumdum
    k_tsk_get(tid, &task_info);

    SER_PutStr ("Compare kStack Size:\n\r");
    printResult(task_info.k_stack_size == KERN_STACK_SIZE); // 0x200
    SER_PutStr ("Compare uStack Size:\n\r");
    printResult(task_info.u_stack_size == 0x400);
    SER_PutStr ("Compare Task ID:\n\r");
    printResult(task_info.tid == tid);
    SER_PutStr ("Compare Priority:\n\r");
    printResult(task_info.prio == LOW);
    SER_PutStr ("Compare State:\n\r");
    printResult(task_info.state == READY);
    SER_PutStr ("Compare Privilege:\n\r");
    printResult(task_info.priv == 0);
    
    if (result == 8){
    	printf(">>>>>>>>>> TESTS PASSED <<<<<<<<<<<< \n\r");
    }else{
    	printf(">>>>>>>>>> TESTS FAILED:  %d <<<<<<<<<<<< \n\r", 8 - result);
    }

    result = 1;

    SER_PutStr (">>>>>>>>>>>>>>>>>>>>> 3. tsk_get Failure Test:\n\r");
    SER_PutStr ("Task ID 0:\n\r");
    printResult(k_tsk_get(0, &task_info) == RTX_ERR);
    SER_PutStr ("Null Buffer Task Info:\n\r");
    printResult(k_tsk_get(tid, NULL) == RTX_ERR);
    SER_PutStr ("Max Task ID:\n\r");
    printResult(k_tsk_get(MAX_TASKS, &task_info) == RTX_ERR);
    SER_PutStr ("Dormant Task 3:\n\r");
    printResult(k_tsk_get(3, &task_info) == RTX_ERR);
    SER_PutStr ("Dormant Task Mid:\n\r");
    printResult(k_tsk_get(MAX_TASKS/2, &task_info) == RTX_ERR);

    if (result == 6){
    	printf(">>>>>>>>>> TESTS PASSED <<<<<<<<<<<< \n\r");
    }else{
    	printf(">>>>>>>>>> TESTS FAILED:  %d <<<<<<<<<<<< \n\r", 6 - result);
    }

    result = 1;

    SER_PutStr (">>>>>>>>>>>>>>>>>>>>> 4. set_prio Function Test:\n\r");
    SER_PutStr ("Set user prio:\n\r");
    k_tsk_set_prio(tid, LOWEST);
    k_tsk_get(tid, &task_info);
    printResult(task_info.prio == LOWEST);
    SER_PutStr ("Set own prio:\n\r");          // Should be setting prio from med to hi
    k_tsk_set_prio(1, HIGH);
    k_tsk_get(1, &task_info);
    printResult(task_info.prio == HIGH);

    if (result == 3){
    	printf(">>>>>>>>>> TESTS PASSED <<<<<<<<<<<< \n\r");
    }else{
    	printf(">>>>>>>>>> TESTS FAILED:  %d <<<<<<<<<<<< \n\r", 3 - result);
    }

    result = 1;

    SER_PutStr (">>>>>>>>>>>>>>>>>>>>> 5. set_prio Failure Test:\n\r");
    SER_PutStr ("Task ID 0:\n\r");
    printResult(k_tsk_set_prio(0, HIGH) == RTX_ERR);
    SER_PutStr ("Null_Task Prio:\n\r");
    printResult(k_tsk_set_prio(tid, PRIO_NULL) == RTX_ERR);
    SER_PutStr ("Max Task ID:\n\r");
    printResult(k_tsk_set_prio(MAX_TASKS, HIGH) == RTX_ERR);
    SER_PutStr ("Dormant Task 3:\n\r");
    printResult(k_tsk_set_prio(3, HIGH) == RTX_ERR);
    SER_PutStr ("Dormant Task Mid:\n\r");
    printResult(k_tsk_set_prio(MAX_TASKS/2, HIGH) == RTX_ERR);
    SER_PutStr ("Undefined Priority:\n\r");
    printResult(k_tsk_set_prio(tid, 99) == RTX_ERR);
    // SER_PutStr ("Privilege Check:\n\r"); // THIS IS WILL PASS FOR KERNEL TASK
    // printResult(k_tsk_set_prio(1, HIGH) == RTX_ERR);

    if (result == 7){
    	printf(">>>>>>>>>> TESTS PASSED <<<<<<<<<<<< \n\r");
    }else{
    	printf(">>>>>>>>>> TESTS FAILED:  %d <<<<<<<<<<<< \n\r", 7 - result);
    }

    result = 1;

    SER_PutStr (">>>>>>>>>>>>>>>>>>>>> 1. tsk_create Stress Function Test:\n\r");

    // Testing max_tasks
    for(int i = 3; i < MAX_TASKS; i++){             // 0 is Null Task, 1 is this kernel task
        k_tsk_create(&tid, &dumdum, LOW, 0x200);
    }

    SER_PutStr ("Creating Another Task Fails:\n\r");
    printResult(k_tsk_create(&tid, &dumdum, LOW, 0x400) == RTX_ERR);

    if (result == 2){
    	printf(">>>>>>>>>> TESTS PASSED <<<<<<<<<<<< \n\r");
    }else{
    	printf(">>>>>>>>>> TESTS FAILED:  %d <<<<<<<<<<<< \n\r", 2 - result);
    }

    k_tsk_set_prio(gp_current_task->tid, LOWEST);
    k_tsk_yield();

    // Testing reusability
    k_tsk_create(&tid, &dumdum, LOW, 0x400);
    SER_PutStr ("Task id after full yield is 1\n\r");
    printResult(tid == 2);
    k_tsk_yield();
    k_tsk_create(&tid, &dumdum, LOW, 0x400);
    SER_PutStr ("Task id after full yield is 1\n\r");
    printResult(tid == 2);

    k_tsk_exit();
}

void reset_scheduling() {
	for (int i = 0; i < 255; i++) {
		s_buffer[i] = 0;
	}
}

// Function to implement strcmp function
int strcmp(const char *X, const char *Y)
{
    while(*X)
    {
        // if characters differ or end of second string is reached
        if (*X != *Y)
            break;

        // move to next pair of characters
        X++;
        Y++;
    }

    // return the ASCII difference after converting char* to unsigned char*
    return *(const unsigned char*)X - *(const unsigned char*)Y;
}

void priv_tasks_scheduling(void) {
	/* Test 1:
	 * Create:
	 * 3 tasks with high priority,
	 * 3 tasks with medium priority,
	 * 3 tasks with low priority,
	 * 3 tasks with lowest priority
	 * Expect High 1 high 2 high 3 med 1 med 2 med 3 etc.
	 *
	 * Then:
	 * Create in order of h1, m1, l1, lw1 etc.
	 *
	 * Then:
	 * Create in order of lw1, l1, m1, h1 etc.
	 *
	 * Then:
	 * Create in order of lw3, m2, l1, h3, etc.
	 *
	 */

    task_t tid[10];

    reset_scheduling();

    k_tsk_create(&tid[0], &stask0, HIGH, 10000);
    k_tsk_create(&tid[1], &stask1, HIGH, 10000);
    k_tsk_create(&tid[2], &stask2, HIGH, 10000);
    k_tsk_create(&tid[3], &stask3, MEDIUM, 10000);
    k_tsk_create(&tid[4], &stask4, MEDIUM, 10000);
    k_tsk_create(&tid[5], &stask5, MEDIUM, 10000);
    k_tsk_create(&tid[6], &stask6, LOW, 10000);
    k_tsk_create(&tid[7], &stask7, LOW, 10000);
    k_tsk_create(&tid[8], &stask8, LOWEST, 10000);
    k_tsk_create(& tid[9], &stask9, LOWEST, 10000);

    k_tsk_set_prio(gp_current_task->tid, LOWEST);

    k_tsk_yield();

    SER_PutStr("Scheduler output after yielding cpu.\n\r");
    printResult(strcmp(s_buffer, "0123456789") == 0);

    reset_scheduling();

    k_tsk_create(&tid[0], &stask7, HIGH, 10000);
    k_tsk_create(&tid[3], &stask2, MEDIUM, 10000);
    k_tsk_create(&tid[6], &stask9, LOW, 10000);
    k_tsk_create(&tid[7], &stask1, LOW, 10000);
    k_tsk_create(&tid[8], &stask0, LOWEST, 10000);
    k_tsk_create(&tid[4], &stask4, MEDIUM, 10000);
    k_tsk_create(&tid[5], &stask5, MEDIUM, 10000);
    k_tsk_create(&tid[1], &stask3, HIGH, 10000);
    k_tsk_create(&tid[2], &stask6, HIGH, 10000);
    k_tsk_create(&tid[8], &stask8, LOWEST, 10000);

    k_tsk_set_prio(gp_current_task->tid, LOWEST);

    k_tsk_yield();

    SER_PutStr("Scheduler output after yielding cpu.\n\r");
    printResult(strcmp(s_buffer, "7362459108") == 0);

    reset_scheduling();

    k_tsk_create(&tid[0], &stask0, HIGH, 10000);
    k_tsk_create(&tid[1], &stask1, HIGH, 10000);
    k_tsk_create(&tid[2], &stask2, HIGH, 10000);
    k_tsk_create(&tid[3], &stask3, MEDIUM, 10000);
    k_tsk_create(&tid[4], &stask4, MEDIUM, 10000);
    k_tsk_create(&tid[5], &stask5, MEDIUM, 10000);
    k_tsk_create(&tid[6], &stask6, LOW, 10000);
    k_tsk_create(&tid[7], &stask7, LOW, 10000);
    k_tsk_create(&tid[8], &stask8, LOWEST, 10000);
    k_tsk_create(& tid[9], &stask9, LOWEST, 10000);

    k_tsk_set_prio(tid[0], LOWEST);
    k_tsk_set_prio(tid[1], LOWEST);
    k_tsk_set_prio(tid[2], LOWEST);
    k_tsk_set_prio(tid[1], MEDIUM);


    k_tsk_yield();

    SER_PutStr("Scheduler output after yielding cpu.\n\r");
    printResult(strcmp(s_buffer, "3451678902") == 0);

    while(1);

    k_tsk_exit();
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

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
