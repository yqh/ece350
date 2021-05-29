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
 * @brief       memory lab demo tests
 *
 * @version     V1.2021.05
 * @authors     Yiqing Huang
 * @date        2021 MAY
 * @details     result is a 32 bit integer. bit[n] set means test n passed
 *              bit[n] cleared means test n failed. 
 *              There are 8 tests in this example. 
 *
 *****************************************************************************/
 /* 
one passed output is:
-----------------------------
0 free memory block(s) found
0x2007c000: 0x4000
1 free memory block(s) found
0x2007e000: 0x2000
1 free memory block(s) found
0 free memory block(s) found
END: 8 cases, result = 0xff
-----------------------------

Note the free memory addresses dumped might be differrent 
since it is implementation dependent.
We look at the last line result number. 
If it is 0xff, it means everyting passed.
The template project without any real implementation produces:

END: 8 cases, result = 0x4c.
*/
#include "rtx.h"
#include "uart_polling.h"
#include "printf.h"

int test_mem(void) {
    static void *p[4];
    
    for ( int i = 0; i < 4; i++ ) {
        p[i] = (void *)0x1;
    }

    U32 result = 0;

    p[0] = mem_alloc(0x800);

    if (p[0] != NULL) {
        result |= BIT(0);
    }

    p[1] = mem_alloc(0x800);

    if (p[1] != NULL && p[1] != p[0]) {
        result |= BIT(1);
    }

    p[2] = mem_alloc(0x800);
    if  (p[2] == NULL) {
        result |= BIT(2);
    }
    
    if (mem_dump() == 0 ) {
        result |=BIT(3);
    }
    
    mem_dealloc(p[0]);
    p[0] = NULL;
    if ( mem_dump() == 1 ) {
        result |= BIT(4);
    }
    
    p[0] = mem_alloc(0x400);
    
    if ( mem_dump() == 1 ) {
        result |= BIT(5);
    }
    
    p[2] = mem_alloc(0x400 - 2);
    
    if ( mem_dump() == 0 ) {
        result |= BIT(6);
    }
    
    if (p[0] != p[2] ) {
        result |= BIT(7);
    }
    
    printf("END: 8 cases, result = 0x%x\r\n", result);
    return result;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
