/**
 * @file:   ae_main_svc.c
 * @brief:  ae_main routine to start up the RTX
 * @author: Yiqing Huang
 * NOTE: standard C library is not allowed in the final kernel code.
 *       A tiny printf function for embedded application development
 *       taken from http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
 *       is configured to use UART0 to output when DEBUG_0 is defined.
 *       Check target option->C/C++ to see the DEBUG_0 definition.
 *       Note that init_printf(NULL, putc) must be called to initialize 
 *       the printf function.
 * IMPORTANT: This file will be replaced by another file in automted testing.
 */

#include <LPC17xx.h>
#include "ae.h"

U32 g_test_result = 0;   /* each bit is a test result */


int set_fixed_tasks(RTX_TASK_INFO *tasks, int num_tasks){

	if (num_tasks != 3) {
        return RTX_ERR;
    }
    
    tasks[0].ptask = &lcd_task;
    tasks[0].u_stack_size = 0x0;
    tasks[0].prio = HIGH;
    tasks[0].priv = 1;
    
    tasks[1].ptask = &kcd_task;
    tasks[1].u_stack_size = 0x100;
    tasks[1].prio = HIGH;
    tasks[1].priv = 0;
    
    tasks[2].ptask = &null_task;
    tasks[2].u_stack_size = 0x100;
    tasks[2].prio = PRIO_NULL;
    tasks[2].priv = 0;
				
    return RTX_OK;
}

int set_wall_clock_task(RTX_TASK_INFO *task){
		task->ptask = &wall_clock_task;
		task->u_stack_size = 0x100;
		task->priv = 0;
		task->prio = HIGH;
	
		return RTX_OK;
}

int main() 
{
    RTX_TASK_INFO task_info[AE_TASKS];    

    /* CMSIS system initialization */
    SystemInit();  /* initialize the system */
    __disable_irq();
    uart_init(1);  /* uart1 uses polling for output */
    init_printf(NULL, putc);
    ae_timer_init_100MHZ(2);
    __enable_irq();

#ifdef DEBUG_0
    printf("Dereferencing Null to get inital SP = 0x%x\r\n", *(U32 *)(IROM_BASE));
    printf("Derefrencing Reset vector to get intial PC = 0x%x\r\n", *(U32 *)(IROM_BASE + 4));
    printf("We are at privileged level, so we can access SP.\r\n"); 
    printf("Read MSP = 0x%x\r\n", __get_MSP());
    printf("Read PSP = 0x%x\r\n", __get_PSP());
#endif /*DEBUG_0*/    
	
    set_fixed_tasks(task_info, 3);  /* kcd, lcd, null tasks */
    set_wall_clock_task(task_info + 3);
    set_test_task(task_info + 4);
    g_test_result = 0;
    rtx_init(32, FIRST_FIT, task_info, 5);


    /* We should never reach here!!! */
    return RTX_ERR;  
}
