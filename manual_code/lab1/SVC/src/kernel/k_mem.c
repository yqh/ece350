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
#include <stdbool.h>
#include "printf.h"
#endif  /* DEBUG_0 */

/*
 *==========================================================================
 *                            GLOBAL VARIABLES
 *==========================================================================
 */
// kernel stack size, referred by startup_a9.s
const U32 g_k_stack_size = KERN_STACK_SIZE;

// task kernel stacks
U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));

/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

typedef struct {
    int size;
    int magic;
} header_t;

struct node_t {
    int size;
    bool allocated;
    struct node_t *next;
    struct node_t *prev;
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
        if (size <= p->size && !p->allocated) {
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
    unsigned int newAddr = (unsigned int) p + sizeof(struct node_t) + size;
    struct node_t *n = (struct node_t*) newAddr;
    n-> size = p-> size - size - sizeof(struct node_t);
    n-> next = p-> next;
    n-> allocated = false;
    n-> prev = p;

    p-> size = size;
    p-> next = n;
    p-> allocated = true;
    return p;
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
    struct node_t *p = (struct node_t*) ptr;
    if (!p->allocated) {
        // if allocated is false that means that ptr was not returned by a previous mem_alloc() call or has already been previously called by mem_dealloc()
        return RTX_ERR;
    }
    // "free" the dynamic memory
    p-> allocated = false;

    // check if memory region is adjacent to the other free memory regions
    // can do this using our doubly linked list
    // first check the next node to see if it has been allocated
    if (p-> next != NULL && !p-> next-> allocated) {
        // if next ptr is pointing at a free memory region, I can merge the two regions
        // essentially deleting the next node and merging it with p node
        p-> size += p-> next-> size + sizeof(struct node_t);
        // p-> next becomes what the next node was pointing at
        p-> next = p-> next-> next;
    }
    // then check the previous node to see if it has been allocated
    if (p-> prev != NULL && !p-> prev-> allocated) {
        // if the prev ptr is pointing to a free memory region, then I can merge the two regions
        // will be deleting the ptr node and merging it with its previous
        //struct node_t * pr = p-> prev;
        p-> prev-> size += p-> size + sizeof(struct node_t);
        p-> prev-> next = p-> next;
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
        // check if region is unallocated and its size is less than the threshold
        if (p->size < size && !p-> allocated) {
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
