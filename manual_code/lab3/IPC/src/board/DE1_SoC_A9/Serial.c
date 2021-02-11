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

/*----------------------------------------------------------------------------
  Serial Port initialization
 *----------------------------------------------------------------------------*/
void SER_Init(int n)
{
  UART0_Init();
}

/*----------------------------------------------------------------------------
  Write String to Serial Port
 *----------------------------------------------------------------------------*/
int SER_PutStr(int n, char *s)
{
  if (s == NULL)
    return 1;
  while (*s !=0) {      /* loop through each char in the string */
    SER_PutChar(n, *s++);/* print the char, then ptr increments  */
  }
  return 0;
}

/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
void SER_PutChar(int n, char c)
{
  if(n == 0){
    JTAG_UART_PutChar(c);
  }
  else if(n == 1){
    UART0_PutChar(c);
  }
}

/*----------------------------------------------------------------------------
  Read character from Serial Port (blocking read)
 *----------------------------------------------------------------------------*/
char SER_GetChar(int n){
  if(n == 0){
    return JTAG_UART_GetChar();
  }
  else if(n == 1){
    return UART0_GetChar();
  }
  return '\0';
}

/*----------------------------------------------------------------------------
  UART0 initialization
 *----------------------------------------------------------------------------*/
void UART0_Init(void)
{
  UART0_Set_baud_rate( 115200 ); 	// set baud rate to 115200
  UART0->UARTLCR |= 0x3; 		// 8 bits
  UART0->UART_IIR_FCR &= 0x1; 	        //FIFO enabled
}

/*----------------------------------------------------------------------------
  Set baud rate
 *----------------------------------------------------------------------------*/
void UART0_Set_baud_rate(uint32_t baud_rate)
{
  UART0->UARTLCR |= 0x80;	                      // set DLAB to enable setup of baud rate
  uint32_t divisor = UART0_CLK/(baud_rate * 16);      // calculate the divisor value according to the datasheet
  UART0->UARTDR = divisor;			      // the lower 8 bit are written to Data Register
  UART0->UART_IIR_FCR = divisor >> 8;		      // the higher 8 bit are written to IIR
  UART0->UARTLCR &= ~(0x80);	                      // clear DLAB bit and return to normal
}

/*----------------------------------------------------------------------------
  Write character to UART0 (PuTTY)
 *----------------------------------------------------------------------------*/
void UART0_PutChar(char c)
{
  while ((UART0->UARTLSR & 0x20) == 0 || (UART0->UARTLSR & 0x40) == 0);   // Wait for UART TX to become free
  UART0->UARTDR = c;
}


/*----------------------------------------------------------------------------
  Read character from UART0 (PuTTY) (blocking read)
 *----------------------------------------------------------------------------*/
char UART0_GetChar (void)
{
  while (UART0->UARTLSR & 0x1 == 0);                // Wait for a character to arrive
  return UART0->UARTDR;
}

/*----------------------------------------------------------------------------
 * Call back function for printf (using JTAG UART)
 *----------------------------------------------------------------------------*/
/**
 * @brief   call back function for printf
 * @note    first parameter p is not used for now. Polling UART is used
 */

void putc(void *p, char c)
{
  if ( p != NULL ) {
    SER_PutStr(0,"putc: first parameter needs to be NULL");
  } else {
    SER_PutChar(0,c);
  }
}

/*----------------------------------------------------------------------------
  Write character to JTAG
 *----------------------------------------------------------------------------*/
void JTAG_UART_PutChar(char c)
{
  while(1)
  {
    if(JTAG_UART->control & 0xFFFF0000)
    {
      JTAG_UART->data = c;
      return;
    }
  }
}

/*----------------------------------------------------------------------------
  Read character from JTAG UART (blocking read)
 *----------------------------------------------------------------------------*/
char JTAG_UART_GetChar(void)
{
  while(1)
  {
    int data;
    data = JTAG_UART->data;
    if(data & 0x00008000)
      return ((char)data & 0xFF);
  }
}

/*----------------------------------------------------------------------------
  Serial UART interrupt handler (UART0)
 *----------------------------------------------------------------------------*/
/*
void SER_Interrupt(void)
{
  int n = UART0->UART_IIR_FCR;	            // read UART0 pending interrupt type
  if((n & 0xF) == 0x4)			    // type 0x4 is receive data
  {
    while(UART0->UARTLSR & 0x1 == 0x1)	    // read while Data Ready is valid
    {
      char c = UART0->UARTDR;		    // would also clear the interrupt if last character is read
      UART0_PutChar(c);			    // display back
    }
  }
  else{                                     // unexpected interrupt type
    SER_PutStr(1, "Error interrupt type\n");
  }
}
*/

/*----------------------------------------------------------------------------
  Config UART0 IRQ
 *----------------------------------------------------------------------------*/
void config_UART0_irq(void)
{
  UART0->UARTIER_DLH |= 0x1;  //enable rx interrupt
}

int Interrupt_Rx(void)
{
  int n = UART0->UART_IIR_FCR;
  if((n & 0xF) == 0x4)
    return 1;
  return 0;
}

int Rx_Data_Ready(void)
{
	return UART0->UARTLSR & 0x1;
}

char Rx_Read_Data(void)
{
	return UART0->UARTDR & 0xFF;
}
