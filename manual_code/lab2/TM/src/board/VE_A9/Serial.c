 /**************************************************************************//**
 * @file     Serial.c
 * @brief    Simple polled UART driver
 * @version
 * @date     27 October 2015
 *
 * @note
 *
 *  Code modification Copyright (c) Yiqing Huang
 *
 *  All rights reserved.
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

#include "Serial.h"

void SER_Init(void)
{
    SER_Disable();  // Disable UART
    SER_Set_baud_rate( 38400 );
    UART0->UARTLCR_H = UART_LCRH_WLEN_8 | UART_LCRH_FEN; // 8 bits, 1 stop bit, no parity, FIFO enabled
    SER_Enable();   // Enable UART and enable TX/RX
}

/*----------------------------------------------------------------------------
  Enable Serial Port
 *----------------------------------------------------------------------------*/
void SER_Enable(void)
{
    UART0->UARTCR = UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE;
}

/*----------------------------------------------------------------------------
  Disable Serial Port
 *----------------------------------------------------------------------------*/
void SER_Disable(void)
{
    UART0->UARTCR = 0x0;
}

/*----------------------------------------------------------------------------
  Set baud rate
 *----------------------------------------------------------------------------*/
void SER_Set_baud_rate(uint32_t baud_rate)
{
    uint32_t divider;
    uint32_t mod;
    uint32_t fraction;

    /*
     * Set baud rate
     *
     * IBRD = UART_CLK / (16 * BAUD_RATE)
     * FBRD = ROUND((64 * MOD(UART_CLK,(16 * BAUD_RATE))) / (16 * BAUD_RATE))
     */
    divider   = UART0_CLK / (16 * baud_rate);
    mod       = UART0_CLK % (16 * baud_rate);
    fraction  = (((8 * mod) / baud_rate) >> 1) + (((8 * mod) / baud_rate) & 1);

    UART0->UARTIBRD = divider;
    UART0->UARTFBRD = fraction;
}

/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
void SER_PutChar(char c)
{
    while (UART0->UARTFR & 0x20);   // Wait for UART TX to become free
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
    while (UART0->UARTFR & 0x10);   // Wait for a character to arrive
    return UART0->UARTDR;
}

/*----------------------------------------------------------------------------
  Serial UART interrupt handler
 *----------------------------------------------------------------------------*/
void interrupt_SER(void)
{
    //your code here
}

