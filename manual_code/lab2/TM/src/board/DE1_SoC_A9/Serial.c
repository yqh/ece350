 /**************************************************************************//**
 * @file     Serial.c
 * @brief    Simple polled UART driver, modified for DE1_SoC
 * @version  V1.2021.01
 * @date     28 January 2021
 * @author   Yiqing Huang, Zehan Gao, ARM
 *
 * @note     
 *
 ******************************************************************************/
/* Copyright (c) 2011 - 2015 ARM LIMITED

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

#include "../DE1_SoC_A9/Serial.h"

void SER_Init(void)
{

    SER_Set_baud_rate( 115200 );



    UART0->UARTLCR = 0x3; // reset DLAB, 8 bits
    UART0->UART_IIR_FCR &= 0x1; //FIFO enabled
}

/*----------------------------------------------------------------------------
  Enable Serial Port
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Set baud rate
 *----------------------------------------------------------------------------*/
void SER_Set_baud_rate(uint32_t baud_rate)
{
	UART0->UARTLCR |= 0x80;	//set DLAB
	uint32_t divisor = UART0_CLK/(baud_rate * 16);
	UART0->UARTDR = divisor;
	UART0->UART_IIR_FCR = divisor >> 8;
	UART0->UARTLCR &= ~(0x80);
}

/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
void SER_PutChar(char c)
{
    while ((UART0->UARTLSR & 0x20) == 0 || (UART0->UARTLSR & 0x40) == 0);   // Wait for UART TX to become free
    //if (c == '\n')
    //{
    //  UART0->UARTDR = '\r';
    //  while (UART0->UARTFR & 0x20);
    //}
    UART0->UARTDR = c;
}

/* HYQS */

/*----------------------------------------------------------------------------
  Write String to Serial Port
 *----------------------------------------------------------------------------*/
int SER_PutStr(char *s)
{
  if (s == NULL)
	  return 1;
  while (*s !=0) {      /* loop through each char in the string */
	  SER_PutChar(*s++);/* print the char, then ptr increments  */
  }
  return 0;
}
/*----------------------------------------------------------------------------
 * Call back function for printf
 *----------------------------------------------------------------------------*/
/**
 * @brief   call back function for printf
 * @note    first parameter p is not used for now. Polling UART is used
 */

void putc(void *p, char c)
{
  if ( p != NULL ) {
    SER_PutStr("putc: first parameter needs to be NULL");
  } else {
      SER_PutChar(c);
  }
}

/* HYQE */

/*----------------------------------------------------------------------------
  Read character from Serial Port (blocking read)
 *----------------------------------------------------------------------------*/
char SER_GetChar (void)
{
    while (UART0->UARTLSR & 0x1 == 0);   // Wait for a character to arrive
    return UART0->UARTDR;
}
/*----------------------------------------------------------------------------
  Serial UART interrupt handler
 *----------------------------------------------------------------------------*/
void interrupt_SER(void)
{
    //your code here
}

