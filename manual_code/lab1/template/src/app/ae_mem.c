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

int test_mem(void) {
	U32 result = 0;

	if(test_mem_leak()){
		result |= BIT(0);
	}
	if(test_coales()){
		result |= BIT(1);
	}
	if(test_reuse_freed()){
		result |= BIT(2);
	}
	if(test_reuse_freed_2()){
		result |= BIT(3);
	}
	if(test_malloc_new_node()){
		result |= BIT(4);
	}

	return result == 31;
}



/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

