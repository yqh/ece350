/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
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

/**************************************************************************//**
 * @file        ae_tasks.h
 * @brief       tasks header file
 *
 * @version     V1.2021.05
 * @authors     Yiqing Huang
 * @date        2021 MAY
 *
 *****************************************************************************/

#ifndef AE_TASKS_H_
#define AE_TASKS_H_

#include "rtx.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */
#ifdef SIM_TARGET       // using the simulator is slow
#define DELAY 100000
#else
#define DELAY 10000000
#endif // SIM_TARGET

/*
 *===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================
 */
void set_ae_init_tasks  (TASK_INIT **tasks, int *num);  // NEW function
void set_ae_tasks       (TASK_INIT *tasks, int num);  
void priv_task1         (void);
void task0              (void);
void task1              (void);
void task2              (void);
void task3              (void);

void gen_req0           (int test_id);
int  test0_start        (int test_id);
void test0_end          (int test_id);
void init_ae_tsk_test   (void);
int  update_exec_seq    (int test_id, task_t tid);

#endif // !AE_TASKS_H_
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
