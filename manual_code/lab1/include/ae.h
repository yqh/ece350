/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2022 Yiqing Huang
 *                          All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice and the following disclaimer.
 *
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************
 */

#ifndef AE_H_
#define AE_H_

#include "uart_polling.h"
#include "printf.h"
#include "rtx.h"
#include "rtx_errno.h"

 /*
  *===========================================================================
  *                            FUNCTION PROTOTYPES
  *===========================================================================
  */

/**************************************************************************//**
 * @file        ae.h
 * @brief       Automated Evaluation (AE) Framework Header File
 *
 * @version     V1.2022.05
 * @authors     Yiqing Huang
 * @date        2022 MAY
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
int	ae_init                 (RTX_SYS_INFO *sys_info, 
                             TASK_INIT    **tasks, 
                             int          *num_tasks, 
                             int          (*cb_func) (void *(arg)), 
                             void         *arg);            

void ae_exit                (void);                            
int  ae_set_sys_info        (RTX_SYS_INFO *sys_info);
void ae_set_init_tasks_info (TASK_INIT **pp_tasks, int *p_num_tasks);                                                         
void set_ae_init_tasks      (TASK_INIT **tasks, int *num);  
void set_ae_tasks(TASK_INIT *task, int num);
                         
#endif // ! AE_H_
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

