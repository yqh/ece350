/**
 * @brief timer.h - Timer header file
 * @author Y. Huang
 * @date 2020/11/01
 */
#ifndef _AE_TIMER_H_
#define _AE_TIMER_H_
#include <LPC17xx.h>

/* Timer Counter Struct */

struct ae_tick {
    uint32_t tc;
    uint32_t pc;
};


extern uint32_t ae_timer_init_100MHZ(uint8_t n_timer);
extern int ae_get_tick(struct ae_tick *tm, uint8_t n_timer);
    
#endif /* ! _AE_TIMER_H_ */
