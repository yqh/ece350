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

//----USER TASKS------// (implement in ae_usr_tasks.c)
// tsk_create
// tsk_get

// tsk_create until max_tasks (same priority)
// tsk_get for all TIDs
// tsk create > max_tasks should fail
// tsk_exit 
// tsk_create should reuse TID

// tsk_create
// tsk_get
// tsk_set_prio
// tsk_get
// tsk_set_prio PRIO_NULL should fail
// tsk_set_prio invalid TID should fail
// tsk_set_prio dormant task should fail
// tsk_set_prio for kernal task should fail
// tsk_set_prio for user task
// tsk_get

char s_buffer[255];

void dumdum(void){
    SER_PutStr ("I have brain damage\n\r");
    tsk_exit();
}

/**
 * @brief: a dummy task1
 */
void task1(void)
{
    task_t tid;
    RTX_TASK_INFO task_info;
    
    SER_PutStr ("task1: entering \n\r");
    /* do something */
    tsk_create(&tid, &task2, LOW, 0x200);  /*create a user task */
    tsk_get(tid, &task_info);
    tsk_set_prio(tid, LOWEST);
    /* terminating */
    tsk_exit();
}

/**
 * @brief: a dummy task2
 */
void task2(void)
{
    SER_PutStr ("task2: entering \n\r");
    /* do something */
    /* terminating */
    tsk_exit();
}

// Function to implement strncat() function in C
char* strncat(char* destination, const char* source, size_t num)
{
    // make ptr point to the end of destination string
    char* ptr = destination + strlen(destination);

    // Appends characters of source to the destination string
    while (*source != '\0' && num--)
        *ptr++ = *source++;

    // null terminate destination string
    *ptr = '\0';

    // destination string is returned by standard strncat()
    return destination;
}


void stask0(void)
{
	strncat(s_buffer, "0", 1);
	tsk_exit();
}

void stask1(void)
{
	strncat(s_buffer, "1", 1);
	tsk_exit();
}

void stask2(void)
{
	strncat(s_buffer, "2", 1);
	tsk_exit();
}

void stask3(void)
{
	strncat(s_buffer, "3", 1);
	tsk_exit();
}

void stask4(void)
{
	strncat(s_buffer, "4", 1);
	tsk_exit();
}

void stask5(void)
{
	strncat(s_buffer, "5", 1);
	tsk_exit();
}

void stask6(void)
{
	strncat(s_buffer, "6", 1);
	tsk_exit();
}

void stask7(void)
{
	strncat(s_buffer, "7", 1);
	tsk_exit();
}

void stask8(void)
{
	strncat(s_buffer, "8", 1);
	tsk_exit();
}

void stask9(void)
{
	strncat(s_buffer, "9", 1);
	tsk_exit();
}



/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
