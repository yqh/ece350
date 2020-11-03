/**
 * @brief Timer interrupt example code. 
          Prints 0-9 every second. Loops around when 9 is reached.
 * @author Yiqing Huang
 * @date 2020/10/02
 */

#include <LPC17xx.h>
#include "timer.h"
#include "uart_polling.h"  

extern volatile uint32_t g_timer_count;

int main () {

    volatile uint8_t sec = 0;

    SystemInit();
    __disable_irq();
    timer_init(0); /* initialize timer 0 */
    uart1_init();  /* uart1 is polling */
    __enable_irq();
   
    while (1) {
        /* g_timer_count gets updated every 1 usec */
        if (g_timer_count == 1000000) { 
            uart1_put_char('0'+ sec);
            sec = (sec + 1)%10;
            g_timer_count = 0; /* reset the counter */
        }     
    }
}
