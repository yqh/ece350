/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
 *
 *          This software is subject to an open source license and
 *          may be freely redistributed under the terms of MIT License.
 ****************************************************************************
 */

/**************************************************************************//**
 * @file        system_a9.c
 * @brief       Generic Cortex-A9 CMSIS System Initialization Source
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @attention
 * @note
 * @details
 *
 *****************************************************************************/

#include "k_HAL_CA.h"
#include "../DE1_SoC_A9/device_a9.h"
#include "../DE1_SoC_A9/interrupt.h"
#include "../DE1_SoC_A9/Serial.h"
#include "../DE1_SoC_A9/timer.h"

// statically allocated initial stacks except for SVC mode
U32 g_stacks[NUM_PRIV_MODES - 1][STACK_SZ >> 2];

/**************************************************************************//**
 * @brief		Set up stacks for each privileged mode except for SVC mode
 * @see			startup_a9.s Reset_Handler
 *****************************************************************************/
void StackInit(void) {
	int i = 0;
	__set_SP_MODE((U32) (g_stacks[++i]), INIT_MODE_SYS);
	__set_SP_MODE((U32) (g_stacks[++i]), INIT_MODE_IRQ);
	__set_SP_MODE((U32) (g_stacks[++i]), INIT_MODE_FIQ);
	__set_SP_MODE((U32) (g_stacks[++i]), INIT_MODE_ABT);
	__set_SP_MODE((U32) (g_stacks[++i]), INIT_MODE_UND);
}

/**************************************************************************//**
 * @brief		Setup the system.
 *         		Initialize the System and update the SystemCoreClock variable.
 * @note		not needed for lab1 or lab2
 *****************************************************************************/

void SystemInit(void) {
	GIC_Enable();
	GIC_EnableIRQ(UART0_Rx_IRQ_ID);
	GIC_EnableIRQ(HPS_TIMER0_IRQ_ID);
	GIC_EnableIRQ(HPS_TIMER1_IRQ_ID);
	GIC_EnableIRQ(A9_TIMER_IRQ_ID);
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
