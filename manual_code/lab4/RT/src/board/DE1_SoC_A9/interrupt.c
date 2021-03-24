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
 * @file        interrupt.c
 * @brief       Interrupt configuration and handler code
 * @version     V1.2021.02
 * @authors     Zehan Gao
 * @date        2021 FEB
 *
 * @note	Only support UART0_irq in current version
 *		Reference to Intel University Program code
 *
 *****************************************************************************/
#include "interrupt.h"
#include "Serial.h"
#include "timer.h"

//Initialize and enable the GIC
void GIC_Enable(void)
{
	GIC_DistInit();
	GIC_CPUInterfaceInit(); //per CPU
}

// Interrupt distributor initialization
void GIC_DistInit(void)
{
	uint32_t i;
	uint32_t num_irq = 0U;
	uint32_t priority_field;

	//A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
	//configuring all of the interrupts as Secure.

	//Disable interrupt forwarding
	GIC_DisableDistributor();
	//Get the maximum number of interrupts that the GIC supports
	num_irq = 32U * ((GIC_DistributorInfo() & 0x1FU) + 1U);

	/* Priority level is implementation defined.
	 To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
	 priority field and read back the value stored.*/
	GIC_SetPriority(0U, 0xFFU);
	priority_field = GIC_GetPriority(0U);

	for (i = 32U; i < num_irq; i++)
	{
		//Disable the SPI interrupt
		GIC_DisableIRQ(i);
		//Deactivate Interrupt
		GIC_EndInterrupt(i);
		//Set level-sensitive (and N-N model)
		GIC_SetConfiguration(i, 0U);
		//Set priority
		GIC_SetPriority(i, priority_field / 2U);
		//Set target list to CPU0
		GIC_SetTarget(i, 1U);
	}
	//Enable distributor
	GIC_EnableDistributor();
}

// CPU interrupt interface initialization
void GIC_CPUInterfaceInit(void)
{
	uint32_t i;
	uint32_t priority_field;

	//A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
	//configuring all of the interrupts as Secure.

	//Disable interrupt forwarding
	GIC_DisableInterface();

	/* Priority level is implementation defined.
	 To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
	 priority field and read back the value stored.*/
	GIC_SetPriority(0U, 0xFFU);
	priority_field = GIC_GetPriority(0U);

	//SGI and PPI
	for (i = 0U; i < 32U; i++)
	{
		if (i > 15U)
		{
			//Set level-sensitive (and N-N model) for PPI
			GIC_SetConfiguration(i, 0U);
		}
		//Disable SGI and PPI interrupts
		GIC_DisableIRQ(i);
		//Deactivate Interrupt
		GIC_EndInterrupt(i);
		//Set priority
		GIC_SetPriority(i, priority_field / 2U);
	}
	//Enable interface
	GIC_EnableInterface();
	//Set binary point to 0
	GIC_SetBinaryPoint(0U);
	//Set priority mask
	GIC_SetInterfacePriorityMask(0xFFU);
}

// Enable the interrupt distributor using the GIC's CTLR register
void GIC_EnableDistributor(void)
{
	GICDistributor->CTLR |= 1U;
}

// Disable the interrupt distributor using the GIC's CTLR register.
void GIC_DisableDistributor(void)
{
	GICDistributor->CTLR &= ~1U;
}

// Enable the CPU's interrupt interface
void GIC_EnableInterface(void)
{
	GICInterface->CTLR = 1U;
}

// Disable the CPU's interrupt interface
void GIC_DisableInterface(void)
{
	GICInterface->CTLR = 0x200;
}

// Read the GIC's TYPER register.
uint32_t GIC_DistributorInfo(void)
{
	return (GICDistributor->TYPER);
}

// Set the priority for the given interrupt in the GIC's IPRIORITYR register.
void GIC_SetPriority(uint32_t IRQn, uint32_t priority)
{
	uint32_t mask = GICDistributor->IPRIORITYR[IRQn / 4U]
			& ~(0xFFUL << ((IRQn % 4U) * 8U));
	GICDistributor->IPRIORITYR[IRQn / 4U] = mask
			| ((priority & 0xFFUL) << ((IRQn % 4U) * 8U));
}

// Read the current interrupt priority from GIC's IPRIORITYR register.
uint32_t GIC_GetPriority(uint32_t IRQn)
{
	return (GICDistributor->IPRIORITYR[IRQn / 4U] >> ((IRQn % 4U) * 8U))
			& 0xFFUL;
}

// Disables the given interrupt using GIC's ICENABLER register.
void GIC_DisableIRQ(uint32_t IRQn)
{
	GICDistributor->ICENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

// Sets the interrupt configuration using GIC's ICFGR register.
void GIC_SetConfiguration(uint32_t IRQn, uint32_t int_config)
{
	uint32_t icfgr = GICDistributor->ICFGR[IRQn / 16U];
	uint32_t shift = (IRQn % 16U) << 1U;

	icfgr &= (~(3U << shift));
	icfgr |= (int_config << shift);

	GICDistributor->ICFGR[IRQn / 16U] = icfgr;
}

// Sets the GIC's ITARGETSR register for the given interrupt.
void GIC_SetTarget(uint32_t IRQn, uint32_t cpu_target)
{
	uint32_t mask = GICDistributor->ITARGETSR[IRQn / 4U]
			& ~(0xFFUL << ((IRQn % 4U) * 8U));
	GICDistributor->ITARGETSR[IRQn / 4U] = mask
			| ((cpu_target & 0xFFUL) << ((IRQn % 4U) * 8U));
}

// Set the interrupt priority mask using CPU's PMR register.
void GIC_SetInterfacePriorityMask(uint32_t priority)
{
	GICInterface->PMR = priority & 0xFFUL;
}

// Configures the group priority and subpriority split point using CPU's BPR register.
void GIC_SetBinaryPoint(uint32_t binary_point)
{
	GICInterface->BPR = binary_point & 7U;
}

// Writes the given interrupt number to the CPU's EOIR register.
void GIC_EndInterrupt(uint32_t IRQn)
{
	GICInterface->EOIR = IRQn;
}

// Enables the given interrupt using GIC's ISENABLER register.
void GIC_EnableIRQ(uint32_t IRQn)
{
	GICDistributor->ISENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

// Read the CPU's IAR register.
uint32_t GIC_AckPending(void)
{
	return (GICInterface->IAR);
}

