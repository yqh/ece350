/**
 * @brief timer.h - Timer header file
 * @author Y. Huang
 * @date 2013/02/12
 */
#ifndef _TIMER_H_
#define _TIMER_H_
#include <LPC17xx.h>

extern int reset_counter(uint8_t n_timer);
extern int reset_start_counter(uint8_t ntimer);
extern int start_counter(uint8_t n_timer);
extern int stop_counter(uint8_t n_timer);

extern uint32_t timer_init ( uint8_t n_timer );  /* initialize timer n_timer PCLK */
extern uint32_t timer_init_100MHZ(uint8_t n_timer);
#endif /* ! _TIMER_H_ */
