/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
 *
 *          This software is subject to an open source license and
 *          may be freely redistributed under the terms of MIT License.
 ****************************************************************************
 */

/**************************************************************************//**
 * @file        ae.c
 * @brief       Automated Evaluation (AE) Framework Source File
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @attention
 * @note
 * @details
 *
 *****************************************************************************/

#include "ae.h"

/**************************************************************************//**
 * @brief   	ae_init
 * @return		RTX_OK on success and RTX_ERR on failure
 * @param[out]  sys_info system initialization struct AE writes to
 * @param[out]	task_info boot-time tasks struct array AE writes to
 *
 *****************************************************************************/

int	ae_init(RTX_SYS_INFO *sys_info, RTX_TASK_INFO *task_info, int num_tasks)
{
	if ( ae_set_sys_info(sys_info) != RTX_OK ) {
		return RTX_ERR;
	}

	ae_set_task_info(task_info, num_tasks);
	return RTX_OK;
}

/**************************************************************************//**
 * @brief       fill the sys_info struct with system configuration info.
 * @return		RTX_OK on success and RTX_ERR on failure
 * @param[out]  sys_info system initialization struct AE writes to
 *
 *****************************************************************************/
int ae_set_sys_info(RTX_SYS_INFO *sys_info)
{
    if (sys_info == NULL) {
        return RTX_ERR;
    }


    /*---------- NOT USED in LAB1 -------------------
    // Scheduling sys info set up, not used in lab1, ignore for now
    struct timeval_rt budget;
    struct timeval_rt period;

    budget.sec = 0;
    budget.usec = MIN_RTX_QTM * 10;

    period.sec = 0;
    period.usec = MIN_RTX_QTM * 100;


    sys_info->rtx_time_qtm = 10 * MIN_RTX_QTM;
    sys_info->sched = RM_PS;
    sys_info->server.b_n = budget;
    sys_info->server.p_n = period;
    -------------------------------------------------*/
    return RTX_OK;
}

/**************************************************************************//**
 * @brief       fill the tasks array with information
 * @param[out]  tasks 		An array of RTX_TASK_INFO elements to write to
 * @param[in]	num_tasks	The length of tasks array
 * @return		None
 *****************************************************************************/

void ae_set_task_info(RTX_TASK_INFO *tasks, int num_tasks) {

	if (tasks == NULL) {
		return;
	}
/*---------- NOT USED in LAB1 -------------------
    for (int i = 0; i < num_tasks; i++ ) {
        tasks[i].u_stack_size = 0x0;
        tasks[i].prio = HIGH;
        tasks[i].priv = 1;
    }
    tasks[0].ptask = &priv_task1;
    tasks[1].ptask = &priv_task2;
-------------------------------------------------*/
    return;
}

int ae_start(void)
{
	return test_mem();
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

