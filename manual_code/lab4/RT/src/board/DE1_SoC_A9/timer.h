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
 * @brief       Timer driver header
 * @version     V1.2021.02
 * @authors     Zehan Gao
 * @date        2021 FEB
 *
 * @note	Only support UART0_irq in current version
 *		Reference to Intel University Program code
 *
 *****************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

#include "common.h"

#define SP0_TIMER_BASE  0xFFC08000
#define SP1_TIMER_BASE  0xFFC09000
#define ARM0_TIMER_BASE 0xFFFEC600

typedef unsigned        char uint8_t;
typedef unsigned short  int uint16_t;
typedef unsigned        int uint32_t;
typedef unsigned        __int64 uint64_t;

typedef struct{
    uint32_t timer1loadcount;                               // we only use timer1 inside of each timer module
    uint32_t timer1currentval;
    uint32_t timer1controlreg;
    uint32_t timer1eoi;
    uint32_t timer1intstat;
} timer_t;

typedef struct{
	uint32_t loadcount;
	uint32_t currentval;
	uint32_t controlreg;
	uint32_t intstat;
} arm_timer_t;

void timer_disable(int n);                                  // disable timer, n = 0-1 for HPS, n = 2 for A9 private
void timer_enable(int n);                                   // enable timer, n = 0-1 for HPS, n = 2 for A9 private
void timer_set_mode(int n, int mode);                       // set mode, 1 for user-defined count or auto and 0 for free-running or one-time
void timer_set_count(int n, int count);                     // set load count, only effective in user-defined count mode for n = 0-1
void timer_clear_irq(int n);                                // clear timer's interrupt request
unsigned int timer_get_current_val(int n);                  // get the current value of the timer's counter

void hps_timer_set_irq_mask(int n, int irq_mask);           // set irq mask, 1 for no interrupts and 0 for interrupts
void a9_timer_set_irq_bit(int n, int irq_bit);              // set irq bit, 0 for no interrupts and 1 for interrupts
void a9_timer_set_prescaler(U8 prescaler);


void config_hps_timer(int n, int count, int mode, int irq_mask);
void config_a9_timer(int count, int mode, int irq_bit, U8 prescaler);

void TIMER0_Interrupt(void);
void TIMER1_Interrupt(void);


#define TIMER0 ((timer_t *)SP0_TIMER_BASE)
#define TIMER1 ((timer_t *)SP1_TIMER_BASE)
#define ARMTIMER ((arm_timer_t *) ARM0_TIMER_BASE)

#endif
