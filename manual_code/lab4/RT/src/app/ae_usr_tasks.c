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
 * C++ version 0.4 char* style "itoa":
 * Written by Lukï¿½s Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

/**
 * @brief: a dummy task1
 */
void utask1(void)
{
	SER_PutStr(0, "utask1: entering \n\r");
	/* do something */
	long int x = 0;
	int i = 0;
	int j = 0;
	while (1) {
		SER_PutStr(0, "utask1: ");
		char out_char = 'a' + i % 10;
		for (j = 0; j < 5; j++)
		{
			SER_PutChar(0, out_char);
		}
		SER_PutStr(0, "\n\r");
		++i;
		for (x = 0; x < 5000000; x++)
			; // some artifical delay
	}
	/* terminating */
	// tsk_exit();
}

void utask2(void)
{
	SER_PutStr(0, "utask2: entering \n\r");
	/* do something */
	long int x = 0;
	int i = 0;
	int j = 0;
	while (1)
	{
		SER_PutStr(0, "utask2: ");
		char out_char = 'A' + i % 10;
		for (j = 0; j < 5; j++) {
			SER_PutChar(0, out_char);
		}
		SER_PutStr(0, "\n\r");
		++i;
		for (x = 0; x < 5000000; x++)
			; // some artifical delay
	}
	/* terminating */
	// tsk_exit();
}

void kcd_reg_and_exit(void){
	mbx_create(KCD_MBX_SIZE);
	RTX_MSG_CHAR msg;
	msg.hdr.length = sizeof(RTX_MSG_HDR) + 1;
	msg.hdr.type = KCD_REG;
	msg.data = 'q';
	if(send_msg(TID_KCD, &msg) == RTX_OK){
		SER_PutStr(0, "Sent Reg Msg");
	}
	tsk_exit();
}

void kcd_receive_and_print(void){
	mbx_create(KCD_MBX_SIZE);
	task_t sender_tid;
	char* recv_buf = mem_alloc(KCD_MBX_SIZE);

	RTX_MSG_CHAR msg;
	msg.hdr.length = sizeof(RTX_MSG_HDR) + 1;
	msg.hdr.type = KCD_REG;
	msg.data = 'w';
	send_msg(TID_KCD, &msg);

	U8 flag = 0;

	while(1){
		int ret_val = recv_msg(&sender_tid, recv_buf, KCD_MBX_SIZE);
		SER_PutStr(0, "kcd_receive_and_print: ");

		if(ret_val != RTX_OK){
			SER_PutStr(0, "Sike\r\n");
			continue;
		}

		RTX_MSG_HDR* msg_hdr = (RTX_MSG_HDR*)(recv_buf);

        if (msg_hdr->type != KCD_CMD){
            // ignore message if data more than 1 char
            SER_PutStr(0, " Not a CMD message\r\n");
            continue;
        }

		*(recv_buf + ((RTX_MSG_HDR*)recv_buf)->length) = '\0';
		SER_PutStr(0, recv_buf + sizeof(RTX_MSG_HDR));
		SER_PutStr(0, "\r\n");

		if (flag == 0) {
			flag = 1;
			msg.hdr.length = sizeof(RTX_MSG_HDR) + 1;
			msg.hdr.type = KCD_REG;
			msg.data = 'q';
			send_msg(TID_KCD, &msg);
		} else if (flag == 1) {
			flag = 2;
			msg.hdr.length = sizeof(RTX_MSG_HDR) + 1;
			msg.hdr.type = KCD_REG;
			msg.data = '!';
			send_msg(TID_KCD, &msg);
		} else if (flag == 2) {
			flag = 3;
			msg.hdr.length = sizeof(RTX_MSG_HDR) + 2;
			msg.hdr.type = KCD_REG;
			msg.data = '!';
			send_msg(TID_KCD, &msg);
		}

	}
}

void kcd_waiting(void){
	mbx_create(KCD_MBX_SIZE);

	RTX_MSG_CHAR msg;
	msg.hdr.length = sizeof(RTX_MSG_HDR) + 1;
	msg.hdr.type = KCD_REG;
	msg.data = 'e';

	send_msg(TID_KCD, &msg);

    while(1){
    	U32 counter = 0;
    	for(U32 timer = 0xFFFFFF; timer != 0; timer--){
    		counter++;
    	}
        SER_PutStr(0, "Uno Cycle\r\n");
        tsk_yield();
    }
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
