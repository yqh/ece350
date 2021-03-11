#include "rtx.h"
#include "Serial.h"
#include "printf.h"
extern void kcd_task(void);

/* The KCD Task Template File */

U8 charIsValid(U8 character){
     if (character > 47 && character < 58){
         return 1;
     } else if (character > 64 && character < 91){
    	 return 1;
     } else if (character > 96 && character < 123){
    	 return 1;
     } else if (character == 13 || character == 37){
    	 return 1;
     } else {
         return 0;
     }
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

//		*(recv_buf + ((RTX_MSG_HDR*)recv_buf)->length) = '\0';
//		SER_PutStr(0, recv_buf + sizeof(RTX_MSG_HDR));

		RTX_MSG_HDR* msg_hdr = (RTX_MSG_HDR*)(recv_buf);

        if (msg_hdr->length != sizeof(RTX_MSG_CHAR)){
            // ignore message if data more than 1 char
            SER_PutStr(0, "KCD_IN: Unexpected message length");
            continue;
        }

		// process KCD_REG or KEY_IN type messages
		if (ret_val == RTX_OK && msg_hdr->type == KCD_REG){
			// command registration

			RTX_MSG_CHAR * msg = (RTX_MSG_CHAR*) recv_buf;

			// get command identifier
			U8 cmd_id = msg->data;

			if (charIsAlphaNum(cmd_id) == 0){
				// invalid cmd_id
				SER_PutStr(0, "Reg: Invalid Command ID");
				continue;
			}
			
			// convert cmd_id to ascii and offset
			U8 index = charToIndex(cmd_id);

			// store it in cmd_reg
			cmd_reg[index] = cmd_id;

		} else if (ret_val == RTX_OK && msg_hdr->type == KEY_IN){
			// Keyboard Input

			// get command char
			RTX_MSG_CHAR * msg = (RTX_MSG_CHAR*) recv_buf;
			U8 cmd_char = msg->data;
			cmd_len++;

			// first character should be %
			if ( (cmd_len == 1 && cmd_char != 37) || (cmd_len != 1 && cmd_char == 37) ){
				cmd_invalid = 1;
				SER_PutStr(0, "Key In: Percent not in the right place");
			}

			// command length (including % and enter) larger than 64 bytes
			if (cmd_len > 64){
				cmd_invalid = 1;
				SER_PutStr(0, "Key In: Command Length too long");
			}

			// TODO: write charIsValid(), figure out what is a valid char?
			if (!charIsValid(cmd_char)){
				cmd_invalid = 1;
				SER_PutStr(0, "Key In: Invalid Character Detected");
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
						SER_PutStr(0, "Command cannot be processed\r\n");
						SER_PutStr(0, "Key In: Unregistered cmd_id OR invalid TID");
					}else{

						// send message to registered task id
						int msg_sent = send_msg(recv_tid, msg);

						if (!msg_sent){
							SER_PutStr(0, "Command cannot be processed\r\n");
							SER_PutStr(0, "Key In: Message failed to send");
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
					// enqueue KEY_IN msg data
					cmd_queue[cmd_queue_counter] = cmd_char;
					cmd_queue_counter++;
				}
			}else{
				// invalid command
				// if enter: dequeue and output failure message
				if (cmd_char == 13){
					SER_PutStr(0, "Invalid Command\r\n");

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