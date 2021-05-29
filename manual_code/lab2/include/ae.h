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

#include "uart_polling.h"
#include "printf.h"
#include "rtx.h"

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

int ae_init             (RTX_SYS_INFO *sys_info, \
                         TASK_INIT    *tasks, \
                         int          num_tasks, \
                         int          (*cb_func) (void *(arg)), \
                         void         *arg);
                            
int  ae_set_sys_info    (RTX_SYS_INFO *sys_info);
void ae_set_task_info   (TASK_INIT *tasks, int num_tasks);

#ifdef AE_LAB1                         
int  ae_start           (void);
extern int test_mem     (void);
#else
void set_ae_tasks(TASK_INIT *task, int num);
#endif
                         
#endif // ! AE_
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

