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
 * @file        k_mem.c
 * @brief       Kernel Memory Management API C Code
 *
 * @version     V1.2021.01.lab2
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
 *                            GLOBAL VARIABLES
 *==========================================================================
 */
// kernel stack size, referred by startup_a9.s
const U32 g_k_stack_size = KERN_STACK_SIZE;
// task proc space stack size in bytes, referred by system_a9.c
const U32 g_p_stack_size = PROC_STACK_SIZE;

// task kernel stacks
U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));

//process stack for tasks in SYS mode
U32 g_p_stacks[MAX_TASKS][PROC_STACK_SIZE >> 2] __attribute__((aligned(8)));

/*
 *==========================================================================
 *                            STRUCTS
 *==========================================================================
 */

typedef struct Node {
	unsigned int size;
	U8 isFree;
    task_t owner;
	struct Node *next;
} Node;

// Global head
Node* HEAD = NULL;

/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

U32* k_alloc_k_stack(task_t tid)
{
    return g_k_stacks[tid+1];
}

U32* k_alloc_p_stack(task_t tid) {
    void* mem = k_mem_alloc(g_tcbs[tid].u_stack_size);
    Node* node = (Node*)mem - 1;
    node->owner = 0;
    return (U32*) ((char*)mem + g_tcbs[tid].u_stack_size);
}

int k_mem_init(void) {
    unsigned int end_addr = (unsigned int) &Image$$ZI_DATA$$ZI$$Limit;
#ifdef DEBUG_0
   printf("k_mem_init: image ends at 0x%x\r\n", end_addr);
   printf("k_mem_init: RAM ends at 0x%x\r\n", RAM_END);
#endif /* DEBUG_0 */

    //check if end addr is valid
    unsigned int totalSize = RAM_END - end_addr;
    if(totalSize <= 0) {
    	return RTX_ERR;
    }

    // round end_addr to nearest 8
    if (end_addr % 8 != 0) {
        end_addr = ((unsigned int)(end_addr / 8)) * 8 + 8;

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
//#ifdef DEBUG_0
//    printf("k_mem_alloc: requested memory size = %d\r\n", size);
//#endif /* DEBUG_0 */

    if (size == 0) {
        return NULL;
    }

    // 8 byte align
    if (size % 8 != 0) {
        size = ((unsigned int)(size / 8)) * 8 + 8;
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
    
    //TODO: assign TID ownership

    if (size == curr->size){
        curr->owner = gp_current_task->tid;
        curr->isFree=0;
        return (void*)((U32)curr + sizeof(Node));
    } else if (size < curr->size && (curr->size < (size + sizeof(Node)))) {
        curr->owner = gp_current_task->tid;
        curr->isFree=0;
        return (void*)((U32)curr + sizeof(Node));
    } else {
        //make a new node
        // might need to use an unsigned depending on how types work
        Node* newNode = (Node*)((unsigned int)curr + sizeof(Node) + size);
        newNode->isFree = 1;
        newNode->size = curr->size - size - sizeof(Node);
        newNode->next = curr->next;

        curr->isFree=0;
        curr->size = size;
        curr->next = newNode;
        curr->owner = gp_current_task->tid;
        // Cast curr to U32 to ensure pointer arithmetic works
        // Pointer addition works by adding by increment of sizeof the pointer
        // argument. If curr is of type Node* and we add sizeof(Node), we add
        // sizeof(Node)^2 amount of bytes.
        return (void*)((U32)curr + sizeof(Node));
    }
}

Node* mergeNode(Node* first, Node* second) {
	if (first > second) {
		return NULL;
	}

	Node* result = first;
	result->size = first->size + second->size + sizeof(Node);
	result->next = second->next;

	return result;
}

int k_mem_dealloc(void *ptr) {
//#ifdef DEBUG_0
//    printf("k_mem_dealloc: freeing 0x%x\r\n", (U32) ptr);
//#endif /* DEBUG_0 */

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
    
    if (!gp_current_task->priv) {
        if (gp_current_task->tid != curr->owner){
            return RTX_ERR;
        }
    }

    curr->isFree = 1;

    // Merge neighboring nodes
    if (prev && prev->isFree > 0) {
    	curr = mergeNode(prev, curr);
    }

    if (curr->next && curr->next->isFree > 0) {
    	curr = mergeNode(curr, curr->next);
    }

//    print_list();

    return RTX_OK;
}

int k_mem_count_extfrag(size_t size) {
#ifdef DEBUG_0
   printf("k_mem_extfrag: size = %d\r\n", size);
#endif /* DEBUG_0 */


    unsigned int memRegionSize;
    int regionCount = 0;

    Node* curNode = HEAD; // HEAD is global var

    while(curNode != NULL){
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

int memLeakCheck(){
    unsigned int howMuchMem = 0;
    Node* curNode = HEAD;
    while(curNode != NULL){
        howMuchMem += curNode->size + sizeof(Node);
        curNode = curNode->next;
    }
    unsigned int end_addr = (unsigned int) &Image$$ZI_DATA$$ZI$$Limit;
    unsigned int totalSize = 0xBFFFFFFF - end_addr;
    return howMuchMem == totalSize;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
