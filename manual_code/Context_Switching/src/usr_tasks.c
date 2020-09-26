/**
 * @file:   usr_tasks.c
 * @brief:  Two user/unprivileged  tasks: task1 and task2
 * @author: Yiqing Huang
 * @date:   2020/09/20
 * NOTE: Each task is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_tasks.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/**
 * @brief: a dummy task1
 */
void task1(void)
{
    task_t tid;
    RTX_TASK_INFO task_info;
    
    uart1_put_string ("task1: entering \n\r");
    /* do something */
    tsk_create(&tid, &task2, LOW, 0x200);  /*create a user task */
    tsk_get(tid, &task_info);
    tsk_set_prio(tid, LOWEST);
    /* terminating */
    tsk_exit();
}

/**
 * @brief: a dummy task2
 */
void task2(void)
{
    uart1_put_string ("task2: entering \n\r");
    /* do something */
    /* terminating */
    tsk_exit();
}
