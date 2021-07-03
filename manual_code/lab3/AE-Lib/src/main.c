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
 * @file        main_lab1.c
 * @brief       main routine to start up the RTX in kernel mode 
 *              
 * @version     V1.2021.04
 * @authors     Yiqing Huang
 * @date        2021 APR
 * @note        Standdard C library is not allowed in the final kernel code.
 *              A tiny printf function for embedded application development
 *              taken from http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
 *              is configured to use UAR1 to output.
 *              Note that init_printf(NULL, putc) must be called to initialize 
 *              the printf function.
 *****************************************************************************/

#include <LPC17xx.h>
#include "rtx.h"
#include "rtx_errno.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae.h"

//#define NUM_INIT_TASKS 2

/**************************************************************************//**
 * @brief   	main routine
 *          
 * @return      0 on success and non-zero on failure
 *****************************************************************************/
int main() 
{   
    /* initial tasks */
    static RTX_SYS_INFO sys;
    //static TASK_INIT tasks[NUM_INIT_TASKS];
    static TASK_INIT *p_tasks = NULL;
    static int num = 0;
    
    /* CMSIS system initialization */
    SystemInit();   
    __disable_irq();
    
    /* uart1 by polling */  
    uart1_init();                        
    
    /* initialize printf to use uart1 by polling */
    init_printf(NULL, putc);
    
    __enable_irq();
    
    U32 ctrl = __get_CONTROL();
#ifdef DEBUG_1    
    printf("ctrl = %d, We should be at privileged level upon reset, so we can access SP.\r\n", ctrl); 
    printf("Read MSP = 0x%x\r\n", __get_MSP);
    printf("Read PSP = 0x%x\r\n", __get_PSP());
#endif // DEBUG_1    

    
    /* initialize the third-party testing framework */
    //ae_init(&sys, tasks , NUM_INIT_TASKS, &k_pre_rtx_init, NULL);   
    //ae_init(&sys, tasks , NUM_INIT_TASKS, &k_pre_rtx_init, &sys);
    ae_init_new(&sys, &p_tasks, &num, &k_pre_rtx_init, &sys);
    __set_CONTROL(__get_CONTROL() | BIT(1));
    __isb(15); // see https://www.keil.com/support/man/docs/armcc/armcc_chr1435075770601.htm#:~:text=This%20intrinsic%20inserts%20an%20ISB,is%20also%20an%20optimization%20barrier.
    /* start the RTX */
    //rtx_init(&sys, tasks, NUM_INIT_TASKS);
    rtx_init(&sys, p_tasks, num);

   
    /* We should never reach here!!! */
    return RTX_ERR;  
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
