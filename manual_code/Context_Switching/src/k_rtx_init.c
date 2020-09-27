/** 
 * @file:   k_rtx_init.c
 * @brief:  Kernel initialization C file
 * @auther: Yiqing Huang
 * @date:   2020/09/26
 */

#include "k_rtx_init.h"
#include "uart_polling.h"
#include "k_mem.h"
#include "k_task.h"

int k_rtx_init(size_t blk_size, int algo, RTX_TASK_INFO *task_info, int num_tasks)
{
    /* interrupts are already disabled when we enter here */
    if ( k_mem_init(blk_size, algo) != RTX_OK) {
        return RTX_ERR;
    }

    if ( k_tsk_init(task_info, num_tasks) != RTX_OK ) {
        return RTX_ERR;
    }
    
    /* start the first task */
    return k_tsk_yield();
}
