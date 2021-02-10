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
#include <stdbool.h>
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
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

U32* k_alloc_k_stack(task_t tid)
{
    return g_k_stacks[tid+1];
}

U32* k_alloc_p_stack(task_t tid)
{
    return g_p_stacks[tid+1];
}

typedef struct {
    int size;
    int magic;
} header_t;

struct node_t {
    int size;
    bool allocated;
    struct node_t *next;
    struct node_t *prev;
    int padding;
}* node_head = NULL;

int k_mem_init(void) {
    // initializes the RTX's memory manager
    // appropriate data structure to easily track the free and allocated regions (linked list?)
    // returns 0 (RTX_OK) on success and -1 (RTX_ERR) on failure (no free space in physical memory)
    unsigned int end_addr = (unsigned int) &Image$$ZI_DATA$$ZI$$Limit;
#ifdef DEBUG_0
    printf("k_mem_init: image ends at 0x%x\r\n", end_addr);
    printf("k_mem_init: RAM ends at 0x%x\r\n", RAM_END);
    //printf("diff: %d\r\n", 0xBFFFFFFF - end_addr);
    //printf("size: %d\r\n", sizeof(node_t));
#endif /* DEBUG_0 */
    // sanity check (first have to check if free space is zero
    if (0xBFFFFFFF - end_addr <= 0) {
        return RTX_ERR;
    }
    // the head of the linked list should point to the end_addr
    // the size of the head of the linked list will be the entire free memory since on init there will only be one large chunk
    // the linked list will be of a single node until we start allocating and splitting the chunk so the next node will be NULL
    struct node_t* head = (struct node_t*) end_addr;
    head -> size = (0xBFFFFFFF - end_addr) - sizeof(struct node_t);
    head -> next = NULL;
    head -> prev = NULL;
    head -> allocated = false;
    head -> padding = 0;
    node_head = head;
    return RTX_OK;
}

void* k_mem_alloc(size_t size) {
    // allocates size bytes according to first-fit algorithm
    // size - number of bytes requested from the memory manager
    // returns a pointer to the allocated memory or NULL if the request fails (RTX cannot allocate the requested memory for whatever reason)
    // allocated memory is not initialized
    // memory requests may be of any size
#ifdef DEBUG_0
    printf("k_mem_alloc: requested memory size = %d\r\n", size);
#endif /* DEBUG_0 */
    // iterate to find first memory block with sufficient size
    // start at the head of the memory manager
    struct node_t *p = node_head;
    while(p != NULL) {
        // printf("addr: 0x%x chunk size: %d allocated: %s\r\n", (U32)p, p->size, p->allocated ? "true" : "false");
        if (size + sizeof(struct node_t) <= p->size && !p->allocated) {
            break;
        }
        p = p->next;
    }
    // exited loop, so found sufficient chunk or p is NULL
    if (p == NULL) {
        return NULL;
    }
    // first split the chunk
    // change the size to the new allocated size
    // point to a new node with the remaining size (this will be the free chunk)
    // note that the memory address are 4 bytes aligned
    int padding = size % 4 == 0 ? 0 : 4 - (size % 4);
    unsigned int newAddr = (unsigned int) p + sizeof(struct node_t) + size + padding;
    struct node_t *n = (struct node_t*) newAddr;
    n-> size = p-> size - size - sizeof(struct node_t);
    n-> next = p-> next;
    n-> allocated = false;
    n-> prev = p;
    n-> padding = 0;

    p-> size = size;
    p-> next = n;
    p-> allocated = true;
    p-> padding = padding;
    // increment the pointer such that the return value will be pointing directly to the memory region instead of the header of the node
    return ++p;
}

int k_mem_dealloc(void *ptr) {
#ifdef DEBUG_0
    printf("k_mem_dealloc: freeing 0x%x\r\n", (U32) ptr);
#endif /* DEBUG_0 */
    // free dynamic memory pointed to by ptr (returned by a previous call to mem_alloc())
    // if ptr is NULL no action is performed
    if (ptr == NULL) {
        return NULL;
    }
    // check if the ptr is actually pointing to an allocated memory region
    // decrement the point so now we are pointing to the memory address of the header of the node instead of the memory region
    struct node_t *p = (struct node_t*)ptr-1;
    if (!p->allocated) {
        // if allocated is false that means that ptr was not returned by a previous mem_alloc() call or has already been previously called by mem_dealloc()
        return RTX_ERR;
    }
    // "free" the dynamic memory
    p-> allocated = false;

    // check if memory region is adjacent to the other free memory regions
    // can do this using our doubly linked list
    // first check the next node to see if it has been allocated
    // printf("next chunk addr: 0x%x chunk size: %d allocated: %s\r\n", (U32)p-> next, p-> next->size, p-> next->allocated ? "true" : "false");
    if (p-> next != NULL && !p-> next-> allocated) {
        // if next ptr is pointing at a free memory region, I can merge the two regions
        // essentially deleting the next node and merging it with p node
        // have to account for padding
        // can simply subtract memory addresses to get the byte difference and set that as the size
        p-> size += p-> next-> size + sizeof(struct node_t) + p-> padding + p-> next-> padding;
        p-> padding = 0;
        // p-> next becomes what the next node was pointing at
        p-> next = p-> next-> next;
        // also have to update the next node's previous node to point back to p
        p-> next-> prev = p;
    }
    // then check the previous node to see if it has been allocated
    // printf("prev chunk addr: 0x%x chunk size: %d allocated: %s\r\n", (U32)p-> prev, p-> prev->size, p-> prev->allocated ? "true" : "false");
    if (p-> prev != NULL && !p-> prev-> allocated) {
        // if the prev ptr is pointing to a free memory region, then I can merge the two regions
        // will be deleting the ptr node and merging it with its previous
        // have to account for padding
        //struct node_t * pr = p-> prev;
        p-> prev-> size += p-> size + sizeof(struct node_t) + p-> padding + p-> prev-> padding;
        p-> prev-> padding = 0;
        p-> prev-> next = p-> next;
        // since we merged this node, we have to also update the following node as well
        p-> next-> prev = p-> prev;
    }
    return RTX_OK;
}

int k_mem_count_extfrag(size_t size) {
#ifdef DEBUG_0
    printf("k_mem_extfrag: size = %d\r\n", size);
#endif /* DEBUG_0 */
    // count externally-fragmented memory regions
    // counts the number of free (unallocated) memory regions that are of size strictly less than size (in bytes)
    // can simply start at the head of the node and increment a counter every time the node->size is less than size
    int counter = 0;
    struct node_t *p = node_head;
    while(p != NULL) {
        // check if chunk is unallocated and its size is less than the threshold
        // printf("addr: 0x%x chunk size: %d allocated: %s\r\n", (U32)p, p->size, p->allocated ? "true" : "false");
        if ((p->size + sizeof(struct node_t)) < size && !p-> allocated) {
            // update the counter
            ++counter;
        }
        // move to the next node
        p = p->next;
    }
    // loop runs until we reach the end p-> next is NULL and return the resultant counter
    return counter;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
