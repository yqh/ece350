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

#include "k_inc.h"
#include "k_mem.h"

/*---------------------------------------------------------------------------
The memory map of the OS image may look like the following:
                   RAM1_END-->+---------------------------+ High Address
                              |                           |
                              |                           |
                              |       MPID_IRAM1          |
                              |   (for user space heap  ) |
                              |                           |
                 RAM1_START-->|---------------------------|
                              |                           |
                              |  unmanaged free space     |
                              |                           |
&Image$$RW_IRAM1$$ZI$$Limit-->|---------------------------|-----+-----
                              |         ......            |     ^
                              |---------------------------|     |
                              |                           |     |
                              |---------------------------|     |
                              |                           |     |
                              |      other data           |     |
                              |---------------------------|     |
                              |      PROC_STACK_SIZE      |  OS Image
              g_p_stacks[2]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[1]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |                           |  OS Image
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |     |                
             g_k_stacks[15]-->|---------------------------|     |
                              |                           |     |
                              |     other kernel stacks   |     |                              
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |  OS Image
              g_k_stacks[2]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |                      
              g_k_stacks[1]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |
              g_k_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |---------------------------|     |
                              |        TCBs               |  OS Image
                      g_tcbs->|---------------------------|     |
                              |        global vars        |     |
                              |---------------------------|     |
                              |                           |     |          
                              |                           |     |
                              |        Code + RO          |     |
                              |                           |     V
                 IRAM1_BASE-->+---------------------------+ Low Address
    
---------------------------------------------------------------------------*/ 

/*
 *===========================================================================
 *                            GLOBAL VARIABLES
 *===========================================================================
 */
// kernel stack size, referred by startup_a9.s
const U32 g_k_stack_size = KERN_STACK_SIZE;
// task proc space stack size in bytes, referred by system_a9.c
const U32 g_p_stack_size = PROC_STACK_SIZE;

// task kernel stacks
U32 g_k_stacks[MAX_TASKS][KERN_STACK_SIZE >> 2] __attribute__((aligned(8)));

// task process stack (i.e. user stack) for tasks in thread mode
// remove this bug array in your lab2 code
// the user stack should come from MPID_IRAM2 memory pool
//U32 g_p_stacks[MAX_TASKS][PROC_STACK_SIZE >> 2] __attribute__((aligned(8)));
U32 g_p_stacks[NUM_TASKS][PROC_STACK_SIZE >> 2] __attribute__((aligned(8)));
/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

/* note list[n] is for blocks with order of n */
mpool_t k_mpool_create (int algo, U32 start, U32 end)
{
    mpool_t mpid = MPID_IRAM1;

#ifdef DEBUG_0
    printf("k_mpool_init: algo = %d\r\n", algo);
    printf("k_mpool_init: RAM range: [0x%x, 0x%x].\r\n", start, end);
#endif /* DEBUG_0 */    
    
    if (algo != BUDDY ) {
        errno = EINVAL;
        return RTX_ERR;
    }
    
    if ( start == RAM1_START) {
        // add your own code
    } else if ( start == RAM2_START) {
        mpid = MPID_IRAM2;
        // add your own code
    } else {
        errno = EINVAL;
        return RTX_ERR;
    }
    
    return mpid;
}

void *k_mpool_alloc (mpool_t mpid, size_t size)
{
#ifdef DEBUG_0
    printf("k_mpool_alloc: mpid = %d, size = %d, 0x%x\r\n", mpid, size, size);
#endif /* DEBUG_0 */
    
    return NULL;
}

int k_mpool_dealloc(mpool_t mpid, void *ptr)
{
#ifdef DEBUG_0
    printf("k_mpool_dealloc: mpid = %d, ptr = 0x%x\r\n", mpid, ptr);
#endif /* DEBUG_0 */
    
    return RTX_OK; 
}

int k_mpool_dump (mpool_t mpid)
{
#ifdef DEBUG_0
    printf("k_mpool_dump: mpid = %d\r\n", mpid);
#endif /* DEBUG_0 */
    
    return 0;
}
 
int k_mem_init(int algo)
{
#ifdef DEBUG_0
    printf("k_mem_init: algo = %d\r\n", algo);
#endif /* DEBUG_0 */
        
    if ( k_mpool_create(algo, RAM1_START, RAM1_END) < 0 ) {
        return RTX_ERR;
    }
    
    if ( k_mpool_create(algo, RAM2_START, RAM2_END) < 0 ) {
        return RTX_ERR;
    }
    
    return RTX_OK;
}

/**
 * @brief allocate kernel stack statically
 */
U32* k_alloc_k_stack(task_t tid)
{
    
    if ( tid >= MAX_TASKS) {
        errno = EAGAIN;
        return NULL;
    }
    U32 *sp = g_k_stacks[tid+1];
    
    // 8B stack alignment adjustment
    if ((U32)sp & 0x04) {   // if sp not 8B aligned, then it must be 4B aligned
        sp--;               // adjust it to 8B aligned
    }
    return sp;
}

/**
 * @brief allocate user/process stack statically
 * @attention  you should not use this function in your lab
 */

U32* k_alloc_p_stack(task_t tid)
{
    if ( tid >= NUM_TASKS ) {
        errno = EAGAIN;
        return NULL;
    }
    
    U32 *sp = g_p_stacks[tid+1];
    
    
    // 8B stack alignment adjustment
    if ((U32)sp & 0x04) {   // if sp not 8B aligned, then it must be 4B aligned
        sp--;               // adjust it to 8B aligned
    }
    return sp;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

