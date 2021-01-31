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
#include "system_a9.h"

// statically allocated initial stacks except for SVC mode and SYS mode
U32 g_stacks[NUM_PRIV_MODES - 2][STACK_SZ >> 2];

/**************************************************************************//**
 * @brief		Set up stacks for each privileged mode except for SVC mode
 * @see			startup_a9.s Reset_Handler
 *****************************************************************************/
void StackInit(void) {
	int i = 0;
	__set_SP_MODE((U32) (g_stacks[++i]), MODE_IRQ);
	__set_SP_MODE((U32) (g_stacks[++i]), MODE_FIQ);
	__set_SP_MODE((U32) (g_stacks[++i]), MODE_ABT);
	__set_SP_MODE((U32) (g_stacks[++i]), MODE_UND);
	__set_SP_MODE((U32) (g_p_stacks[1]), MODE_SYS);
}

/**************************************************************************//**
 * @brief		Setup the system.
 *         		Initialize the System and update the SystemCoreClock variable.
 * @note		not needed for lab1 or lab2
 *****************************************************************************/

void SystemInit(void) {
	// 1. copy vector table , not needed for VE9
	// 2. TODO set up system clocks for devices, not needed for lab1 or lab2
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
