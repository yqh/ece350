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
 * @file        k_rtx_init.c
 * @brief       RTX System Initialization C file
 *              l2
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @details
 * @note
 *
 *****************************************************************************/

#include "k_rtx_init.h"
#include "Serial.h"
#include "k_mem.h"
#include "k_task.h"

int k_rtx_init(RTX_TASK_INFO *task_info, int num_tasks)
{
    /* interrupts are already disabled when we enter here */
    if ( k_mem_init() != RTX_OK) {
        return RTX_ERR;
    }

    if ( k_tsk_init(task_info, num_tasks) != RTX_OK ) {
        return RTX_ERR;
    }
    
    /* start the first task */
    //return k_tsk_start();
    return RTX_OK;
}

int k_rtx_init_rt(RTX_SYS_INFO *sys_info, RTX_TASK_INFO *task_info, int num_tasks)
{
    /* initialize the scheduler here */
    k_rtx_init(task_info, num_tasks);
    return RTX_OK;
}

int k_get_sys_info(RTX_SYS_INFO *buffer)
{
    return RTX_OK;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
