 /**************************************************************************//**
 * @file     Serial.h
 * @brief    Simple polled UART driver, modified for DE1_SoC
 * @version  V1.2021.01
 * @date     28 January 2021
 * @author   Yiqing Huang, Zehan Gao, ARM
 *
 * @note
 *
 ******************************************************************************/
/* Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#ifndef SERIAL_H_
#define SERIAL_H_

typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __int64 uint64_t;

#define   __RO     volatile const
#define   __WO     volatile
#define   __RW     volatile

/* UART - Register Layout Typedef */
typedef struct {
  __RW uint32_t UARTDR;             /*0x000 Data Register*/
  __RW uint32_t UARTIER_DLH;		/*0x004 Interrupt Enable and Divisor Latch High Register */
  __RW uint32_t UART_IIR_FCR;        /*0x008 */
  __RW uint32_t UARTLCR;			/*0x00C Line Control Register */
  uint32_t RESERVED_0;				/*0x010 */
  __RO uint32_t UARTLSR;			/*0x014 Line Status Register */
  __RO uint32_t UARTFR;             /*0x018 Flag Register*/
} UART_Type;




#define UART0_BASE      (0xFFC02000u)  /* Cyclone V datasheet */
#define UART0           ((UART_Type *)UART0_BASE)

#define UART0_CLK       100000000 // L4_SP = 100MHz

/* HYQS */
#define BIT(X)    ( 1 << (X) )
#define NULL 0
#define uart_init(n)			SER_Init()
#define uart_get_char(n)		SER_GetChar()
#define uart_put_char(n, c)		SER_PutChar(c)
#define uart_put_string(n, s)	SER_PutStr(s)

#define uart0_init()        	uart_init(0)
#define uart0_get_char()    	uart_get_char(0)
#define uart0_put_char(c)   	uart_put_char(0,c)
#define uart0_put_string(s) 	uart_put_string(0,s)

#define uart1_init()        	uart_init(1)
#define uart1_get_char()    	uart_get_char(1)
#define uart1_put_char(c)   	uart_put_char(1,c)
#define uart1_put_string(s) 	uart_put_string(1,s)

/* HYQE */


extern void SER_Init(void);
extern void SER_Enable(void);
extern void SER_Disable(void);
extern char SER_GetChar (void);
extern void SER_PutChar(char c);
extern void SER_Set_baud_rate(uint32_t baud_rate);
extern void interrupt_SER(void);

/* HYQS */
extern int SER_PutStr(char *s);
extern void putc(void *p, char c);     /* call back function for printf, use uart1 */
/* HYQE */
#endif /* SERIAL_H_ */
