/* The KCD Task Template File */
#include "rtx.h"
#include "Serial.h"
#include "printf.h"
#include "k_inc.h"

#define SIZE 62 // all possible command characters
extern TCB *gp_current_task;

char str[64];
int counter = 0;

char hash_table[MAX_TASKS];

void kcd_task(void)
{
	// create mailbox for kcd task
    mbx_create(KCD_MBX_SIZE);

    for (int i = 0; i < MAX_TASKS; i++) {
    	hash_table[i] = NULL;
    }

    void *ptr = mem_alloc(sizeof(RTX_MSG_HDR) + sizeof(char));


    size_t msg_hdr_size = sizeof(struct rtx_msg_hdr);
    U8 *buf = (char*) mem_alloc(64 + sizeof(struct rtx_msg_hdr));
    struct rtx_msg_hdr *ptr2 = (void *)buf;
    ptr2->length = 0;
    ptr2->type = KCD_CMD;
    buf += msg_hdr_size;

    while(1) {
    	task_t sender_tid = NULL;

    	if (recv_msg(&sender_tid, ptr, sizeof(RTX_MSG_HDR)+sizeof(char)) == -1){
    		continue;
    	}

		RTX_MSG_HDR *hdr = (RTX_MSG_HDR*) ptr;
		size_t msg_hdr_size = sizeof(RTX_MSG_HDR);
		char *c = (char*)ptr + msg_hdr_size;

		// registering command
		if (hdr->type == KCD_REG) {
			hash_table[sender_tid] = *c;

		// keyboard input
		} else if (hdr->type == KEY_IN) {
			if (*c == '\n' || *c == '\r') {
				// ready to send the string
				if (str[0] != '%' || counter > 64) {
					printf("Invalid Command\n");
				} else {
					for (int i = 0; i < counter-1; i++) {
						buf[i] = str[i+1];
					}

					task_t tid = MAX_TASKS;
					for (int i = 0; i < MAX_TASKS; i++) {
						if (hash_table[i] == str[1]) {
							tid = i;
							break;
						}
					}
					if (tid == MAX_TASKS) {
						printf("Command cannot be processed\n");
					}

					ptr2->length = counter-1 + sizeof(RTX_MSG_HDR);
					send_msg(tid, (void *)ptr2);

				}
				counter = 0;

			} else {
				// recieving a character
				str[counter] = *c;
				++counter;
			}
		}
    }
}