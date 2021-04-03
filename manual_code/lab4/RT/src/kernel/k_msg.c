/**
 * @file:   k_msg.c
 * @brief:  kernel message passing routines
 * @author: Yiqing Huang
 * @date:   2020/10/09
 */

#include "k_msg.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

extern TCB *gp_current_task;
extern TCB g_tcbs[MAX_TASKS];

int k_mbx_create_task(task_t tid, size_t size) {
    // new self defined function to create mailbox for another task 
    // primarily used for creating a mailbox for a real time task 
    #ifdef DEBUG_0
    printf("k_mbx_create: size = %d\r\n", size);
    #endif /* DEBUG_0 */
    // create a mailboxc for the calling task 
    // size argument - capactiy of the mailbox in bytes
    // capacity used for emssages and meta data kernel might need for each message to manage mailbox 
    // mailbox serves messages using FCFS
    // each task will have at most 1 mailbox
    // owner of memory allocated to each task's mailbox is kernel 
    // memory for mailbox is deallocated when task exits

    TMB *t_mb = &t_mailbox[tid];
    // available memory at run time is not enough to create requested mailbox
    t_mb-> mb_buffer = (U32*) k_mem_alloc_task(tid, size); 
    if (t_mb-> mb_buffer == NULL) {
        // not enough memory at run time to create mailbox 
        return -1; 
    }
    // should zero-initailize the memory chunk so it is easier to analyze the data 
    unsigned char* p = (unsigned char*) t_mb-> mb_buffer;
    int len = size; 
    while(len--) {
        *p++ = 0; 
    }
    // otherwise the allocation works so can fill in the other field
    t_mb-> mb_capacity = size;
    U8 *tmp_mb_buffer = (U8 *)t_mb->mb_buffer + size;
    t_mb-> mb_buffer_end = (unsigned int*)tmp_mb_buffer;
    t_mb-> mb_head = (MSG*) t_mb-> mb_buffer;
    t_mb-> mb_tail = NULL;
    t_mb-> mb_head-> next = NULL; 
    t_mb-> mb_tail-> next = NULL;
    return 0;
}

int k_mbx_create(size_t size) {
#ifdef DEBUG_0
    printf("k_mbx_create: size = %d\r\n", size);
#endif /* DEBUG_0 */
    // create a mailboxc for the calling task 
    // size argument - capactiy of the mailbox in bytes
    // capacity used for emssages and meta data kernel might need for each message to manage mailbox 
    // mailbox serves messages using FCFS
    // each task will have at most 1 mailbox
    // owner of memory allocated to each task's mailbox is kernel 
    // memory for mailbox is deallocated when task exits

    // possible cases of failure 
    // calling task already has a mailbox
    TMB *t_mb = &t_mailbox[gp_current_task->tid];
    if (t_mb-> mb_capacity > 0) {
        return -1;
    }
    // size argument < MIN_MBX_SIZE
    if (size < MIN_MBX_SIZE) {
        return -1; 
    }
    // available memory at run time is not enough to create requested mailbox
    t_mb-> mb_buffer = (U32*) k_mem_alloc(size); 
    if (t_mb-> mb_buffer == NULL) {
        // not enough memory at run time to create mailbox 
        return -1; 
    }
    // should zero-initailize the memory chunk so it is easier to analyze the data 
    unsigned char* p = (unsigned char*) t_mb-> mb_buffer;
    int len = size; 
    while(len--) {
        *p++ = 0; 
    }
    // otherwise the allocation works so can fill in the other field
    t_mb-> mb_capacity = size; 
    gp_current_task-> num_msgs = 0; 
    U8 *tmp_mb_buffer = (U8 *)t_mb->mb_buffer + size;
    t_mb-> mb_buffer_end = (unsigned int*)tmp_mb_buffer;
    t_mb-> mb_head = (MSG*) t_mb-> mb_buffer;
    t_mb-> mb_tail = NULL;
    t_mb-> mb_head-> next = NULL; 
    t_mb-> mb_tail-> next = NULL;
    return 0;
    // implement mailbox as a ring buffer
}

int k_send_msg(task_t receiver_tid, const void *buf) {
#ifdef DEBUG_0
    printf("k_send_msg: receiver_tid = %d, buf=0x%x\r\n", receiver_tid, buf);
#endif /* DEBUG_0 */
    // send a message to the mailbox of task
    // data specified by buf to amilbox identifed by receiver_tid
    // if task state BLK_MSG, it becomes unblocked 
    // if prio of unblocked task > currently running task, 
    // then unblocked task preempts currently running task, 
    // currently running task is added to queue
    // if prio is not higher, added to back of ready queue
    // message buf starts with message header followed by actual message data
    TCB *p_tcb = &g_tcbs[receiver_tid];
    TMB *t_mb = &t_mailbox[receiver_tid];
    // causes of failure 
    // receiver_tid does not exist or is dormant
    if (receiver_tid >= MAX_TASKS || p_tcb-> state == DORMANT) {
        return -1; 
    }
    // receiver_tid does not have a mailbox 
    if (t_mb-> mb_buffer == NULL) {
        return -1; 
    }
    // buf is a null pointer 
    if (buf == NULL) {
        return -1; 
    }
    // length field in buf < MIN_MSG_SIZE
    RTX_MSG_HDR *header_msg = (RTX_MSG_HDR*) buf;
    if (header_msg-> length < MIN_MSG_SIZE) { 
        return -1;
    }
    // receiver_tid mailbox does not have enough free space for message 
    // sending a message will move the tail pointer
    // receiving a message will move the head pointer 
    // mailbox on the heap 
    // msg_send is where head will point to after this write
    // create a pointer that points to the the task that we want to send the message to

    // create a struct pointer that points to the head of the queue
    MSG *msg_send;
    // msg_send will be different for the first time a message is sent since the next prop will be invalid
    if (t_mb-> mb_head-> next == NULL) {
        msg_send = t_mb-> mb_head;
    } else {
        msg_send = t_mb-> mb_head->next;
    }


    unsigned int temp = header_msg-> length + sizeof(MSG);
    int padding = temp % 4 == 0 ? 0 : 4 - (temp % 4);
    temp += padding;

    // check to see if free space from head to end of queue 
    if (((unsigned int) msg_send) + temp > (unsigned int) t_mb-> mb_buffer_end) {
        // can't write without overflow, can only overwrite read messages at the beginning  (circle back)
        // set msg_send to point at the start of the queue again (circle back)
        msg_send = (MSG*)t_mb-> mb_buffer;
        // check enough space between front of queue and tail 
        if (((unsigned int) msg_send) + temp > (unsigned int) t_mb-> mb_tail) {
            // not enough space
            return -1; 
        } else {
            // there is enough space so I can overwrite messages that have been received 
            // set head-> next to point to msg_send which has now circled back to the beginning of the queue 
            t_mb-> mb_head-> next = msg_send;
        }
    }
    // can set the meta data for the message
    msg_send-> sender_id = gp_current_task->tid; 
    // the next will point to the end of this message address
    unsigned int endAddr = ((unsigned int) msg_send) + temp;
    msg_send-> next = (MSG*) endAddr;
    // can just memcpy implementation copy from buf to mailbox tail 
    char *csrc = (char*) buf;
    char *cdest = (char*) msg_send + sizeof(MSG);
    for (int i = 0; i < header_msg-> length; i++) {
        cdest[i] = csrc[i]; 
    }

    ++p_tcb-> num_msgs;
    t_mb-> mb_head = msg_send; 
    // preempt the task based on priority ordering 
    // check if receiver is blocked, changed to unblocked if it is
    if (p_tcb-> state == BLK_MSG) {
        p_tcb-> state = READY;
        k_tsk_run_new();
        // run to check for preemption
    }
    return 0;
}

int k_recv_msg(task_t *sender_tid, void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg: sender_tid  = 0x%x, buf=0x%x, len=%d\r\n", sender_tid, buf, len);
#endif /* DEBUG_0 */
    // receives a message from its mailbox if there are any 
    // gets blocked if there are none 
    // buf will get filled with the received message 

    // possible causes of failure 
    // calling task does not have a mailbox 
    TMB *t_mb = &t_mailbox[gp_current_task->tid];
    if (t_mb-> mb_buffer == NULL) {
        return -1; 
    }
    // buf argument is a null pointer 
    if (buf == NULL) {
        return -1; 
    }
    if (gp_current_task-> num_msgs == 0) {
        // set the state to be blocked
        gp_current_task-> state = BLK_MSG; 
        k_tsk_run_new(); 
        return 0;
    }

    if (t_mb-> mb_tail-> next == NULL) {
        t_mb->mb_tail = (MSG*) t_mb->mb_buffer;
    } else {
        t_mb->mb_tail = t_mb-> mb_tail->next;
    }

    // can fail if the buffer is too small to hold the message
    RTX_MSG_HDR *msg_head = (RTX_MSG_HDR*) t_mb-> mb_tail + 1;
    if (msg_head-> length > len) {
        return -1; 
    }

    if (sender_tid != NULL) {
        // fill with sender task ID
        *sender_tid = t_mb-> mb_tail->sender_id;
    }
    // fill the buf with the received message (memcpy same as in send)
    char *csrc = (char*) msg_head;
    char *cdest = (char*) buf;


    for (int i = 0; i < msg_head->length; i++) {
        cdest[i] = csrc[i]; 
    }

    // message received, the number of messages is now reduced
    --gp_current_task-> num_msgs; 

    // move the tail pointer to the next message
    if (gp_current_task->num_msgs == 0) {
        t_mb-> mb_head = (MSG*) t_mb-> mb_buffer;
        t_mb-> mb_tail = NULL;
        t_mb-> mb_head-> next = NULL;
        t_mb-> mb_tail-> next = NULL;
    }

    return 0;
}

int k_recv_msg_nb(task_t *sender_tid, void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg: sender_tid  = 0x%x, buf=0x%x, len=%d\r\n", sender_tid, buf, len);
#endif /* DEBUG_0 */
    // receives a message from its mailbox if there are any 
    // returns if there are none 
    // buf will get filled with the received message 

    // possible causes of failure 
    // calling task does not have a mailbox 
    TMB *t_mb = &t_mailbox[gp_current_task->tid];
    if (t_mb-> mb_buffer == NULL) {
        return -1; 
    }
    // buf argument is a null pointer 
    if (buf == NULL) {
        return -1; 
    }
    if (gp_current_task-> num_msgs == 0) {
        // returns if there are none
        return -1; 
    }

    if (t_mb-> mb_tail-> next == NULL) {
        t_mb->mb_tail = (MSG*) t_mb->mb_buffer;
    } else {
        t_mb->mb_tail = t_mb-> mb_tail->next;
    }

    // can fail if the buffer is too small to hold the message
    RTX_MSG_HDR *msg_head = (RTX_MSG_HDR*) t_mb-> mb_tail + 1;
    if (msg_head-> length > len) {
        return -1; 
    }

    if (sender_tid != NULL) {
        // fill with sender task ID
        *sender_tid = t_mb-> mb_tail->sender_id;
    }
    // fill the buf with the received message (memcpy same as in send)
    char *csrc = (char*) msg_head;
    char *cdest = (char*) buf;


    for (int i = 0; i < msg_head->length; i++) {
        cdest[i] = csrc[i]; 
    }

    // message received, the number of messages is now reduced
    --gp_current_task-> num_msgs; 

    // move the tail pointer to the next message
    if (gp_current_task->num_msgs == 0) {
        t_mb-> mb_head = (MSG*) t_mb-> mb_buffer;
        t_mb-> mb_tail = NULL;
        t_mb-> mb_head-> next = NULL;
        t_mb-> mb_tail-> next = NULL;
    }
    return 0;
}

int k_mbx_ls(task_t *buf, int count) {
#ifdef DEBUG_0
    printf("k_mbx_ls: buf=0x%x, count=%d\r\n", buf, count);
#endif /* DEBUG_0 */
    return 0;
}
