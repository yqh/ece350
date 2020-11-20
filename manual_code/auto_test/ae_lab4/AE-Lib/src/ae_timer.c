/**
 * @brief ae_timer.c - Timer2 as a free running counter
 * @author Y. Huang
 * @author NXP Semiconductors
 * @date 2020/11/01
 */

#include "ae_timer.h"
#include "common.h"

#define BIT(X) ( 1 << (X) )


/* a free running counter set up, no interrupt fired */

uint32_t ae_timer_init_100MHZ(uint8_t n_timer) 
{
    LPC_TIM_TypeDef *pTimer;
    if (n_timer == 2) {
        pTimer = (LPC_TIM_TypeDef *) LPC_TIM2;
    } else { /* other timer not supported yet */
        return 1;
    }

    /* Power setting of TIMER2 
     * See Table 46 on page 63 of LPC17xx user's manual
     */
    LPC_SC->PCONP |= BIT(22);
    
    /* PCLK = CCLK, can also be set in system_LPC17xx.c by using configuratio wizard 
       We set explicitly here to overwrite the system_LPC17xx.c configuration so that
       we do not need to modify student's system_LPC17xx.c file
       See Tables 41 and 42 on page 57 of LPC17xx user's manual
     */
    
    LPC_SC->PCLKSEL1 |= BIT(12);
    LPC_SC->PCLKSEL1 &= ~BIT(13);
    

    /* Step 1. Prescale Register PR setting 
       CCLK = 100 MHZ, PCLK = CCLK
       Prescale counter increments every PCLK tick: (1/100)* 10^(-6) s = 10 ns
       TC increments every (PR + 1) PCLK cycles
       TC increments every (MR0 + 1) * (PR + 1) PCLK cycles 
    */
    pTimer->PR = 100000000 - 1; /* increment timer counter every 1*10^8 PCLK ticks, which is 1 sec */ 

    /* Step 2: MR setting, see section 21.6.7 on pg496 of LPC17xx_UM. */
    /* Effectively, using timer2 as a counter to measure time, there is no overflow in TC in 1K years */
    pTimer->MR0 = 0xFFFFFFFF - 1;  

    /* Step 3: MCR setting, see table 429 on pg496 of LPC17xx_UM.
       Reset on MR0: Reset TC if MR0 mathches it. No interrupt triggered on match.
    */
    pTimer->MCR = BIT(1);

    /* Step 4: Enable the counter. See table 427 on pg494 of LPC17xx_UM. */
    pTimer->TCR = 1;

    return 0;
}

int ae_get_tick(struct ae_tick *tm, uint8_t n_timer) 
{
    LPC_TIM_TypeDef *pTimer;
    if (n_timer == 2) {
        pTimer = (LPC_TIM_TypeDef *) LPC_TIM2;
    } else { /* other timer not supported yet */
        return 1;
    }
    
    tm->tc = pTimer->TC;
    tm->pc = pTimer->PC;
    
    return RTX_OK;
}
