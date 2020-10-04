/**
 * @brief: uart_irq.h 
 * @author: Yiqing Huang
 * @date: 2020/08/03
 */

#ifndef UART_IRQ_H_
#define UART_IRQ_H_

/* typedefs */
#include <stdint.h>	
#include "uart_def.h"

/* initialize the n_uart to use interrupt */
int uart_irq_init(int n_uart);		

#endif /* ! UART_IRQ_H_ */
