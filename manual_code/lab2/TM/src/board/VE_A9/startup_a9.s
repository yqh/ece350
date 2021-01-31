;/*
; ****************************************************************************
; *
; *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
; *
; *                     Copyright (C) 2009-2018 ARM Limited.
; *                          All rights reserved.
; *
; *                     Copyright (C) 2020-2021 Yiqing Huang
; *                          All rights reserved.
;-----------------------------------------------------------------------------
;/* Copyright (c) 2011 - 2015 ARM LIMITED
;   All rights reserved.
;   Redistribution and use in source and binary forms, with or without
;   modification, are permitted provided that the following conditions are met:
;   - Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;   - Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;   - Neither the name of ARM nor the names of its contributors may be used
;     to endorse or promote products derived from this software without
;     specific prior written permission.
;   *
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
;   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;   POSSIBILITY OF SUCH DAMAGE.
; ---------------------------------------------------------------------------*/
; * Code Modification Copyright (c) 2020-2021 Yiqing Huang
; *
; *  All rights reserved.
; *  Redistribution and use in source and binary forms, with or without
; *  modification, are permitted provided that the following conditions are met:
; *  - Redistributions of source code must retain the above copyright
; *    notice and the following disclaimer.
; *
; *
; *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
; *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
; *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
; *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; *  POSSIBILITY OF SUCH DAMAGE.
; ****************************************************************************
; */

;/*********************************************************************************************
; * @brief modifed version of ARM startup_VE_A9_MP.s
; * @note  MMU part is taken out, simpify IRQ handlers.
; *        The device dependent content is the RAM_BASE, set it according to the specific device
; *        Other parts are generic to any A9 processor devices
; *        This file references scatter file defined symbols, so it should be used together
; *        with the provided scatter file
; *********************************************************************************************/

RAM_BASE        EQU     0x80000000      ; VE9
SVC_Stack_Size  EQU     0x00000000      ; we do not allocate SVC stack here, take it from g_k_stacks[0]

;reset of exception mode stacks go to c routine to set up
IRQ_Stack_Size  EQU     0x00000000
USR_Stack_Size  EQU     0x00000000      ; user stacks will be set up by the kernel

ISR_Stack_Size  EQU     (SVC_Stack_Size + IRQ_Stack_Size)

;---------------------------------------------------------------------------
; Stack, Heap for application using microlib
;---------------------------------------------------------------------------

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   USR_Stack_Size
__initial_sp    SPACE   ISR_Stack_Size
Stack_Top

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

;---------------------------------------------------------------------------
; Code
;---------------------------------------------------------------------------
                PRESERVE8
                ARM

; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, CODE, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       LDR     PC, Reset_Addr            ; Address of Reset Handler
                LDR     PC, Undef_Addr            ; Address of Undef Handler
                LDR     PC, SVC_Addr              ; Address of SVC Handler
                LDR     PC, PAbt_Addr             ; Address of Prefetch Abort Handler
                LDR     PC, DAbt_Addr             ; Address of Data Abort Handler
                NOP                               ; Reserved Vector
                LDR     PC, IRQ_Addr              ; Address of IRQ Handler
                LDR     PC, FIQ_Addr              ; Address of FIQ Handler
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

Reset_Addr      DCD     Reset_Handler
Undef_Addr      DCD     Undef_Handler
SVC_Addr        DCD     SVC_Handler
PAbt_Addr       DCD     PAbt_Handler
DAbt_Addr       DCD     DAbt_Handler
IRQ_Addr        DCD     IRQ_Handler
FIQ_Addr        DCD     FIQ_Handler




                AREA    |.text|, CODE, READONLY

Reset_Handler   PROC
                EXPORT  Reset_Handler               [WEAK]
                IMPORT  StackInit
                IMPORT  SystemInit
                IMPORT  main
                IMPORT  g_k_stacks					; the kernel stack array symbol
                IMPORT  g_k_stack_size              ; the kernel stack size for each task
                LDR     R0, =g_k_stacks             ; R0 has the starting address of g_k_stacks[][] array
                LDR     R1, =g_k_stack_size         ; R1 has the kernel stack size
                LDR     R1, [R1]
                ADD     R0, R0, R1                  ; Move to the high address of the first task's stack
                MOV     SP, R0                      ; Use the first task

                ; Put any cores other than 0 to sleep
                MRC     p15, 0, R0, c0, c0, 5     ; Read MPIDR
                ANDS    R0, R0, #3
goToSleep
                WFINE
                BNE     goToSleep

                MRC     p15, 0, R0, c1, c0, 0       ; Read CP15 System Control register
                BIC     R0, R0, #(0x1 << 12)        ; Clear I bit 12 to disable I Cache
                BIC     R0, R0, #(0x1 <<  2)        ; Clear C bit  2 to disable D Cache
                BIC     R0, R0, #0x1                ; Clear M bit  0 to disable MMU
                BIC     R0, R0, #(0x1 << 11)        ; Clear Z bit 11 to disable branch prediction
                BIC     R0, R0, #(0x1 << 13)        ; Clear V bit 13 to disable hivecs
                MCR     p15, 0, R0, c1, c0, 0       ; Write value back to CP15 System Control register
                ISB

; Configure ACTLR
                MRC     p15, 0, r0, c1, c0, 1       ; Read CP15 Auxiliary Control Register
                ORR     r0, r0, #(1 <<  1)          ; Enable L2 prefetch hint (UNK/WI since r4p1)
                MCR     p15, 0, r0, c1, c0, 1       ; Write CP15 Auxiliary Control Register
; Set Vector Base Address Register (VBAR) to point to this application's vector table
                LDR     R0, =__Vectors
                MCR     p15, 0, R0, c12, c0, 0

                LDR     R0, =StackInit              ; Initialize stack for each exception mode
                BLX     R0
                LDR     R0, =SystemInit
                BLX     R0                          ; copy vector table, set up system clocks
                LDR     R0, =main
                BLX     main                        ; start the main function
                B       .                           ; loop if main ever returns
                ENDP

Undef_Handler   PROC
                EXPORT  Undef_Handler               [WEAK]
                B       .
                ENDP

PAbt_Handler    PROC
                EXPORT  PAbt_Handler                [WEAK]
                B       .
                ENDP

DAbt_Handler    PROC
                EXPORT  DAbt_Handler                [WEAK]
                B       .
                ENDP

SVC_Handler     PROC
                EXPORT  SVC_Handler                 [WEAK]
                B       .
                ENDP

IRQ_Handler     PROC
                EXPORT  IRQ_Handler                 [WEAK]
                B       .
                ENDP

FIQ_Handler     PROC
                EXPORT  FIQ_Handler                 [WEAK]
                B       .
                ENDP

; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, = (Stack_Mem + USR_Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ENDIF

                END
;/*
; *===========================================================================
; *                             END OF FILE
; *===========================================================================
; */
