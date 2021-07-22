/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTX LAB  
 *
 *                     Copyright 2020-2021 Yiqing Huang
 *                          All rights reserved.
 *---------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------*/

/**************************************************************************//**
 * @file        ae_task_util.h
 * @brief       task management lab auto-tester utility function header file
 *
 * @version     V1.2021.06
 * @authors     Yiqing Huang
 * @date        2021 May
 *
 *****************************************************************************/
#ifndef AE_TASKS_UTIL_H_
#define AE_TASKS_UTIL_H_
#include "ae.h"
#include "ae_inc.h"

/*
 *===========================================================================
 *                             STRUCTURES AND TYPES
 *===========================================================================
 */

/*
 *===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================
 */

int  dump_task_info (RTX_TASK_INFO *ptr);
int  dump_tasks     (task_t *p_tids, size_t len);
int  dump_mbx_info  (task_t tid);
int  dump_mailboxes (task_t *p_tids, size_t len);

#endif // ! AE_TASKS_UTIL_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

