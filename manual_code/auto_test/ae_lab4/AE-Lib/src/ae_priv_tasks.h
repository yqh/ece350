/**
 * @file:   ae_priv_tasks.h
 * @brief:  One test manager kernel task header file
 * @author: Yiqing Huang
 * @date:   2020/11/18
 */
 
#ifndef _AE_PRIV_TASKS_H_
#define _AE_PRIV_TASKS_H_

#include "rtx.h"

#ifdef SIM_TARGET       /* using the simulator is slow */
#define DELAY 500000
#else
#define DELAY 50000000
#endif /* SIM_TARGET */

void set_test_task(RTX_TASK_INFO *task);
task_t create_rt(void);
void task_test_manager(void);
int get_test_result(void *data);

#endif /* _AE_PRIV_TASKS_H_ */
