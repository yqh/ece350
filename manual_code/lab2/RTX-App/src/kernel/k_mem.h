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
 * @file        k_mem.h
 * @brief       Kernel Memory Management API Header File
 *
 * @version     V1.2021.04
 * @authors     Yiqing Huang
 * @date        2021 APR 
 *
 * @note        skeleton code
 *
 *****************************************************************************/


#ifndef K_MEM_H_
#define K_MEM_H_
#include "k_inc.h"
#include "lpc1768_mem.h"        // board memory map

/*
 * ------------------------------------------------------------------------
 *                             FUNCTION PROTOTYPES
 * ------------------------------------------------------------------------
 */
// kernel API that requires mpool ID
mpool_t k_mpool_create  (int algo, U32 strat, U32 end);
void   *k_mpool_alloc   (mpool_t mpid, size_t size);
int     k_mpool_dealloc (mpool_t mpid, void *ptr);
int     k_mpool_dump    (mpool_t mpid);

int     k_mem_init      (int algo);
U32    *k_alloc_k_stack (task_t tid);
U32    *k_alloc_p_stack (task_t tid);
// declare newly added functions here


/*
 * ------------------------------------------------------------------------
 *                             FUNCTION MACROS
 * ------------------------------------------------------------------------
 */

#endif // ! K_MEM_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

