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
 * @file        ae_task_util.c
 * @brief       task management lab auto-tester utility functions
 *
 * @version     V1.2021.06
 * @authors     Yiqing Huang
 * @date        2021 May
 *
 *****************************************************************************/
 
#include "ae_tasks_util.h"
#include "ae_util.h"


int dump_task_info(RTX_TASK_INFO *ptr) 
{

    if ( ptr == NULL ) {
        return RTX_ERR;
    }
    
    printf("tid:          %d\r\n",   ptr->tid);
    printf("state:        %d\r\n",   ptr->state);
    printf("prio:         %d\r\n",   ptr->prio);
    printf("priv:         %d\r\n",   ptr->priv);
    printf("ptask:        0x%x\r\n", ptr->ptask);
    printf("k_sp_base:    0x%x\r\n", ptr->k_sp_base);
    printf("k_sp:         0x%x\r\n", ptr->k_sp);
    printf("k_stack_size: 0x%x\r\n", ptr->k_stack_size);
    printf("u_sp_base:    0x%x\r\n", ptr->u_sp_base);
    printf("u_sp:         0x%x\r\n", ptr->u_sp);
    printf("u_stack_size: 0x%x\r\n", ptr->u_stack_size);
    
    return RTX_OK;
}

int dump_tasks(task_t *p_tids, size_t len)
{
    RTX_TASK_INFO task_info;
    
    if ( p_tids == NULL ) {
        return RTX_ERR;
    }
    
    for ( int i = 0; i < len; i++ ) {
        int ret_val = tsk_get(p_tids[i], &task_info);
        if ( ret_val == RTX_OK ) {
            dump_task_info(&task_info);
        } else {
            printf ("ERR: tsk_get failed\r\n");
        }
    }
    return RTX_OK;
}

int dump_mbx_info(task_t tid)
{
    int space = mbx_get(tid);
    
    if ( space < 0 ) {
        printf("ERR: mbx_get on tid=%u failed\r\n", tid);
        return RTX_ERR;
    } 
    
    printf("TID = %u, malbox free bytes = %u.\r\n", tid, space); 
    return RTX_OK;
}

int dump_mailboxes(task_t *p_tids, size_t len)
{
    if ( p_tids == NULL ) {
        return RTX_ERR;
    }
    
    for ( int i = 0; i < len; i++) {
        printf("tid %u has a mailbox\r\n", p_tids[i]);
        dump_mbx_info(p_tids[i]);   
    }
    return RTX_OK;
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
