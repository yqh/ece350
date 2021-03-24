/**
 * @file:   k_sem.h
 * @brief:  kernel semaphore header file
 * @author: Yiqing Huang
 * @date:   2020/10/05
 */
 
#ifndef K_MSG_H_
#define K_MSG_H_

#include "k_rtx.h"

int k_mbx_create(size_t size);
int k_send_msg(task_t receiver_tid, const void *buf);
int k_recv_msg(task_t *sender_tid, void *buf, size_t len);
int k_recv_msg_nb(task_t *sender_tid, void *buf, size_t len);
int k_mbx_ls(task_t *buf, int count);

#endif /* ! K_MSG_H_ */
