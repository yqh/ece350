/** 
 * @file:   k_rtx_init.c
 * @brief:  Kernel initialization C file
 * @auther: Yiqing Huang
 * @date:   2020/09/20
 */

#include "k_rtx_init.h"
#include "uart_polling.h"
#include "k_mem.h"
#include "k_task.h"

void k_rtx_init(size_t blk_size, int algo, RTX_TASK_INFO *task_info, int num_tasks)
{
    /* interrupts are already disabled when we enter here */
    k_mem_init(blk_size, algo);
    k_tsk_init(task_info, num_tasks);
    
    /* start the first task */
    k_tsk_yield();
}
