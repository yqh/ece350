/**
 * @file:   usr_tasks.c
 * @brief:  Two user/unprivileged  tasks: task1 and task2
 * @author: Yiqing Huang
 * IMPORTANT: This file will be replaced by another file in automated testing.
 */

#include "ae.h"
#include "ae_usr_tasks.h"


/* the following arrays can also be dynamic allocated 
   They do not have to be global buffers. 
 */

extern U32 g_test_result;

void task_rt1()
{
    static int i=0, j=0;
    static task_t sender_tid = PID_NULL;
    struct ae_tick tk;
    RTX_MSG_HDR *msg_hdr;
    struct ae_test_req *p_test_req;
    size_t msg_hdr_size = sizeof(RTX_MSG_HDR);
    size_t s1 = sizeof(struct data_rt);
    size_t s2 = sizeof(struct ae_test_req);
    s1 = (s1 > s2) ? s1 : s2;
    size_t bufsize = msg_hdr_size + s1; 
    
    U8 *buf = mem_alloc(bufsize);
    if (buf == NULL ) {
        printf("task_rt1: mem_alloc failed\r\n");
        tsk_exit();
    }
    
    ae_get_tick(&tk, AE_TIMER2);
#ifdef AE_DEBUG_0
    printf("task_rt1: i=%d: tc = %d, pc = %d\r\n", i++, tk.tc, tk.pc);
#endif // AE_DEBUG_0    
    if ( sender_tid == PID_NULL) {
        if (recv_msg_nb(&sender_tid, buf, bufsize) == RTX_OK) {
            p_test_req = (struct ae_test_req *)(buf + msg_hdr_size);
#ifdef AE_DEBUG_0
            printf("task_rt1: sender_tid = %d, received: 0x%x\r\n", sender_tid, p_test_req->magic_word);  
#endif // AE_DEBUG_0            
            if (p_test_req->magic_word == 0xDEADBEEF ) {
                g_test_result |= BIT(p_test_req->req);   /* test 0 result */
            }
        } 
        mem_dealloc(buf);
        tsk_done_rt();
    }
    
    if ( j++ < 4) {
        struct data_rt *ptr;
        msg_hdr = (RTX_MSG_HDR *)buf;
        msg_hdr->type = AE_RT0;
        msg_hdr->length = bufsize;
        ptr = (struct data_rt *)(buf + msg_hdr_size);
        ptr->seq = i;  /* we can also use j */
        ptr->tick.pc = tk.pc;
        ptr->tick.tc = tk.tc;
        
        if ( send_msg(sender_tid, buf) != RTX_OK ) {
            printf("task_rt1: send_message failed\r\n");
        }            
    }
    
    mem_dealloc(buf);
    tsk_done_rt();
}
