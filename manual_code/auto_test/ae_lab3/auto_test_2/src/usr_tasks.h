/**
 * @file:   usr_task.h
 * @brief:  Two user tasks header file
 * @author: Yiqing Huang
 * @date:   2020/08/07
 */
 
#ifndef USR_TASK_H_
#define USR_TASK_H_
#include "rtx.h"

#ifdef SIM_TARGET       /* using the simulator is slow */
#define DELAY 500000
#else
#define DELAY 50000000
#endif /* SIM_TARGET */

void auto_test_2(void);


#endif /* USR_TASK_H_ */
