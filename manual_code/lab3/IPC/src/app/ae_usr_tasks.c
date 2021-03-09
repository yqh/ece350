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
 * Written by Luk�s Chmela
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
	U32 counter = 0;
	while(1) {
		void* msg = mem_alloc(200);
		RTX_MSG_HDR* hdr = msg;
		char count_str[5];
		char* count_str_s = itoa(counter, count_str, 10);
		char hello_world[19] = "hello world \0\0\0\0\n\r\0";
		int i = 12;
		while (*(count_str_s+i-12) != '\0' && i < 16) {
			hello_world[i] = *(count_str_s + i - 12);
			i++;
		}

		hello_world[i] = '\n';
		hello_world[i + 1] = '\r';
		hello_world[i + 2] = '\0';
		hdr->length = sizeof(RTX_MSG_HDR) + i + 3;

		for (int i = 0; i < 19; i++) {
			*((char*)msg + sizeof(RTX_MSG_HDR) + i) = hello_world[i];
		}

		send_msg(TID_KCD, msg);
		mem_dealloc(msg);
		counter++;
	}
}

U8 charIsValid(U8 character){
    // if (character > 47 && character < 58){
    //     // 0-9 = 48-57
    //     return character - 48;
    // }else if (character > 64 && character < 91){
    //     // A-Z = 65-90
    //     return character - 55;
    // }else if (character > 96 && character < 123){
    //     // a-z = 97-122
    //     return character - 61;
    // } else {
    //     return 255;
    // }
    return 1;
}

U8 charIsAlphaNum(U8 character){
    if (character > 47 && character < 58){
        // 0-9 = 48-57
        return 1;
    }else if (character > 64 && character < 91){
        // A-Z = 65-90
        return 1;
    }else if (character > 96 && character < 123){
        // a-z = 97-122
        return 1;
    } else {
        return 0;
    }
}

U8 charToIndex(U8 character){
    if (character > 47 && character < 58){
        // 0-9 = 48-57
        return character - 48;
    }else if (character > 64 && character < 91){
        // A-Z = 65-90
        return character - 55;
    }else if (character > 96 && character < 123){
        // a-z = 97-122
        return character - 61;
    } else {
        return 255;
    }
}

void kcd_task(void)
{
	mbx_create(KCD_MBX_SIZE);
	task_t sender_tid;
	char* recv_buf = mem_alloc(KCD_MBX_SIZE);

	// 62 valid option commands for registration
	U8 cmd_reg[62];
	for (int i=0; i<62; i++){
		cmd_reg[i] = 0;
	}

	// KEY_IN queue
	U8 cmd_queue[64];
	for (int i=0; i<64; i++){
		cmd_queue[i] = 0;
	}

	U8 cmd_queue_counter = 0;
	U8 cmd_len = 0;
	U8 cmd_invalid = 0;

	while(1) {
		int ret_val = recv_msg(&sender_tid, recv_buf, KCD_MBX_SIZE);

		SER_PutStr(0, recv_buf + sizeof(RTX_MSG_HDR));

		RTX_MSG_HDR* msg_hdr = (RTX_MSG_HDR*)(recv_buf);

		// process KCD_REG or KEY_IN type messages
		if (ret_val && msg_hdr->type == KCD_REG){
			// command registration

			RTX_MSG_CHAR * msg = (RTX_MSG_CHAR*) recv_buf;

			if (msg->hdr.length != sizeof(RTX_MSG_CHAR)){
				// ignore message if data more than 1 char
				continue;
			}

			// get command identifier
			U8 cmd_id = msg->data;

			if (charIsAlphaNum(cmd_id) == 0){
				// invalid cmd_id
				continue;
			}
			
			// convert cmd_id to ascii and offset
			U8 index = charToIndex(cmd_id);

			// store it in cmd_reg
			cmd_reg[index] = cmd_id;

		} else if (ret_val && msg_hdr->type == KEY_IN){
			// Keyboard Input

			// get command char
			RTX_MSG_CHAR * msg = (RTX_MSG_CHAR*) recv_buf;
			U8 cmd_char = msg->data;
			cmd_len++;

			// first character should be %
			if (cmd_len == 1 && cmd_char != 37){
				cmd_invalid = 1;
			}

			// command length (including % and enter) larger than 64 bytes
			if (cmd_len > 64){
				cmd_invalid = 1;
			}

			// TODO: write charIsValid(), figure out what is a valid char?
			if (!charIsValid(cmd_char)){
				cmd_invalid = 1;
			}

			if (cmd_invalid == 0){
				// valid command
				// if enter: dequeue and create string
				if (cmd_char == 13){
					// populate command message buffer

					void* msg = mem_alloc(sizeof(RTX_MSG_HDR) + cmd_len - 2);
					RTX_MSG_HDR* hdr = msg;

					//RTX_MSG_HDR *cmd_msg;
					hdr->type = KCD_CMD;
					hdr->length = sizeof(RTX_MSG_HDR) + cmd_len - 2; // exclude % and enter

					// TODO: verify if string create works
					for(int i=0; i<cmd_queue_counter-1; i++){
						// exclude % at i=0
						*((char*)msg + sizeof(RTX_MSG_HDR) + i) = cmd_queue[i+1];
					}

					task_t recv_tid = cmd_reg[charToIndex(cmd_queue[1])];

					// TODO: check validity of cmd_id and task_id and cmd_len
					RTX_TASK_INFO task_info;
					k_tsk_get(recv_tid, &task_info);

					// unregistered cmd_id or invalid tid
					if(recv_tid <= 0 || task_info.state == DORMANT || recv_tid > MAX_TASKS){
						SER_PutStr(0, "Command cannot be processed");
					}else{

						// send message to registered task id
						int msg_sent = send_msg(recv_tid, msg);

						if (!msg_sent){
							SER_PutStr(0, "Command cannot be processed");
						}
					}
					
					// reset
					for(int i=0; i<64; i++){
						cmd_queue[i] = 0;
					}
					cmd_queue_counter = 0;
					cmd_len = 0;
					cmd_invalid = 0;

				}else{
					if(cmd_len >= 64){
						cmd_invalid = 1;
					}else{
						// enqueue KEY_IN msg data
						cmd_queue[cmd_queue_counter] = cmd_char;
						cmd_queue_counter++;
					}
				}
			}else{
				// invalid command
				// if enter: dequeue and output failure message
				if (cmd_char == 13){
					SER_PutStr(0, "Invalid Command");

					// reset
					for(int i=0; i<64; i++){
						cmd_queue[i] = 0;
					}
					cmd_queue_counter = 0;
					cmd_len = 0;
					cmd_invalid = 0;
				}
			}
		}
	}
}

void kcd_waiting(void){
    while(1){
        SER_PutStr(0, "Uno\r\n");
        SER_PutStr(0, "Dos\r\n");
        SER_PutStr(0, "Tres\r\n");
        SER_PutStr(0, "Cuatro\r\n");
        SER_PutStr(0, "Cinco\r\n");
        SER_PutStr(0, "Seis\r\n");
        SER_PutStr(0, "Siete\r\n");
        SER_PutStr(0, "Ocho\r\n");
        SER_PutStr(0, "Nueve\r\n");
        SER_PutStr(0, "Diez\r\n");
        SER_PutStr(0, "Once\r\n");
        SER_PutStr(0, "Doce\r\n");
        SER_PutStr(0, "Trece\r\n");
        SER_PutStr(0, "Catorce\r\n");
        SER_PutStr(0, "Quince\r\n");
        SER_PutStr(0, "Dieciseis\r\n");
        tsk_yield();
    }
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
