/**
 * @file:   main_svc.c
 * @brief:  main routine to start up the RTX and compile-time created tasks
 * @author: Yiqing Huang
 * @date:   2020/09/20
 * NOTE: standard C library is not allowed in the final kernel code.
 *       A tiny printf function for embedded application development
 *       taken from http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
 *       is configured to use UART0 to output when DEBUG_0 is defined.
 *       Check target option->C/C++ to see the DEBUG_0 definition.
 *       Note that init_printf(NULL, putc) must be called to initialize 
 *       the printf function.
 */

#include <LPC17xx.h>
#include "rtx.h"
#include "usr_task.h"
#include "uart_polling.h"
#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

void set_task_info(RTX_TASK_INFO *tasks) {
    tasks[0].ptask = &task1;
    tasks[0].prio = MEDIUM;
    tasks[0].stack_size = 0x0200;
    tasks[1].ptask = &task2;
    tasks[1].prio = MEDIUM;
    tasks[1].stack_size = 0x0200;
}

int main() 
{    
    RTX_TASK_INFO task_info[2];    
    /* CMSIS system initialization */
    SystemInit(); 
    __disable_irq();
    uart1_init(); /* uart1 polling */
#ifdef DEBUG_0
    init_printf(NULL, putc);
#endif /* DEBUG_0 */
    __enable_irq();
    /* user provides task information */
    set_task_info(task_info);
    /* start the RTX and built-in tasks */
    rtx_init(32, FIRST_FIT, task_info);  
    /* We should never reach here!!! */
    return RTX_ERR;  
}
