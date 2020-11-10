/**
 * @file:   main_svc.c
 * IMPORTANT: This file will be replaced by another file in automted testing.
 */

#include <LPC17xx.h>
#include "rtx.h"
#include "priv_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "timer.h"

#ifdef RAM_TARGET
#define IROM_BASE  0x10000000
#else
#define IROM_BASE  0x0
#endif

extern void lcd_task(void);
extern void kcd_task(void);
extern void null_task(void);

int set_fixed_tasks(RTX_TASK_INFO *tasks, int num_tasks){

    if (num_tasks !=3) {
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
int main() 
{      
    RTX_TASK_INFO task_info[5];    /* 5 tasks, only 2 are used in uncommented code */
   
    /* CMSIS system initialization */
    SystemInit();  /* initialize the system */
    __disable_irq();
    timer_init(0);
    uart_init(1);  /* uart1 uses polling for output */
    init_printf(NULL, putc);
    __enable_irq();
#ifdef DEBUG_0
    printf("Dereferencing Null to get inital SP = 0x%x\r\n", *(U32 *)(IROM_BASE));
    printf("Derefrencing Reset vector to get intial PC = 0x%x\r\n", *(U32 *)(IROM_BASE + 4));
    printf("We are at privileged level, so we can access SP.\r\n"); 
    printf("Read MSP = 0x%x\r\n", __get_MSP());
    printf("Read PSP = 0x%x\r\n", __get_PSP());
#endif /*DEBUG_0*/    
    /* sets task information */
    set_auto_test_3(task_info);
    set_fixed_tasks(task_info + 1, 3);  /* kcd, lcd, null tasks */
    /* start the RTX and built-in tasks */
    rtx_init(32, FIRST_FIT, task_info, 4); 
    /* We should never reach here!!! */
    return RTX_ERR;  
}
