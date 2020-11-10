/**
 * @file:   priv_tasks.h
 * @brief:  Two kernel tasks header file
 * @author: Yiqing Huang
 * @date:   2020/09/23
 */
 
#ifndef PRIV_TASKS_H_
#define PRIV_TASKS_H_

#include "rtx.h"

#ifdef SIM_TARGET       /* using the simulator is slow */
#define DELAY 500000
#else
#define DELAY 50000000
#endif /* SIM_TARGET */

void set_task_info(RTX_TASK_INFO *tasks,int num_tasks);

void set_auto_test_3(RTX_TASK_INFO *tasks);

#endif /* PRIV_TASKS_H_ */
