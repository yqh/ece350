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
 * @file:   	main_svc.c
 * @brief:  	main routine to start up the RTX and two initial tasks
 * @version     V1.2021.01.lab2
 * @authors     Yiqing Huang
 * @date        2021 JAN
 * @note 		standard C library is not allowed in the final kernel code.
 *       		A tiny printf function for embedded application development
 *       		taken from http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
 *       		is configured to use UART0 to output when DEBUG_0 is defined.
 *       		The init_printf(NULL, putc) MUST be called to initialize
 *       		the printf function.
 *****************************************************************************/


#include "ae.h"
#include "system_a9.h"
#include "Serial.h"
#include "printf.h"
#include "k_inc.h"
#include "k_rtx.h"


extern void __ch_MODE (U32 mode);
extern void __atomic_on(void);
extern void __atomic_off(void);


void task_null (void)
{
    while (1) {
#ifdef DEBUG_0
        for ( int i = 0; i < 5; i++ ){
            printf("==============Task NULL===============\r\n");
        }
#endif
        k_tsk_yield();
    }
}

int main() 
{    
    RTX_SYS_INFO  sys_info;
	RTX_TASK_INFO task_info[2];
    char mode = 0;

    // CMSIS system initialization
    SystemInit();

    __atomic_on();
    SER_Init();  				// uart1 uses polling for output
    init_printf(NULL, putc);	// printf uses uart1 for output
    __atomic_off();

    mode = __get_mode();
    printf("mode = 0x%x\r\n", mode);

    // System and Task set up by auto testing software
    if (ae_init(&sys_info, task_info, 2) != RTX_OK) {
    	printf("RTX INIT FAILED\r\n");
    	return RTX_ERR;
    }

    // start the RTX and built-in tasks
    if (mode == MODE_SVC) {
        gp_current_task = NULL;
        k_rtx_init(task_info, 2);
    }

    task_null();

    // We should never reach here!!!
    return RTX_ERR;  
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
