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

int test_coales(void) {
	U32 result = 0;
	if (countNodes()==1){
	    	result |= BIT(0);
	    }

	void *p[4];

	p[0] = mem_alloc(12);
	p[1] = mem_alloc(24);
	p[2] = mem_alloc(48);

    if (countNodes()==4){
    	result |= BIT(1);
    }

    mem_dealloc(p[1]);
    if (countNodes()==4){
		result |= BIT(2);
	}

    mem_dealloc(p[0]);
	if (countNodes()==3){
		result |= BIT(3);
	}

	mem_dealloc(p[2]);
		if (countNodes()==1){
			result |= BIT(4);
	}

	return result == 31;
}

// Test 4:
// Alloc(12) -> p0
// Alloc(12) -> p1
// Alloc(12) -> p2
// Alloc(12) -> p3
// Alloc(16) -> p4
// Alloc(12) -> p5
// Alloc(18) -> p6

// Assert count regions = 8

// Dealloc(p2)
// Alloc(15) -> p7
// Assert count regions = 9
// Dealloc(p4)
// Alloc(18) -> p8
// Assert count regions = 10

// Count mem ext frag(12) = 0
// Count mem ext frag(13) = 1
// Count mem ext frag(17) = 2

int test4(void){
	U32 result = 0;
	if (countNodes() == 1){
		result |= BIT(0);
	}

	void *p[10];

	p[0] = mem_alloc(12);
	p[1] = mem_alloc(12);
	p[2] = mem_alloc(12);
	p[3] = mem_alloc(12);
	p[4] = mem_alloc(16);
	p[5] = mem_alloc(12);
	p[6] = mem_alloc(18);

	if (countNodes() == 8){
    	result |= BIT(1);
    }

	mem_dealloc(p[2]);
	p[7] = mem_alloc(15);

	if (countNodes() == 9){
    	result |= BIT(2);
    }

	mem_dealloc(p[4]);
	p[8] = mem_alloc(18);

	if (countNodes() == 10){
    	result |= BIT(2);
    }

	if (mem_count_extfrag(12) == 0){
		result |= BIT(3);
	}

	if (mem_count_extfrag(13) == 1){
		result |= BIT(4);
	}

	if (mem_count_extfrag(17) == 2){
		result |= BIT(5);
	}

	return result == 63;
}

int test_mem(void) {
	printf("test_coales passed: %x\r\n", test_coales());

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
