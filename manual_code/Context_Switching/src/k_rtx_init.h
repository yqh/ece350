/** 
 * @file:   k_rtx_init.h
 * @brief:  Kernel initialization header file
 * @auther: Yiqing Huang
 * @date:   2014/01/17
 */
 
#ifndef K_RTX_INIT_H_
#define K_RTX_INIT_H_

#include "k_rtx.h"

/* Functions */

void k_rtx_init(size_t blk_size, int alog, RTX_TASK_INFO *task_info, int num_tasks);
#endif /* ! K_RTX_INIT_H_ */
