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

#ifndef AE_
#define AE_

#include "Serial.h"
#include "printf.h"
#include "rtx.h"
//#include "ae_priv_tasks.h"
//#include "ae_usr_tasks.h"

 /*
  *===========================================================================
  *                            FUNCTION PROTOTYPES
  *===========================================================================
  */

/**************************************************************************//**
 * @file        ae.h
 * @brief       Automated Evaluation (AE) Framework Header File
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


/*
 *===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================
 */

int	 ae_init			(RTX_SYS_INFO *sys_info, \
		                 RTX_TASK_INFO *task_info, \
						 int num_tasks);
int  ae_set_sys_info	(RTX_SYS_INFO *sys_info);
void ae_set_task_info	(RTX_TASK_INFO *tasks, int num_tasks);
int  ae_start           (void);

extern int test_mem     (void);

#endif // ! AE_
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

