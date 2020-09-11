/** 
 * @brief:  k_mem.c kernel API implementations, this is only a skeleton.
 * @author: Yiqing Huang
 * @date:   2020/09/03
 */

#include "k_mem.h"
#include "uart_polling.h"
#ifdef DEBUG_0
#include "printf.h"
#endif  /* DEBUG_0 */

/* 
   This symbol is defined in the scatter file, 
   refer to ARM Compiler v5.x Linker User Guide
*/  
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

int k_mem_init(size_t blk_size, int algo){
	unsigned int end_addr = (unsigned int) &Image$$RW_IRAM1$$ZI$$Limit;
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

void k_mem_dealloc(void *ptr) {
#ifdef DEBUG_0
	printf("k_mem_dealloc: freeing 0x%x\r\n", (U32) ptr);
#endif /* DEBUG_0 */
	return;
}

int k_mem_count_extfrag(size_t size) {
#ifdef DEBUG_0
	printf("k_mem_extfrag: size = %d\r\n", size);
#endif /* DEBUG_0 */
	return 0;
}
