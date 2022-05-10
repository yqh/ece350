/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2022 Yiqing Huang
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
 * @file        lpc1768_mem.h
 * @brief       LPC1768 memory layout header file
 *              
 * @version     V1.2021.05.rev1
 * @authors     Yiqing Huang
 * @date        2022 MAY
 *
 *****************************************************************************/
#define WORD_SIZE       32                           /* word size in bits    */
#define IROM1_BASE      0x0
#define IROM1_SIZE      0x80000                      /* size in bytes        */
#define IROM_BASE       IROM1_BASE

#define IRAM1_BASE      0x10000000                   /* IRAM1 start addr     */
#define IRAM1_SIZE      0x8000                       /* size in bytes        */
#define IRAM1_SIZE_LOG2 0xF                          /* log2(IRAM1_SIZE)     */
#define IRAM2_BASE      0x2007C000
#define IRAM2_SIZE      0x8000                       /* size in bytes        */
#define IRAM2_SIZE_LOG2 0xF                          /* log2(IRAM2_SIZE)     */

#define RTX_IMG_END     (Image$$RW_IRAM1$$ZI$$Limit) /* linker-defined symbol*/
#define RAM1_START_RT   (U32)(&RTX_IMG_END)             
#define RAM1_SIZE       0x1000                       /* RAM1 size in bytres  */
#define RAM1_SIZE_LOG2  0xC                          /* log2(RAM1_SIZE)      */
#define RAM1_START      (IRAM1_BASE + IRAM1_SIZE - RAM1_SIZE)
#define RAM1_END        (IRAM1_BASE + IRAM1_SIZE - 1)

#define RAM2_START      (IRAM2_BASE)
#define RAM2_END        (IRAM2_BASE + IRAM2_SIZE - 1)
#define RAM2_SIZE       IRAM2_SIZE                   /* RAM2 size in bytes   */   
#define RAM2_SIZE_LOG2  IRAM2_SIZE_LOG2              /* log2(RAM2_SIZE)      */


/*
 *===========================================================================
 *                            GLOBAL VARIABLES
 *===========================================================================
 */
 
extern unsigned int     RTX_IMG_END;

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

