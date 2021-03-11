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
		if (counter == 39) {
			int j = 0;
			int kk = j;
		}
	}
}

void kcd_reg_and_exit(void){

}

void kcd_receive_and_print(void){

}

void kcd_waiting(void){
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
