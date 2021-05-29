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
 *****************************************************************************/

#include "ae.h"

/**************************************************************************//**
 * @brief   	ae_init
 * @return		RTX_OK on success and RTX_ERR on failure
 * @param[out]  sys_info system initialization struct AE writes to
 * @param[out]	task_info boot-time tasks struct array AE writes to
 *
 *****************************************************************************/

int	ae_init(RTX_SYS_INFO *sys_info, 
            TASK_INIT    *tasks, 
            int          num_tasks, 
            int          (*cb_func) (void *(arg)), 
            void         *arg)
{
	if ( ae_set_sys_info(sys_info) != RTX_OK ) {
		return RTX_ERR;
	}
    cb_func(arg);
	ae_set_task_info(tasks, num_tasks);
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

    sys_info->mem_algo      = BUDDY;
    //sys_info->start_addr    = RAM_START;
    //sys_info->end_addr      = RAM_END;
    return RTX_OK;
}

/**************************************************************************//**
 * @brief       fill the tasks array with information
 * @param[out]  tasks 		An array of TASK_INIT elements to write to
 * @param[in]	num_tasks	The length of tasks array
 * @return		None
 *****************************************************************************/

void ae_set_task_info(TASK_INIT *tasks, int num_tasks) 
{

	if (tasks == NULL) {
		return;
	}
    set_ae_tasks(tasks, num_tasks);
    return;
}

#ifdef AE_LAB1
int ae_start(void)
{
	return test_mem();
}
#endif

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

