/**
 * @brief timer.c - Timer1 as a free running counter
 * @author Y. Huang
 * @author NXP Semiconductors
 * @date 2020/111/01
 */

#include "timer.h"

#define BIT(X) ( 1 << (X) )


/* a free running counter set up, no interrupt fired */

uint32_t timer_init_100MHZ(uint8_t n_timer) 
{
    LPC_TIM_TypeDef *pTimer;
    if (n_timer == 1) {
        pTimer = (LPC_TIM_TypeDef *) LPC_TIM1;
    } else { /* other timer not supported yet */
        return 1;
    }


    /* Step 1. Prescale Register PR setting 
       CCLK = 100 MHZ, PCLK = CCLK
       Prescalar counter increments every PCLK tick: (1/100)* 10^(-6) s = 10 ns
       TC increments every (PR + 1) PCLK cycles
       TC increments every (MR0 + 1) * (PR + 1) PCLK cycles 
    */
    pTimer->PR = 4000000000 - 1; /* increment timer counter every 4*10^9 PCLK ticks, which is 40 sec */ 

    /* Step 2: MR setting, see section 21.6.7 on pg496 of LPC17xx_UM. */
    /* Effectively, using timer1 as a counter to measure time, there is no overflow in TC in 4K-5K years */
    pTimer->MR0 = 0xFFFFFFFF - 1;  

    /* Step 3: MCR setting, see table 429 on pg496 of LPC17xx_UM.
       Reset on MR0: Reset TC if MR0 mathches it. No interrupt triggered on match.
    */
    pTimer->MCR = BIT(1);

    /* Step 4: Enable the counter. See table 427 on pg494 of LPC17xx_UM. */
    pTimer->TCR = 1;

    return 0;
}
