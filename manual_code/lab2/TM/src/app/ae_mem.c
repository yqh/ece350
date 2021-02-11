/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
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

//TODO test out allocating and de-allocating the same task or a diff task

// test 1
// create tcb
// malloc
//dealloc
// assert pass

// test 2
// create tcb 1
// create tcb 2
// malloc as tcb 1
// switch to tcb 2
// dealloc as tcb 2
// assert rtx error

// test 2
// create tcb 1
// create tcb 2 with privilidge
// malloc as tcb 1
// switch to tcb 2
// dealloc as tcb 2
// assert pass


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

int test_reuse_freed(void) {
	void *p[4];
	U32 result = 0;
	if (countNodes()==1){
		result |= BIT(0);
	}

	p[0] = mem_alloc(12);
	p[1] = mem_alloc(24);
	p[2] = mem_alloc(48);

    if (countNodes()==4){
    	result |= BIT(1);
    }

    mem_dealloc(p[0]);
    if (countNodes()==4){
		result |= BIT(2);
	}

    p[3] = mem_alloc(12);
	if (countNodes()==4){
		result |= BIT(3);
	}

	mem_dealloc(p[1]);
	mem_dealloc(p[2]);
	mem_dealloc(p[3]);
	if (countNodes()==1){
		result |= BIT(4);
	}

	if(memLeakCheck()==1){
		result |= BIT(5);
	}

	return result == 63;
}

int test_reuse_freed_2(void) {
	void *p[4];
	U32 result = 0;
	if (countNodes()==1){
		result |= BIT(0);
	}

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

    p[3] = mem_alloc(20);
	if (countNodes()==4){
		result |= BIT(3);
	}

	mem_dealloc(p[0]);
	mem_dealloc(p[2]);
	mem_dealloc(p[3]);
	if (countNodes()==1){
		result |= BIT(4);
	}

	if(memLeakCheck()==1){
		result |= BIT(5);
	}

	return result == 63;
}

int test_malloc_new_node(void) {
	void *p[4];
	U32 result = 0;
	if (countNodes()==1){
		result |= BIT(0);
	}

	p[0] = mem_alloc(12);
	p[1] = mem_alloc(60);
	p[2] = mem_alloc(48);

    if (countNodes()==4){
    	result |= BIT(1);
    }

	mem_dealloc(p[1]);
    if (countNodes()==4){
		result |= BIT(2);
	}

	p[3] = mem_alloc(12);
	if (countNodes()==5){
		result |= BIT(3);
	}

	mem_dealloc(p[3]);
    if (countNodes()==4){
		result |= BIT(4);
	}

	mem_dealloc(p[0]);
	mem_dealloc(p[2]);
	if (countNodes()==1){
		result |= BIT(5);
	}

	if(memLeakCheck()==1){
		result |= BIT(6);
	}

	return result == 127;
}

int test_mem_leak(){
	void *p[4];
	U32 result = 0;
	if(memLeakCheck()==1){
		result |= BIT(0);
	}

	p[0] = mem_alloc(12);

	if(memLeakCheck()==1){
		result |= BIT(1);
	}

	mem_dealloc(p[0]);

	if(memLeakCheck()==1){
		result |= BIT(2);
	}

	return result == 7;
}

int test_extfrag(void){
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
    	result |= BIT(3);
    }

	if (mem_count_extfrag(12+12) == 0){
		result |= BIT(4);
	}

	if (mem_count_extfrag(13+12) == 1){
		result |= BIT(5);
	}

	if (mem_count_extfrag(17+12) == 2){
		result |= BIT(6);
	}

	mem_dealloc(p[0]);
	mem_dealloc(p[1]);

	mem_dealloc(p[3]);

	mem_dealloc(p[5]);
	mem_dealloc(p[6]);
	mem_dealloc(p[7]);
	mem_dealloc(p[8]);

	return result == 127;
}

//Default test mem
int test_mem_default(void){
    void *p[4];
    int n;

    U32 result = 0;

    p[0] = mem_alloc(8);

    if (p[0] != NULL) {
        result |= BIT(0);
    }

    p[1] = mem_alloc(8);

    if (p[1] != NULL && p[1] != p[0]) {
        result |= BIT(1);
    }

    mem_dealloc(p[0]);
    n = mem_count_extfrag(128);
    if (n == 1) {
        result |= BIT(2);
    }

    mem_dealloc(p[1]);
    n = mem_count_extfrag(128);
    if (n == 0) {
        result |= BIT(3);
    }
    return result;
}

int test_utilization(void) {
	unsigned int totalUsed = 0;
    unsigned int counter = 0;
    unsigned int regionSize = 1048576;
    unsigned int magicSize = 1070585247;

    while(1){
        if (mem_alloc(regionSize) == NULL) {
            break;
        }
        totalUsed += regionSize;
        counter++;
        if(totalUsed > magicSize){
        	return -1;
        }
    }

    unsigned int numAllocs = counter;
    return 0;
}

int test_throughput(void){
	for(int i = 0; i < 200; i++){
		test_reuse_freed_2();
	}
	return 1;
}

int test_mem(void) {
    test_mem_default();
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
