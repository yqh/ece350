/**
 * @file:   priv_tasks.c
 * IMPORTANT: This file will be replaced by anothe file in automated testing.
 */

#include "uart_polling.h"
#include "priv_tasks.h"
#include "usr_tasks.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

void set_auto_test_2(RTX_TASK_INFO *tasks)
{
	tasks[0].u_stack_size = 512;
	tasks[0].prio = MEDIUM;
	tasks[0].priv = 0;
	tasks[0].ptask = &auto_test_2;
}
