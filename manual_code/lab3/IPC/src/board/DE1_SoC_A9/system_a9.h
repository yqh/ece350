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
 * @file        system_a9.h
 * @brief       Coretx-A9 Generic CMSIS System Initialization
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 * @note
 * @details
 *
 *****************************************************************************/
#ifndef _SYSTEM_A9_H
#define _SYSTEM_A9_H

/*
 *===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================
 */
#ifdef __cplusplus
extern "C" {
#endif


extern void StackInit (void);
extern void SystemInit (void);

#endif /* _SYSTEM_A9_H */
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
