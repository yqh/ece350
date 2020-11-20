/**
 * @file:   usr_tasks.c
 * IMPORTANT: This file will be replaced by another file in automated testing.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_tasks.h"
#include "common.h"

void auto_test_2(void)
{
	int ret_val;
	U8* buf;
	buf = mem_alloc(64);
	ret_val = recv_msg(NULL, buf, 64);
	if(ret_val != RTX_ERR) {
        uart1_put_string("Test failed.\r\n");
    } else {
		uart1_put_string("Test passed.\r\n");
	}
	tsk_exit();
}
