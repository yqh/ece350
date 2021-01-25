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
 * @file        k_mem.c
 * @brief       Kernel Memory Management API C Code
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @note        skeleton code
 *
 *****************************************************************************/

/** 
 * @brief:  k_mem.c kernel API implementations, this is only a skeleton.
 * @author: Yiqing Huang
 */

#include "k_mem.h"
#include "Serial.h"
#ifdef DEBUG_0
#include "printf.h"
#endif  /* DEBUG_0 */


/*
 *==========================================================================
 *                            STRUCTS
 *==========================================================================
 */

struct node {
	int size;
	int isFree;
	struct node *next;
};

/*
 *==========================================================================
 *                            GLOBAL VARIABLES
 *==========================================================================
 */
// kernel stack size, referred by startup_a9.s
const U32 g_k_stack_size = KERN_STACK_SIZE;

// task kernel stacks
U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));
struct node* HEAD = NULL;

/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

int k_mem_init(void) {
    unsigned int end_addr = (unsigned int) &Image$$ZI_DATA$$ZI$$Limit;
#ifdef DEBUG_0
    printf("k_mem_init: image ends at 0x%x\r\n", end_addr);
    printf("k_mem_init: RAM ends at 0x%x\r\n", RAM_END);
#endif /* DEBUG_0 */

    //check if end addr is valid
    int totalSize = 0xBFFFFFFF - end_addr;
    if(totalSize <= 0) {
    	return RTX_ERR;
    }

    // cast end_addr to pointer given in end_addr
    HEAD = (struct node*) end_addr;

    // setup head
    HEAD->size = totalSize - sizeof(struct node);
    HEAD->isFree = 1;
    HEAD->next = NULL;

    return RTX_OK;
}

void* k_mem_alloc(size_t size) {
#ifdef DEBUG_0
    printf("k_mem_alloc: requested memory size = %d\r\n", size);
#endif /* DEBUG_0 */

    // 4 byte align
    if (size % 4 == 0) {
        size = ((int)(size / 4)) * 4;
    } else {
        size = ((int)(size / 4)) * 4 + 4;
    }

    struct node* curr = HEAD;

    while(curr != NULL) {
    	if (size <= curr->size && curr->isFree) {
    		break;
    	}
    	
    	curr = curr->next;
    }

    // couldn't allocate since no free space
    if (curr == NULL) {
    	return RTX_ERR;
    }
    //make a new node
    // might need to use an unsigned depending on how types work
    struct node* newNode = (struct node_t*)((unsigned int)curr + sizeof(struct node) + size); 
    newNode->isFree = 1;
    newNode->size = curr->size-size-sizeof(struct node);
    newNode->next = NULL;

    curr->isFree=0;
    curr->size = size;
    curr->next = newNode;

    // return pointer to new memory?
    // return curr
    return RTX_OK;
}

int k_mem_dealloc(void *ptr) {
#ifdef DEBUG_0
    printf("k_mem_dealloc: freeing 0x%x\r\n", (U32) ptr);
#endif /* DEBUG_0 */
    return RTX_OK;
}

int k_mem_count_extfrag(size_t size) {
#ifdef DEBUG_0
    printf("k_mem_extfrag: size = %d\r\n", size);
#endif /* DEBUG_0 */
    return RTX_OK;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
