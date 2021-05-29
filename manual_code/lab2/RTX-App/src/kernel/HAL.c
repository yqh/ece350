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
 * @brief   	SVC Handler
 * @note        assuming MSP is used in thread mode
 *              requires modification if PSP is used in thread mode
 *****************************************************************************/
__asm void SVC_Handler (void) 
{
    PRESERVE8                       // 8 bytes alignement of the stack
    EXPORT  SVC_RESTORE
    IMPORT  gp_current_task
    
    CPSID   I                       // disable interrupt       
    CMP     LR, #0xFFFFFFFD         // LR has the EXC_RETURN, If PSP used,  EXC_RETURN == 0xFFFFFFFD 
                                    // EXC_RETURN = 0xFFFFFFFF9 or 0xFFFFFFF1, then MSP is used
    ITE     EQ
    MRSEQ   R0, PSP                 // Read PSP
    MRSNE   R0, MSP                 // Read MSP
    
    LDR     R1, [R0, #24]           // Read Saved PC from SP
                                    // Loads R1 from a word 24 bytes above the address in R0
                                    // Note that R0 now contains the the SP value after the
                                    // exception stack frame is pushed onto the stack.
             
    LDRH    R1, [R1, #-2]           // Load halfword because SVC number is encoded there
    BICS    R1, R1, #0xFF00         // Extract SVC Number and save it in R1.  
                                    // R1 <= R1 & ~(0xFF00), update flags
                   
    BNE     SVC_EXIT                // if SVC Number !=0, exit
    
SVC_SAVE
    PUSH    {R0}                    // save user stack pointer on the kernel stack
    PUSH    {R4-R11, LR}            // Save other registers for preemption caused by i-procs
    LDM     R0, {R0-R3, R12}        // Read R0-R3, R12 from user stack. 
                                    // NOTE R0 contains the sp before this instruction
    BLX     R12                     // Call SVC C Function, 
                                    // R12 contains the corresponding 
                                    // C kernel functions entry point
                                    // R0-R3 contains the kernel function input parameter (See AAPCS)
SVC_RESTORE             
    POP     {R4-R11, LR}            // Restore other registers for preemption caused by i-procs
    POP     {R12}                   // pop user user space SP off and save it into R12.   
    STR     R0, [R12]               // store C kernel function return value in R0
                                    // to R0 on the exception stack frame  
    MSR     PSP, R12                // set the PSP
    
SVC_EXIT  
    CPSIE   I                       // enable interrupt
    BX      LR
    ALIGN                           // 4-byte alignment of an address within Thumb code
}
