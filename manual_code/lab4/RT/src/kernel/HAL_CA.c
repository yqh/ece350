/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *              Copyright 2020-2021 Yiqing Huang and Zehan Gao
 *                          All rights reserved.
 *
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
 ****************************************************************************
 */

/**************************************************************************//**
 * @file        HAL.c
 * @brief       Hardware Abstraction Layer for Cortex-A series
 * @version     V1.2021.02
 * @authors     Yiqing Huang and Zehan Gao
 * @date        2021 Feb
 *
 * @note        This file contains embedded assembly.
 *              The code borrowed ideas from ARM RTX source code
 *
 *****************************************************************************/
 
#include "common.h"
#include "k_inc.h"
#include "k_HAL_CA.h"
#include "interrupt.h"
#include "Serial.h"
#include "k_task.h"
#include "timer.h"
#include "printf.h"

#pragma push
#pragma arm

/**************************************************************************//**
 * @brief   assembly labels that all embedded assembly code can refer to
 * @note    This function should not be called
 *****************************************************************************/
__asm void __asm_symbols(void)
{
Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F

I_Bit           EQU     0x80                ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40                ; when F bit is set, FIQ is disabled
T_Bit           EQU     0x20                ; when T bit is set, core is in Thumb state

}

/**************************************************************************//**
 * @brief   disable both IRQ and FIQ
 * @post    IRQ and FIQ disabled
 * TODO     needs more testing
 *****************************************************************************/
__asm void __atomic_on(void)
{
        PRESERVE8
        PUSH    {R4, LR}
        MRS     R4, CPSR
        ORR     R4, #I_Bit:OR:F_Bit ; set both I and F bits
        MSR     CPSR_c, R4
        POP     {R4, PC}
}

/**************************************************************************//**
 * @brief   enable both IRQ and FIQ
 * @post    IRQ and FIQ enabled
 * TODO     needs more testing
 *****************************************************************************/
__asm void __atomic_off(void)
{
        PRESERVE8
        PUSH    {R4, LR}
        MRS     R4, CPSR
        BIC     R4, #I_Bit:OR:F_Bit  ; clear both I and F bits
        MSR     CPSR_c, R4
        POP     {R4, PC}
}
#pragma pop

/**************************************************************************//**
 * @brief   change processor mode
 *
 * @param   mode    the processor mode numerical value
 * @post    processor mode changes if caller is not in USR mode
 *
 *****************************************************************************/
#pragma push
#pragma arm
__asm void __ch_MODE (U32 mode) {
        ARM
        PUSH	{R1, R4}
        MOV     R1, LR
        MSR 	CPSR_csxf, R0	; no effect in USR mode
        ISB                     ; flushes the pipeline in the processor
        MOV     LR, R1
        POP     {R1, R4}
        BX      LR
}
#pragma pop

/**************************************************************************//**
 * @brief   set the stack of a given mode
 *
 * @param   sp      the stack pointer to be set
 * @param   mode    the mode of the sp
 * @post    SP_MODE updated if caller is not in USR mode
 *
 * TODO:    needs more testing
 *
 *****************************************************************************/
#pragma push
#pragma arm

/**
 * @brief       set the stack of a given mode
 * @pre:        the mode is a valid mode, caller checks
 * @post:		processor retains the mode before the function is called
 */
__asm void __set_SP_MODE (U32 sp, U32 mode) {
        ARM

        PUSH	{R4, LR}
        MRS     R4, CPSR    	; save CPSR in R4
        MSR     CPSR_csxf, R1   ; R1 contains the mode, no effect in USR mode
        ISB                 	; flushes the pipeline in the processor
        MOV     SP, R0      	; R0 contains SP to be set
        MSR     CPSR_c, R4  	; restore CPSR, no effect in USR mode
        ISB                 	; flushes the pipline in the processor
        POP     {R4, PC}
}

#pragma pop

/**************************************************************************//**
 * @brief   	SVC Handler (i.e. trap handler)
 * @pre     	The caller should be in USR/SYS mode
 *          	R12 contains trap table mapped kernel function entry point
 *          	Processor is in ARM Mode
 * @attention   Only handles ARM Mode
 *****************************************************************************/
#pragma push
#pragma arm
__asm void SVC_Handler (void) 
{

        PRESERVE8                       ; 8 bytes alignement of the stack
        ARM
        EXPORT  SVC_RESTORE

SVC_SAVE

        SRSFD   SP!, #Mode_SVC          ; Push LR_SVC and SPSR_SVC onto SVC mode stack
        SUB     SP, SP, #56
        STM     SP, {R0-R12, SP}^       ; push SP_USR and R0 - R12 onto the kernel stack


        ;// extract SVC number, only handles #0
        MRS     R4,SPSR                 ; Get SPSR
        LDR     R4,[LR,#-4]             ; ARM:   Load Word
        BIC     R4,R4,#0xFF000000       ; Extract SVC Number

        CMP     R4,#0
        BNE     SVC_EXIT                ; if not SVC #0, go to SVC_EXIT

        BLX     R12                     ; invoke the corresponding c kernel function

SVC_RESTORE
        STR     R0, [SP]                ; save the function return value on R0 that is on top of the stack

SVC_EXIT  
        LDM     SP, {R0-R12, SP}^       ; restore SP_USR and R0-R12 from their saved values on the stack
        ADD     SP, SP, #56
        RFEFD   SP!                     ; Return from exception
}
#pragma pop
#pragma push
#pragma arm

__asm void IRQ_Handler(void){
        PRESERVE8
        ARM
        IMPORT	c_IRQ_Handler

        SUB     LR, LR, #4              ; Pre-adjust LR
        SRSFD   SP!, #Mode_SVC          ; Push LR_IRQ and SPSR_IRQ onto SVC mode stack
        CPS     #MODE_SVC               ; Change to SVC mode

        PUSH	{R0-R12, LR}		; Push LR_SVC and R0 - R12 onto the kernel stack
        SUB 	SP, SP, #8
        STM     SP, {SP}^		; Push SP_USR onto the kernel stack

        BL 	c_IRQ_Handler           ; Call the uart interrupt handler for UART0 interrupt

EXIT_IRQ
        LDM     SP, {SP}^               ; Restore SP_USR and R0-R12 from their saved values on the stack
        ADD     SP, SP, #8
        POP     {R0-R12, LR}
        RFEFD   SP!                     ; Return from exception
}

#pragma pop


void c_IRQ_Handler(void)
{
	static unsigned int a9_timer_last = 0xFFFFFFFF; // the initial value of free-running timer
	unsigned int a9_timer_curr;

	char switch_flag = 0;
	// Read the ICCIAR from the CPU Interface in the GIC
	U32 interrupt_ID = GIC_AckPending();
	if (interrupt_ID == UART0_Rx_IRQ_ID)
	{
		if(UART0_GetRxIRQStatus())			// check if interrupt type is Data Receive
		{
			while(UART0_GetRxDataStatus())	// read while Data Ready is valid
			{
				char c = UART0_GetRxData();	// would also clear the interrupt if last character is read
				SER_PutChar(1, c);	        // display back
			}
			switch_flag = 1;
		}
		else
		{   // unexpected interrupt type
			SER_PutStr(0, "Error interrupt type!\r\n");
		}
	}
	else if(interrupt_ID == HPS_TIMER0_IRQ_ID)
	{
		timer_clear_irq(0);
		a9_timer_curr = timer_get_current_val(2);	//get the current value of the free running timer
		if ((a9_timer_last - a9_timer_curr) > 500000U)
		{
			printf("%d ms passed!\r\n", ((a9_timer_last - a9_timer_curr)/1000U));
			a9_timer_last = a9_timer_curr;
		}
	}
	else if(interrupt_ID == HPS_TIMER1_IRQ_ID)
	{
		timer_clear_irq(1);
	}
	else if(interrupt_ID == A9_TIMER_IRQ_ID)
	{
		timer_clear_irq(2);
	}
	else
	{
		printf("unrecognized interrupt!\r\n");
	}
	// Write to the End of Interrupt Register (ICCEOIR)
	GIC_EndInterrupt(interrupt_ID);
	// Make sure to call line 246 before context switching
	if (switch_flag == 1)
	{
		k_tsk_run_new();
	}
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
