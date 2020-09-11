/**
 * @brief: Display strings to UART0 and UART1 on MCB1700 by polling
 * @author: Yiqing Huang
 * @date: 2020/09/06
 */

#include <LPC17xx.h>
#include "uart_def.h"
#include "uart_polling.h"

int main() {
	SystemInit();
	uart0_init();
	uart1_init();
	uart0_put_string("UART0 - Howdy!\r\n");
	uart1_put_string("UART1 - Hello World!\r\n");
	return 0;
}
