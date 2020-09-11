/**
 * @brief:  main_svc.c, demonstrate svc as a gateway to os functions
 * @author: Yiqing Huang
 * @date:   2020/09/09
 * NOTE: Standard C library is not allowed in the final kernel code.
 *       A tiny printf function for embedded application development
 *       taken from http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
 *       and is configured to use UART1 to output.
 */

#include <LPC17xx.h>
#include "rtx.h"
#include "printf.h"
#include "uart_polling.h"


#ifdef RAM_TARGET
#define IROM_BASE  0x10000000
#else
#define IROM_BASE  0x0
#endif

int main()
{
   
  U32 ret_val = 1234;
	void *ptr = NULL;

  SystemInit();  /* initialize the system */
  __disable_irq();
  uart_init(1);  /* uart1 uses polling for output */
  init_printf(NULL, putc);
  __enable_irq();
  
  
	printf("Dereferencing Null to get inital SP = 0x%x\r\n", *(U32 *)(IROM_BASE));
	printf("Derefrencing Reset vector to get intial PC = 0x%x\r\n", *(U32 *)(IROM_BASE + 4));
  printf("We are at privileged level, so we can access SP.\r\n"); 
	printf("Read MSP = 0x%x\r\n", __get_MSP());
	printf("Read PSP = 0x%x\r\n", __get_PSP());
	
	/* transit to unprivileged level, default MSP is used */
  __set_CONTROL(__get_CONTROL() | BIT(0));
  printf("We are at unprivileged level, we cannot access SP.\r\n");
	printf("Cannot read MSP = 0x%x\r\n", __get_MSP());
	printf("Cannot read PSP = 0x%x\r\n", __get_PSP());

	ret_val = mem_init(4, FIRST_FIT);
	ptr = mem_alloc(40);
	mem_dealloc(ptr);
	mem_count_extfrag(12);
  
  /* printf has been retargeted to use the UART1,
     check putc function in uart_polling.c.
  */
  printf("The ret_val=%d\r\n",ret_val); 
  return 0;  
}
