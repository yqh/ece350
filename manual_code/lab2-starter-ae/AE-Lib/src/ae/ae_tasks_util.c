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
    
    printf("tid:\t %d\r\n", ptr->tid);
    printf("state:\t %d\r\n", ptr->state);
    printf("prio:\t %d\r\n", ptr->prio);
    printf("priv:\t %d\r\n", ptr->priv);
    printf("ptask:\t 0x%x\r\n", ptr->ptask);
    printf("k_sp_base:\t 0x%x\r\n", ptr->k_sp_base);
    printf("k_sp:\t 0x%x\r\n", ptr->k_sp);
    printf("k_stack_size:\t 0x%x\r\n", ptr->k_stack_size);
    printf("u_sp_base:\t 0x%x\r\n", ptr->u_sp_base);
    printf("u_sp:\t 0x%x\r\n", ptr->u_sp);
    printf("u_stack_size:\t 0x%x\r\n", ptr->u_stack_size);
    
    return RTX_OK;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
