/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
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
 * @file        a usr_tasks.c
 * @brief       Two user/unprivileged  tasks: task1 and task2
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *****************************************************************************/

#include "ae_usr_tasks.h"
#include "rtx.h"
#include "Serial.h"
#include "printf.h"


/**
 * @brief: a dummy task1
 */
void task1(void)
{
	tsk_exit();
}

/**
 * @brief: a dummy task2
 */
void task2(void)
{
	mbx_create(KCD_MBX_SIZE);
	while(1) {
		void* msg = mem_alloc(200);
		RTX_MSG_HDR* hdr = msg;
		hdr->length = sizeof(RTX_MSG_HDR) + 13;
		char hello_world[13] = "hello world\n\r";

		for (int i = 0; i < 13; i++) {
			*((char*)msg + sizeof(RTX_MSG_HDR) + i) = hello_world[i];
		}

		send_msg(TID_KCD, msg);
		mem_dealloc(msg);
	}
}

void kcd_task(void)
{
	mbx_create(KCD_MBX_SIZE);
	while(1) {
		task_t sender_tid;
		char* recv_buf = mem_alloc(KCD_MBX_SIZE);
		int ret_val = recv_msg(&sender_tid, recv_buf, KCD_MBX_SIZE);

		SER_PutStr(0, recv_buf + sizeof(RTX_MSG_HDR));
	}
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
