/**
 * @file:   usr_tasks.c
 * IMPORTANT: This file will be replaced by another file in automated testing.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_tasks.h"
#include "common.h"

void auto_test_3(void)
{
	int ret_val;

	ret_val = mbx_create(64);
	ret_val = mbx_create(64);
	if (ret_val != RTX_ERR) {
        uart1_put_string("Test failed.\r\n");
    } else {
		uart1_put_string("Test passed.\r\n");
	}
	tsk_exit();
}
