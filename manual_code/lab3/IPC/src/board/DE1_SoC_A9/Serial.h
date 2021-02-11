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

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;

#define   __RO     volatile const
#define   __WO     volatile
#define   __RW     volatile

/* UART - Register Layout Typedef */
typedef struct {
  __RW uint32_t UARTDR;             /*0x000 Data Register*/
  __RW uint32_t UARTIER_DLH;	    /*0x004 Interrupt Enable and Divisor Latch High Register */
  __RW uint32_t UART_IIR_FCR;       /*0x008 */
  __RW uint32_t UARTLCR;	    /*0x00C Line Control Register */
  uint32_t RESERVED_0;		    /*0x010 */
  __RO uint32_t UARTLSR;	    /*0x014 Line Status Register */
  __RO uint32_t UARTFR;             /*0x018 Flag Register*/
} UART_Type;

typedef struct {
  uint32_t data;
  uint32_t control;
} JTAG_UART_Type;


#define UART0_BASE                      (0xFFC02000u)  /* UART0 base from Cyclone V datasheet */
#define UART0                           ((UART_Type *)UART0_BASE)

#define JTAG_UART_BASE                  (0xFF201000u)
#define JTAG_UART	                ((JTAG_UART_Type *)JTAG_UART_BASE)

#define UART0_CLK                       100000000 // L4_SP = 100MHz

/* ECE350 START */
#define BIT(X)                          ( 1 << (X) )
#define NULL                            0
/* ECE350 END */

extern void SER_Init(int n);
extern char SER_GetChar (int n);
extern void SER_PutChar(int n, char c);
extern int  SER_PutStr(int n, char *s);

void UART0_Init(void);
void UART0_PutChar(char c);
char UART0_GetChar (void);
void UART0_Set_baud_rate(uint32_t baud_rate);

void JTAG_UART_PutChar(char c);
char JTAG_UART_GetChar(void);

extern void config_UART0_irq(void);
extern int Interrupt_Rx(void);
extern int Rx_Data_Ready(void);
extern char Rx_Read_Data(void);

extern void putc(void *p, char c);     /* call back function for printf, use JTAG UART */

#endif /* SERIAL_H_ */
