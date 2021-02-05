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
 * @file        k_inc.h
 * @brief       Kernel Macros and Data Structure Header file
 *              l2
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @note        all kernel .c files include this one
 *
 *****************************************************************************/

#ifndef K_INC_H_
#define K_INC_H_

#include "device_a9.h"
#include "common.h"

/*
 *==========================================================================
 *                   GLOBAL VARIABLES DECLARATIONS
 *==========================================================================
 */
// Memory related globals are defined in k_mem.c

// kernel stack size
extern const U32 g_k_stack_size;
// task kernel stacks are statically allocated inside the OS image

extern U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));

extern unsigned int Image$$ZI_DATA$$ZI$$Limit; 	// Linker defined symbol
												// See ARM Compiler User Guide 5.x

#endif // ! K_INC_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
