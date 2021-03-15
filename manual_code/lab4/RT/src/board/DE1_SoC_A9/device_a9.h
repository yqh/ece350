/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *              Copyright 2020-2021 Yiqing Huang and Zehan Gao
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
 * @authors     Yiqing Huang, Zehan Gao
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
#define RAM_SIZE        0x40000000      // The DE1 has 1G RAM
#define RAM_END         0x3FFFFFFF      // The DE1 RAM END

#endif
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
