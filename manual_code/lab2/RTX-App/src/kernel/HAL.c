/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO SE 350 RTX LAB  
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
 * @file        HAL.c
 * @brief       Hardware Abstraction Layer 
 *              
 * @version     V1.2021.05
 * @authors     Yiqing Huang
 * @date        2021 MAY
 * @note        This file contains embedded assembly. 
 *              The code borrowed some ideas from ARM RL-RTX source code
 *
 *****************************************************************************/

#include "k_rtx.h"
#include "k_inc.h"

/**************************************************************************//**
 * @brief   	pop off exception stack frame from the stack
 * @pre         PSP is used in thread mode before entering any exception
 *              SVC_Handler is configured as the highest interrupt priority
 *****************************************************************************/
__asm void __rte(void)
{
    PRESERVE8
    EXPORT  SVC_RTE
SVC_RTE
    MVN     LR, #:NOT:0xFFFFFFFD    // set EXC_RETURN value, Thread mode, PSP
    BX      LR    
    ALIGN
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

