/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTX LAB  
 *
 *                     Copyright 2020-2021 Yiqing Huang
 *                          All rights reserved.
 *---------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------*/
 

/**************************************************************************//**
 * @brief       AE Timer header file              
 * @version     V1.2021.08
 * @authors     Yiqing Huang
 * @date        2021 Aug
 *****************************************************************************/

#ifndef _AE_TIMER_H_
#define _AE_TIMER_H_
#include <timer.h>

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */
 
#define BIT(X) ( 1 << (X) )

/*
 *===========================================================================
 *                             STRUCTURES
 *===========================================================================
 */
 

/**
 * @brief   data strucure to represent time in seconds and milliseconds
 * @note    Example: to represent 3.004 seconds, set
 *          sec = 3 and msec = 4
 */
struct ae_time {
    uint32_t sec;       /*< seconds             */
    uint32_t msec;      /*< milliseconds        */
};

/*
 *===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================
 */

uint32_t ae_timer_init_100MHZ(uint8_t n_timer);
int      ae_get_tick_diff    (struct ae_time *tm, TM_TICK *tk1, TM_TICK *tk2);
void     ae_spin             (uint32_t msec);

#endif /* ! _AE_TIMER_H_ */

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
