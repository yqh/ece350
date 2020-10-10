/**
 * @file:   usr_tasks.c
 * @brief:  Two user/unprivileged  tasks: task1 and task2
 * @author: Yiqing Huang
 * @date:   2020/10/09
 * NOTE: Each task is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_tasks.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* the following arrays can also be dynamic allocated 
   They do not have to be global buffers. 
 */
U8 g_buf1[256];
U8 g_buf2[256];
task_t g_tasks[MAX_TASKS];

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
    mbx_create(256);
    tsk_ls(g_tasks, MAX_TASKS);
    mbx_ls(g_tasks, MAX_TASKS);
    /* terminating */
    tsk_exit();
}

/**
 * @brief: a dummy task2
 */
void task2(void)
{
    size_t msg_hdr_size = sizeof(struct rtx_msg_hdr);
    U8  *buf = &g_buf1[0]; /* buffer is allocated by the caller */
    struct rtx_msg_hdr *ptr = (void *)buf;
    task_t sender_tid = 0;
    
    uart1_put_string ("task2: entering \n\r");
    
    mbx_create(128); 
    ptr->length = msg_hdr_size + 1;
    ptr->type = KCD_REG;
    buf += msg_hdr_size;
    *buf = 'W';
    send_msg(TID_KCD, (void *)buf);
    recv_msg(&sender_tid, g_buf2, 128);
    
    /* terminating */
    tsk_exit();
}



