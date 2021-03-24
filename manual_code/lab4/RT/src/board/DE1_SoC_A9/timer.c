/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Zehan Gao
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
 * @file        timer.c
 * @brief       Timer driver code
 * @version     V1.2021.02
 * @authors     Zehan Gao
 * @date        2021 FEB
 *
 * @note	Only support UART0_irq in current version
 *		Reference to Intel University Program code
 *
 *****************************************************************************/
#include "printf.h"
#include "timer.h"
#include "common.h"

timer_t* TIMERS[2] = {TIMER0, TIMER1};
void config_hps_timer(int n, int count, int mode, int irq_mask)
{
	if (n < 2)
	{
		timer_disable(n);
		timer_set_count(n,count);
		timer_set_mode(n,mode);
		hps_timer_set_irq_mask(n,irq_mask);
		timer_enable(n);
	}
}
void config_a9_timer(int count, int mode, int irq_bit, U8 prescaler)
{
	timer_disable(2);
	timer_set_count(2,count);
	timer_set_mode(2,mode);
	a9_timer_set_irq_bit(2, irq_bit);
	a9_timer_set_prescaler(prescaler);
	timer_enable(2);
}
void timer_disable(int n)
{
	// Set bit 0 of control register to 0 to disable timer
	if(n >=0 && n <= 1)
		TIMERS[n]->timer1controlreg &= ~(0x1);
	else if (n == 2)
		ARMTIMER->controlreg &= ~(0x1);
}
void timer_enable(int n)
{
	// Set bit 0 of control register to 1 to enable timer
	if(n >=0 && n <= 1)
		TIMERS[n]->timer1controlreg |= 0x1;
	else if(n == 2)
		ARMTIMER->controlreg |= 0x1;
}
void timer_set_mode(int n, int mode)
{
	if(mode == 0)
	{
		if(n >= 0 && n <= 1)
			//Set bit 1 of the control register to 0 to set the mode to free-running mode
			TIMERS[n]->timer1controlreg &= ~(0x2);
		else if(n == 2)
			//Set bit 1 of the control register to 0 to set the mode to one-time mode
			ARMTIMER->controlreg &= ~(0x2);
	}
	else if(mode == 1)
	{
		if(n >= 0 && n <= 1)
			//Set bit 1 of the control register to 1 to set the mode to user-defined mode
			TIMERS[n]->timer1controlreg |= 0x2;
		else if(n == 2)
			//Set bit 1 of the control register to 1 to set the mode to auto mode
			ARMTIMER->controlreg |= 0x2;
	}
}
void timer_set_count(int n, int count)
{
	//Set the load count register to the given count
	if(n >= 0 && n <= 1)
		TIMERS[n]->timer1loadcount = count;
	else if(n == 2)
		ARMTIMER->loadcount = count;
}
void timer_clear_irq(int n)
{
	volatile int t;
	if(n >= 0 && n <= 1)
		t = TIMERS[n]->timer1eoi; //Read the EOI register to clear the IRQ
	else if(n == 2)
		ARMTIMER->intstat = 0x1;  //Write to the interrupt status register to clear the IRQ
}
unsigned int timer_get_current_val(int n)
{
	// Return the current value of the counter in timer
	if(n >= 0 && n <= 1)
		return TIMERS[n]->timer1currentval;
	else if(n == 2)
		return ARMTIMER->currentval;
	return 0;
}

void hps_timer_set_irq_mask(int n, int irq_mask)
{
	if(n >= 0 && n <= 1)
	{
		if(irq_mask == 0)
		{
			TIMERS[n]->timer1controlreg &= ~(0x4);  //Set bit 2 of the control register to 0 to disable interrupt mask (enable interrupt).
		}
		else if(irq_mask == 1)
		{
			TIMERS[n]->timer1controlreg |= 0x4;     //Set bit 2 of the control register to 1 to enable interrupt mask (disable interrupt).
		}
	}
}

void a9_timer_set_irq_bit(int n, int irq_bit)
{
	if(n == 2)
	{
		if(irq_bit == 0)
			ARMTIMER->controlreg &= ~(0x4);			//Set bit 2 of the control register to 0 to disable interrupt
		else if(irq_bit == 1)
			ARMTIMER->controlreg |= 0x4;            //Set bit 2 of the control register to 1 to enable interrupt
	}
}
void a9_timer_set_prescaler(uint8_t prescaler)
{
	//Set bit 8-15 of the control register using word addressing operations
	volatile uint32_t controlreg = ARMTIMER->controlreg;
	controlreg &= 0xF;
	ARMTIMER->controlreg = (uint32_t) ((prescaler << 8) + controlreg);
}
