/**
 * @file:   priv_tasks.c
 * @brief:  Two kernel tasks: priv_task1 and priv_task2
 * @author: Yiqing Huang
 * @date:   2020/09/24
 * NOTE: Each task is in an infinite loop. These Tasks never terminate.
 */

#include "uart_polling.h"
#include "priv_tasks.h"
#include "usr_tasks.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/**
 * @brief: fill the tasks array with information 
 * @param: tasks, an array of RTX_TASK_INFO elements
 */
void set_task_info(RTX_TASK_INFO *tasks, int num_tasks) {
    for (int i = 0; i < num_tasks; i++ ) {
        tasks[i].u_stack_size = 0x0;
        tasks[i].prio = HIGH;
        tasks[i].priv = 1;
    }
    tasks[0].ptask = &priv_task1;
    tasks[1].ptask = &priv_task2;
}

/**
 * @brief: a task that prints AAAAA, BBBBB, CCCCC,...., ZZZZZ on each line. 
 *         It yields the cpu every 6 lines are printed.
 */
void priv_task1(void)
{
    int i = 0;
    long int x = 0;
    int ret_val = 10;
    
    while (1) {
        char out_char = 'A' + i%26;
        for (int j = 0; j < 5; j++ ) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\n\r");
        
        for ( x = 0; x < DELAY; x++); // some artifical delay
        if ( (++i)%6 == 0 ) {
            uart1_put_string("priv_task1 before yielding cpu.\n\r");
            ret_val = tsk_yield();
            uart1_put_string("priv_task1 after yielding cpu.\n\r");
#ifdef DEBUG_0
            printf("priv_task1: ret_val=%d\n\r", ret_val);
#endif /* DEBUG_0 */
        }
    }
}

/**
 * @brief: a task that prints 00000, 11111, 22222,....,99999 on each line. 
 *         It yields the cpu every 6 lines are printed
 *         before printing these lines indefinately, it creates a
 *         new task and and obtains the task information. It then
 *         changes the newly created task's priority.
 */
void priv_task2(void)
{
    long int x = 0;
    int ret_val = 10;
    RTX_TASK_INFO task_info;
    task_t tid;
    
    tsk_create(&tid, &task1, LOW, 0x200);  /*create a user task */
    tsk_get(tid, &task_info);
    tsk_set_prio(tid, LOWEST);
    
    for (int i=1;;i++) {
        char out_char = '0' + i%10;
        for (int j = 0; j < 5; j++ ) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\n\r");
        
        for ( x = 0; x < DELAY; x++); // some artifical delay
        if ( i%6 == 0 ) {
            uart1_put_string("priv_task2 before yielding cpu.\n\r");
            ret_val = tsk_yield();
            uart1_put_string("priv_task2 after yielding cpu.\n\r");
#ifdef DEBUG_0
            printf("priv_task2: ret_val=%d\n\r", ret_val);
#endif /* DEBUG_0 */
        }
    }
}
