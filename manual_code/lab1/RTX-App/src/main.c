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

/**************************************************************************//**
 * @brief   	main routine
 *          
 * @return      0 on success and non-zero on failure
 *****************************************************************************/
int main() 
{   
    /* initial tasks */
    static RTX_SYS_INFO sys;
    
    /* CMSIS system initialization */
    SystemInit();   
    __disable_irq();
    
    /* uart1 by polling */  
    uart1_init();                        
    
    /* initialize printf to use uart1 by polling */
    init_printf(NULL, putc);
    
    __enable_irq();
    

#ifdef DEBUG_1
    U32 ctrl = 0;
    ctrl = __get_CONTROL();
    printf("ctrl = %d, We are at privileged level, so we can access SP.\r\n", ctrl); 
	  printf("Read MSP = 0x%x\r\n", __get_MSP());
	  printf("Read PSP = 0x%x\r\n", __get_PSP());
#endif // DEBUG_1	
   
    /* initialize the third-party testing framework */
    ae_init(&sys, NULL , 0, &k_pre_rtx_init, NULL);
   
    /* start the RTX */
    rtx_init(&sys, NULL, 0);  
    ae_start();
  
    /* We should never reach here!!! */
    return RTX_ERR;  
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
