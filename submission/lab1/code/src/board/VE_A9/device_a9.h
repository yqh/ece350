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
 * @file        device_a9.h
 * @brief       Cortex-A9 device header file
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 *****************************************************************************/

#ifndef DEVICE_A9_H_
#define DEVICE_A9_H_

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */

#define NUM_PRIV_MODES  0x00000006      // 6 privileged modes
#define STACK_SZ        0x00000200      // 512 B stack for each mode
#define RAM_SIZE        0x40000000      // The VE9 has 2G RAM, we only use 1G
#define RAM_END         0xFFFFFFFF      // The VE9 RAM END

#endif
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
