/**
 * @brief: uart_irq.h 
 * @author: Yiqing Huang
 * @date: 2020/10/05
 */

#ifndef UART_IRQ_H_
#define UART_IRQ_H_

#include <stdint.h>	
#include "uart_def.h"
#include "common.h"

/* initialize the n_uart to use interrupt */
int uart_irq_init(int n_uart);	
ssize_t k_uart_read(void *buf, size_t count);
ssize_t k_uart_write(const void *buf, size_t count);

#endif /* ! UART_IRQ_H_ */
