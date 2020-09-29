/**
 * @file:   k_mem.c
 * @brief:  kernel memory managment routines
 * @author: Yiqing Huang
 * @date:   2020/09/28
 */

#include "k_mem.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* The final memory map of IRAM1 looks like the following. 
   Refer to k_task.c for initilization function details.
---------------------------------------------------------------------------

                    0x10008000+---------------------------+ High Address
                              |                           |
                              |  Free Memory Space        |
                              |                           |
                              |                           |
                              |                           |
                              |---------------------------|
                              |        Padding            |
&Image$$RW_IRAM1$$ZI$$Limit-->|---------------------------|
                              |                           |          
                              |       RTX  Image          |
                              |                           |
                    0x10000000+---------------------------+ Low Address

*/


int k_mem_init(size_t blk_size, int algo) {
    U32 end_addr = (unsigned int) &Image$$RW_IRAM1$$ZI$$Limit;
#ifdef DEBUG_0
    printf("k_mem_init: blk_size = %d, algo = %d\r\n", blk_size, algo);
    printf("k_mem_init: image ends at 0x%x\r\n", end_addr);
    printf("k_mem_init: IRAM1 ends at 0x%x\r\n", IRAM1_END);
#endif /* DEBUG_0 */
    return 0;
}

void *k_mem_alloc(size_t size) {
#ifdef DEBUG_0
    printf("k_mem_alloc: requested memory size = %d\r\n", size);
#endif /* DEBUG_0 */
    return NULL;
}

int k_mem_dealloc(void *ptr) {
#ifdef DEBUG_0
    printf("k_mem_dealloc: freeing 0x%x\r\n", (U32) ptr);
#endif /* DEBUG_0 */
    return 0;
}

int k_mem_count_extfrag(size_t size) {
#ifdef DEBUG_0
    printf("k_mem_extfrag: size = %d\r\n", size);
#endif /* DEBUG_0 */
    return 0;
}

