/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
 *
 *          This software is subject to an open source license and
 *          may be freely redistributed under the terms of MIT License.
 ****************************************************************************
 */


/**************************************************************************//**
 * @file:   	main_svc.c
 * @brief:  	main routine to start up the RTX and two initial tasks
 * @version     V1.2021.01
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
#include "k_HAL_CA.h"


void task_null (void)
{
    for ( int i = 0; i < 5; i++ ){
        printf("==============Task NULL===============\r\n");
    }
	while (1);
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

    __ch_MODE(MODE_SYS);
    mode = __get_mode();
    printf("mode = 0x%x\r\n", mode);

    __ch_MODE(MODE_USR);
    mode = __get_mode();
    printf("mode = 0x%x\r\n", mode);

    __ch_MODE(MODE_SYS);
    mode = __get_mode();
    printf("mode = 0x%x\r\n", mode);

    // System and Task set up by auto testing software
    if (ae_init(&sys_info, task_info, 2) != RTX_OK) {
    	printf("RTX INIT FAILED\r\n");
    	return RTX_ERR;
    }

    int ret = mem_init();
    if (ret != RTX_OK) {
       printf("mem_init failed.\r\n");
       return RTX_ERR;
    }

    printf("test result = %d\r\n", ae_start());

    task_null();

    // We should never reach here!!!
    return RTX_ERR;  
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
