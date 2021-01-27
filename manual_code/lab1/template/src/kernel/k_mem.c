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

typedef struct Node {
	unsigned int size;
	int isFree;
	struct Node *next;
} Node;

/*
 *==========================================================================
 *                            GLOBAL VARIABLES
 *==========================================================================
 */
// kernel stack size, referred by startup_a9.s
const U32 g_k_stack_size = KERN_STACK_SIZE;

// task kernel stacks
U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));
Node* HEAD = NULL;

/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

void print_list() {
	Node* curr = HEAD;
	while (curr) {
		printf("location: 0x%x, size: %x, free: %x\r\n", (U32)curr, (U32)curr->size, curr->isFree);
		curr = curr->next;
	}
}

int k_mem_init(void) {
    unsigned int end_addr = (unsigned int) &Image$$ZI_DATA$$ZI$$Limit;
#ifdef DEBUG_0
    printf("k_mem_init: image ends at 0x%x\r\n", end_addr);
    printf("k_mem_init: RAM ends at 0x%x\r\n", RAM_END);
#endif /* DEBUG_0 */

    //check if end addr is valid
    unsigned int totalSize = 0xBFFFFFFF - end_addr;
    if(totalSize <= 0) {
    	return RTX_ERR;
    }

    // round end_addr to nearest 4
    if (end_addr % 4 != 0) {
        end_addr = ((unsigned int)(end_addr / 4)) * 4 + 4;
    }

    // cast end_addr to pointer given in end_addr
    HEAD = (Node*) end_addr;

    // setup head
    HEAD->size = totalSize - sizeof(Node);
    HEAD->isFree = 1;
    HEAD->next = NULL;

    return RTX_OK;
}

void* k_mem_alloc(size_t size) {
#ifdef DEBUG_0
    printf("k_mem_alloc: requested memory size = %d\r\n", size);
#endif /* DEBUG_0 */

    if (size == 0) {
        return NULL;
    }
    
    // 4 byte align
    if (size % 4 != 0) {
        size = ((unsigned int)(size / 4)) * 4 + 4;
    }

    Node* curr = HEAD;

    while(curr != NULL) {
    	if (size <= curr->size && curr->isFree) {
    		break;
    	}
    	curr = curr->next;
    }

    // couldn't allocate since no free space
    if (curr == NULL) {
    	return NULL;
    }

    if (size == curr->size){
        return (void*)((U32)curr + sizeof(Node));
    } else if (size < curr->size && (curr->size < (size + sizeof(Node)))) {
        return (void*)((U32)curr + sizeof(Node));
    } else {
        //make a new node
        // might need to use an unsigned depending on how types work
        Node* newNode = (Node*)((unsigned int)curr + sizeof(Node) + size);
        newNode->isFree = 1;
        newNode->size = curr->size - size - sizeof(Node);
        newNode->next = NULL;

        curr->isFree=0;
        curr->size = size;
        curr->next = newNode;
        // Cast curr to U32 to ensure pointer arithmetic works
        // Pointer addition works by adding by increment of sizeof the pointer
        // argument. If curr is of type Node* and we add sizeof(Node), we add
        // sizeof(Node)^2 amount of bytes.
        return (void*)((U32)curr + sizeof(Node));
    }
    // print_list();
}

Node* mergeNode(Node* first, Node* second) {
	if (first > second) {
		return NULL;
	}

	Node* result = first;
	result->size = first->size + second->size;
	result->next = second->next;

	return result;
}

int k_mem_dealloc(void *ptr) {
#ifdef DEBUG_0
    printf("k_mem_dealloc: freeing 0x%x\r\n", (U32) ptr);
#endif /* DEBUG_0 */

    Node* curr = HEAD;
    Node* prev = NULL;

    while (ptr != (char*)curr + sizeof(Node)) {
    	if (curr->next == NULL) {
    		return RTX_ERR;
    	}

    	prev = curr;
    	curr = curr->next;
//    	printf("0x%x\n", (U32)((char*)curr + sizeof(Node) + curr->size));
    }

    // Double free
    if (curr->isFree > 0) {
    	return RTX_ERR;
    }

    curr->isFree = 1;

    // Merge neighboring nodes
    if (prev && prev->isFree > 0) {
    	curr = mergeNode(prev, curr);
    }

    if (curr->next && curr->next->isFree > 0) {
    	curr = mergeNode(curr, curr->next);
    }

    print_list();

    return RTX_OK;
}

int k_mem_count_extfrag(size_t size) {
#ifdef DEBUG_0
    printf("k_mem_extfrag: size = %d\r\n", size);
#endif /* DEBUG_0 */
    // return RTX_OK;

    unsigned int memRegionSize;
    int regionCount = 0;

    Node* curNode = HEAD; // HEAD is global var

    while(curNode->next != NULL){
        memRegionSize = curNode->size + sizeof(Node);
        if(curNode->isFree){
            if(memRegionSize < size){
                regionCount++;
            }
        }
        curNode = curNode->next; // idk if this is the right way to goto next node
    }

    return regionCount;
}

int countNodes(){
	Node* n = HEAD;
	int ret = 0;
	while(n != NULL) {
		ret += 1;
		n = n->next;
	}
	return ret;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
