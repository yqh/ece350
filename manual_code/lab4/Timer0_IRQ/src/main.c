/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO SE 350 RTX LAB  
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
 * @file        main.c
 * @brief       Timer0 interrupt main driver code
 *              Prints 0,1,2 ... every second. 
 *              Outputs Timer1 TC and PC register values
 *              
 * @version     V1.2021.07
 * @authors     Yiqing Huang
 * @date        2021 JUL
 *****************************************************************************/

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "timer.h"
#include "uart_polling.h" 
#include "printf.h"

extern volatile uint32_t g_timer_count;

int main () {

    volatile uint8_t sec = 0;
    TM_TICK tk ={ 0, 0};

    SystemInit();
    __disable_irq();
    timer_irq_init(TIMER0);     /* initialize timer0, fires interrupts per 500 usec */
    timer_freerun_init(TIMER1); /* initialize timer1, free running timer            */
    uart1_init();   
    init_printf(NULL, putc);
    __enable_irq();
    uart1_put_string("\r\nTimer0 IRQ fires every 500 microseconds.\r\n");
    uart1_put_string("Timer1 is a free running timer. PC increments very 10 nsec and TC increments every 1 sec.\r\n");
    while (1) {
        /* g_timer_count gets updated every 500 us */
        if (g_timer_count % 2000 == 0) { 
            int retval = get_tick(&tk, TIMER1);
            if ( retval != 0 ) {
                printf("ERR: reading timer1 register failed\r\n");
            }
            printf("%u: TC = %u, PC = %u\r\n", sec++, tk.tc, tk.pc);
        }     
    }

}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
