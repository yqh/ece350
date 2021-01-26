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
 * @file        ae_mem.c
 * @brief       memory lab auto-tester
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 *****************************************************************************/

#include "rtx.h"
#include "Serial.h"
#include "printf.h"

int test_mem(void) {
    void *p[4];
    int n;
    U32 result = 0;
    U32 largeMemVal = 4294967295;

    p[0] = mem_alloc(8);
    n = mem_count_extfrag(largeMemVal);
    if (n == 1) {
        result |= BIT(0);
    }
    return result;
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
