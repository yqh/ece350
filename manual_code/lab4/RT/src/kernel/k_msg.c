/**
 * @file:   k_msg.c
 * @brief:  kernel message passing routines
 * @author: Yiqing Huang
 * @date:   2020/10/09
 */

#include "k_msg.h"
#include "common_ext.h"

#ifdef DEBUG_0
#include "printf.h"
#include "Serial.h"
#endif /* ! DEBUG_0 */

// data_end can be less than data_start. Then the data wraps from
// start_data through buffer_end, back to buffer_start, to data_end.
typedef struct circular_buffer {
	U32 buffer_start; // Start of buffer in memory
	U32 buffer_end; // End of buffer in memory
	U32 data_start; // Start of "valid data" in memory
	U32 data_end; // End of "valid data" in memory.
} __attribute__((aligned(8)))C_BUFFER;

typedef struct c_metadata {
	task_t sender_tid;
} __attribute__((aligned(8)))C_METADATA;

void mem_cpy(void* source, void* dest, size_t len) {
	for (int i = 0; i < len; i++) {
		*((char*)dest+i) = *((char*)source+i);
	}
}

// Prereq: addr is aligned
U32 align_increment(U32 addr, U32 increment, U32 alignment) {
	if ((addr + increment) % alignment == 0) {
		return addr + increment;
	}

	return addr + increment + (alignment - increment % alignment);
}


// TODO: Do not wrap the header because I need the
// struct intact to read the length properly
// E.g., just wrap if buffer_end - data_start < RTX_MSG_HDR
int c_insert(C_BUFFER* mailbox, void* buf) {

	RTX_MSG_HDR* hdr = buf;
	C_METADATA meta = {gp_current_task->tid};

    U32 mailbox_len = mailbox->buffer_end - mailbox->buffer_start;
    U32 length = hdr->length + sizeof(C_METADATA);
    if (mailbox_len < length) {
    	return RTX_ERR;
    }

    // If data_end < data_start, data_end + header->length
    // must not exceed data_start
    if (mailbox->data_end < mailbox->data_start) {
    	if (mailbox->data_end + length >= mailbox->data_start) {
    		return RTX_ERR;
    	}

    	mem_cpy(&meta, (void*)mailbox->data_end, sizeof(C_METADATA));
    	mem_cpy(buf,
    			(void*)(mailbox->data_end + sizeof(C_METADATA)),
				length - sizeof(C_METADATA));
    	mailbox->data_end = align_increment(mailbox->data_end, length, 8);

        if (mailbox->data_end >= mailbox->buffer_end - (sizeof(C_METADATA) + sizeof(RTX_MSG_HDR))) {
        	mailbox->data_end = mailbox->buffer_start;
        }

    	return RTX_OK;
    }

    if (mailbox->data_end + length <= mailbox->buffer_end) {
    	mem_cpy(&meta, (void*)mailbox->data_end, sizeof(C_METADATA));
    	mem_cpy(buf,
    			(void*)(mailbox->data_end + sizeof(C_METADATA)),
				length - sizeof(C_METADATA));
    	mailbox->data_end = align_increment(mailbox->data_end, length, 8);

        if (mailbox->data_end >= mailbox->buffer_end - (sizeof(C_METADATA) + sizeof(RTX_MSG_HDR))) {
        	mailbox->data_end = mailbox->buffer_start;
        }

    	return RTX_OK;
    }

    //
    U32 first_copy_length = mailbox->buffer_end - mailbox->data_end;


    mem_cpy(&meta, (void*)mailbox->data_end, sizeof(C_METADATA));
    mailbox->data_end = align_increment(mailbox->data_end, sizeof(C_METADATA), 8);

    mem_cpy(buf, (void*)mailbox->data_end, sizeof(RTX_MSG_HDR));
    mailbox->data_end = align_increment(mailbox->data_end, sizeof(RTX_MSG_HDR), 8);

    mem_cpy((char*)buf + sizeof(RTX_MSG_HDR),
    		(void*)mailbox->data_end
			, first_copy_length - sizeof(C_METADATA) - sizeof(RTX_MSG_HDR));
    mailbox->data_end = mailbox->buffer_start;

    mem_cpy((char*)buf + first_copy_length - sizeof(C_METADATA),
    		(void*)mailbox->data_end
			, length - first_copy_length);

    mailbox->data_end = align_increment(mailbox->data_end, length - first_copy_length, 8);

    if (mailbox->data_end >= mailbox->buffer_end - (sizeof(C_METADATA) + sizeof(RTX_MSG_HDR))) {
    	mailbox->data_end = mailbox->buffer_start;
    }

    return RTX_OK;
}

int k_mbx_create(size_t size) {
#ifdef DEBUG_0
    printf("k_mbx_create: size = %d\r\n", size);
#endif /* DEBUG_0 */

    if (gp_current_task->mailbox || size < MIN_MBX_SIZE) {
    	return RTX_ERR;
    }

    // Memory layout: Buffer_data | Buffer size
    size_t total_size = size + sizeof(C_BUFFER);
    void *mem = k_mem_alloc(total_size);
    if (!mem) {
    	return RTX_ERR;
    }


    // Init of C_BUFFER struct
    C_BUFFER *buffer = mem;
    buffer->buffer_start = (U32)buffer + sizeof(C_BUFFER);
    buffer->buffer_end   = (U32)buffer + total_size;
    buffer->data_start   = buffer->buffer_start;
    buffer->data_end     = buffer->data_start;

    gp_current_task->mailbox = (U32)buffer;

    return 0;
}

int k_send_msg(task_t receiver_tid, const void *buf) {
#ifdef DEBUG_0
    printf("k_send_msg: receiver_tid = %d, buf=0x%x\r\n", receiver_tid, buf);
#endif /* DEBUG_0 */
    if (receiver_tid >= MAX_TASKS || !buf) {
    	return RTX_ERR;
    }

    TCB* tcb = &g_tcbs[receiver_tid];
    // TODO: Verify that a receiver having no mailbox is an error
    if (tcb->state == DORMANT || !tcb->mailbox) {
    	return RTX_ERR;
    }

    RTX_MSG_HDR* header = (RTX_MSG_HDR*)buf;
    if (header->length < MIN_MSG_SIZE) {
    	return RTX_ERR;
    }

    C_BUFFER* mailbox = (C_BUFFER*)tcb->mailbox;
    int ret_val = c_insert(mailbox, header);

    if (ret_val == RTX_ERR) {
    	return ret_val;
    }

    // unblocking procedure
    if (tcb->state == BLK_MSG) {
    	k_tsk_unblock(tcb);
    }

    return RTX_OK;
}

int k_recv_msg(task_t *sender_tid, void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg: sender_tid  = 0x%x, buf=0x%x, len=%d\r\n", sender_tid, buf, len);
#endif /* DEBUG_0 */

    TCB tcb = *gp_current_task;
    if (!buf || !tcb.mailbox) {
    	return RTX_ERR;
    }

    C_BUFFER* mailbox = (C_BUFFER*)tcb.mailbox;

    // Prevents the header itself wrapping
    if (mailbox->data_start >= mailbox->buffer_end - (sizeof(C_METADATA) + sizeof(RTX_MSG_HDR))) {
    	mailbox->data_start = mailbox->buffer_start;
    }

    // No messages to receive
    while (mailbox->data_start == mailbox->data_end) {
    	k_tsk_block();
    }

    C_METADATA * meta = (C_METADATA*) mailbox->data_start;
    RTX_MSG_HDR* msg = (RTX_MSG_HDR*)(meta + 1);
    if (len < msg->length) {
    	return RTX_ERR;
    }

    // Process one message
    *sender_tid = meta->sender_tid;
    mailbox->data_start += sizeof(C_METADATA);
    if (msg->length + mailbox->data_start <= mailbox->buffer_end) {
    	mem_cpy((void*)(mailbox->data_start),
    			buf,
				msg->length);
    	mailbox->data_start = align_increment(mailbox->data_start, msg->length, 8);
    } else {

    	U32 free_space = mailbox->buffer_end - mailbox->data_start;

    	mem_cpy((void*)((U32)mailbox->data_start),
    			buf,
				free_space);
    	mem_cpy((void*)mailbox->buffer_start,
    			(char*)buf + free_space,
				msg->length - free_space);

    	mailbox->data_start = align_increment(mailbox->buffer_start, msg->length - free_space, 8);
    }

    return RTX_OK;
}

int k_mbx_ls(task_t *buf, int count) {
#ifdef DEBUG_0
    printf("k_mbx_ls: buf=0x%x, count=%d\r\n", buf, count);
#endif /* DEBUG_0 */
    return 0;
}


int k_recv_msg_nb(task_t *sender_tid, void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg_nb: sender_tid  = 0x%x, buf=0x%x, len=%d\r\n", sender_tid, buf, len);
#endif /* DEBUG_0 */
    return 0;
}

