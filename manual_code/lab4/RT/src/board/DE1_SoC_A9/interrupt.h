/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Zehan Gao
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
 * @file        interrupt.h
 * @brief       Interrupt configuration and handler header
 * @version     V1.2021.02
 * @authors     Zehan Gao, Intel University Program
 * @date        2021 FEB
 *
 * @note	Only support UART0_irq in current version
 *
 *
 *****************************************************************************/

#ifndef INTERRUPT_H
#define	INTERRUPT_H

#define	A9_TIMER_IRQ_ID 29
#define	UART0_Rx_IRQ_ID 194
#define	HPS_TIMER0_IRQ_ID 199
#define	HPS_TIMER1_IRQ_ID 200

#define __IM     volatile const      /* Defines 'read only' structure member permissions */
#define __OM     volatile            /* Defines 'write only' structure member permissions */
#define __IOM    volatile            /* Defines 'read/write' structure member permissions */

typedef unsigned int uint32_t;


void GIC_Enable(void);
void GIC_EnableIRQ(uint32_t);
void GIC_DisableIRQ(uint32_t);
void GIC_EndInterrupt(uint32_t);
uint32_t GIC_AckPending(void);
void GIC_SetBinaryPoint(uint32_t);
void GIC_SetInterfacePriorityMask(uint32_t);
void GIC_SetTarget(uint32_t, uint32_t);
void GIC_SetConfiguration(uint32_t, uint32_t);
uint32_t GIC_GetPriority(uint32_t);
void GIC_SetPriority(uint32_t, uint32_t);
uint32_t GIC_DistributorInfo(void);
void GIC_DisableInterface(void);
void GIC_EnableInterface(void);
void GIC_DisableDistributor(void);
void GIC_EnableDistributor(void);
void GIC_CPUInterfaceInit(void);
void GIC_DistInit(void);

typedef struct
{
    uint32_t CTLR;					/* Offset: 0x000 (R/W) Distributor Control Register */
    uint32_t TYPER;					/* Offset: 0x004 (R/ ) Interrupt Controller Type Register */
    uint32_t IIDR;					/* Offset: 0x008 (R/ ) Distributor Implementer Identification Register */
    uint32_t RESERVED0;
    uint32_t STATUSR;				/* Offset: 0x010 (R/W) Error Reporting Status Register, optional */
    uint32_t RESERVED1[11];
    uint32_t SETSPI_NSR;			/* Offset: 0x040 ( /W) Set SPI Register */
    uint32_t RESERVED2;
    uint32_t CLRSPI_NSR;			/* Offset: 0x048 ( /W) Clear SPI Register */
    uint32_t RESERVED3;
    uint32_t SETSPI_SR;				/* Offset: 0x050 ( /W) Set SPI, Secure Register */
    uint32_t RESERVED4;
    uint32_t CLRSPI_SR;				/* Offset: 0x058 ( /W) Clear SPI, Secure Register */
    uint32_t RESERVED5[9];
    uint32_t IGROUPR[32];			/* Offset: 0x080 (R/W) Interrupt Group Registers */
    uint32_t ISENABLER[32];			/* Offset: 0x100 (R/W) Interrupt Set-Enable Registers */
    uint32_t ICENABLER[32];			/* Offset: 0x180 (R/W) Interrupt Clear-Enable Registers */
    uint32_t ISPENDR[32];			/* Offset: 0x200 (R/W) Interrupt Set-Pending Registers */
    uint32_t ICPENDR[32];			/* Offset: 0x280 (R/W) Interrupt Clear-Pending Registers */
    uint32_t ISACTIVER[32];			/* Offset: 0x300 (R/W) Interrupt Set-Active Registers */
    uint32_t ICACTIVER[32];			/* Offset: 0x380 (R/W) Interrupt Clear-Active Registers */
    uint32_t IPRIORITYR[255];		/* Offset: 0x400 (R/W) Interrupt Priority Registers */
    uint32_t RESERVED6;
    uint32_t ITARGETSR[255];		/* Offset: 0x800 (R/W) Interrupt Targets Registers */
    uint32_t RESERVED7;
    uint32_t ICFGR[64];				/* Offset: 0xC00 (R/W) Interrupt Configuration Registers */
    uint32_t IGRPMODR[32];			/* Offset: 0xD00 (R/W) Interrupt Group Modifier Registers */
    uint32_t RESERVED8[32];
    uint32_t NSACR[64];				/* Offset: 0xE00 (R/W) Non-secure Access Control Registers */
    uint32_t SGIR;					/* Offset: 0xF00 ( /W) Software Generated Interrupt Register */
    uint32_t RESERVED9[3];
    uint32_t CPENDSGIR[4];			/* Offset: 0xF10 (R/W) SGI Clear-Pending Registers */
    uint32_t SPENDSGIR[4];			/* Offset: 0xF20 (R/W) SGI Set-Pending Registers */
}  GICDistributor_Type;

typedef struct
{
  __IOM uint32_t CTLR;				/* Offset: 0x000 (R/W) CPU Interface Control Register */
  __IOM uint32_t PMR;               /* Offset: 0x004 (R/W) Interrupt Priority Mask Register */
  __IOM uint32_t BPR;               /* Offset: 0x008 (R/W) Binary Point Register */
  __IM  uint32_t IAR;               /* Offset: 0x00C (R/ ) Interrupt Acknowledge Register */
  __OM  uint32_t EOIR;              /* Offset: 0x010 ( /W) End Of Interrupt Register */
  __IM  uint32_t RPR;               /* Offset: 0x014 (R/ ) Running Priority Register */
  __IM  uint32_t HPPIR;             /* Offset: 0x018 (R/ ) Highest Priority Pending Interrupt Register */
  __IOM uint32_t ABPR;              /* Offset: 0x01C (R/W) Aliased Binary Point Register */
  __IM  uint32_t AIAR;              /* Offset: 0x020 (R/ ) Aliased Interrupt Acknowledge Register */
  __OM  uint32_t AEOIR;             /* Offset: 0x024 ( /W) Aliased End Of Interrupt Register */
  __IM  uint32_t AHPPIR;            /* Offset: 0x028 (R/ ) Aliased Highest Priority Pending Interrupt Register */
  __IOM uint32_t STATUSR;           /* Offset: 0x02C (R/W) Error Reporting Status Register, optional */
  uint32_t RESERVED1[40];
  __IOM uint32_t APR[4];            /* Offset: 0x0D0 (R/W) Active Priority Register */
  __IOM uint32_t NSAPR[4];          /* Offset: 0x0E0 (R/W) Non-secure Active Priority Register */
  uint32_t RESERVED2[3];
  __IM  uint32_t IIDR;              /* Offset: 0x0FC (R/ ) CPU Interface Identification Register */
  uint32_t RESERVED3[960];
  __OM  uint32_t DIR;               /* Offset: 0x1000( /W) Deactivate Interrupt Register */
}  GICInterface_Type;


#define GICDistributor	((GICDistributor_Type*)	0xFFFED000)
#define GICInterface	((GICInterface_Type*)	0xFFFEC100)

#endif
