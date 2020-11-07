/**
 * @brief Free-running timer example
 * @author Yiqing Huang
 * @date 2020/11/01
 */

#include <LPC17xx.h>
#include "timer.h"
#include "uart_polling.h"  
#include "printf.h"

#define TIMER1 1

void add_op(uint32_t num_op) 
{
    uint32_t x = 0;
    uint32_t i = 0;
    
    for ( i = 0; i < num_op; i++ ) {
        x++;
    }
    return;
}

/**
 *
 */
void get_runtime(uint8_t n_timer, void (*pfunc)(uint32_t), uint32_t num_op) {
    
    uint32_t e_tc;
    uint32_t e_pc;
    
    if (n_timer != 1) {  // only supports timer 1
        return;
    }
    LPC_TIM_TypeDef *pTimer = LPC_TIM1;
    
    pTimer->TCR = 2;  // disable counter, reset counters     
    pTimer->TCR = 1;  //enable counter
    
    /* this is the operation we want to measure the time */
    pfunc(num_op);
    
    pTimer->TCR = 0; //disable counter
    e_tc = pTimer->TC;
    e_pc = pTimer->PC;
    printf("@end: TC = %u, PC = %u. \r\n", e_tc, e_pc);
    return;
}



int main()
{
    volatile uint8_t sec = 0;
    uint32_t num_op = 1;

    SystemInit();
    __disable_irq();
    timer_init_100MHZ(TIMER1);   /* timer1 PCLK=100MHZ, free-running counter, no interrupts */
    uart1_init();           /* uart1 is polling */
    init_printf(NULL, putc);
    __enable_irq();
    
    for (int i=0; i < 10; i++) {
        printf("repeat = %d\r\n", i);
        get_runtime(TIMER1, &add_op, num_op);
        num_op *= 10;
    }
}


