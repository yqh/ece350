/**
 * @file:   ae_priv_tasks.c
 * @author: Y. Huang
 * @brief:  privileted tasks
 */

#include "ae.h"
extern U32 g_test_result;

struct data_rt g_data[AE_NUM_JOBS];

void set_test_task(RTX_TASK_INFO *task)
{
    task->u_stack_size = 0x100;
    task->prio = HIGH;
    task->priv = 1;
    task->ptask = &task_test_manager;
}

task_t create_rt()
{
    task_t tid;
    TASK_RT task_rt;
    RTX_MSG_HDR msg_hdr;
    
#ifdef SIM_TARGET
    task_rt.p_n.sec = 0;
    task_rt.p_n.usec = 100000;
#elif RAM_TARGET
    task_rt.p_n.sec = 1;
    task_rt.p_n.usec = 0;
#endif
    task_rt.priv = 0;
    task_rt.u_stack_size = 0x100;
    task_rt.task_entry = &task_rt1;
    
    msg_hdr.type = AE_TEST_REQ;
    msg_hdr.length = sizeof(RTX_MSG_HDR) + sizeof(struct ae_test_req);
    
    tsk_create_rt(&tid, &task_rt, &msg_hdr, 1); 
    
    return tid;
}


void task_test_manager()
{
    
    int i =0;
    task_t sender_tid;
    task_t tid;
    size_t msg_hdr_size = sizeof(RTX_MSG_HDR);
    RTX_MSG_HDR* msg_hdr;
    U8* buf;
    struct ae_test_req *p_test_req;
    struct data_rt *pdata = g_data;
        
    buf = mem_alloc(AE_BUFSIZE);
    if (buf == NULL ) {
        printf("task_test_manager: mem_alloc failed!\r\n");
        tsk_exit();
    }
    
    if (mbx_create(AE_BUFSIZE) != RTX_OK ) {
        printf("task_test_manager: mbx_create failed!\r\n");
        mem_dealloc(buf);
        tsk_exit();
    }
    
    msg_hdr = (RTX_MSG_HDR *)buf;
    msg_hdr->type = AE_TEST_REQ;
    msg_hdr->length = msg_hdr_size + sizeof(struct ae_test_req);
    p_test_req = (struct ae_test_req *) (buf + msg_hdr_size);
    p_test_req->magic_word = 0xDEADBEEF;
    p_test_req->req = 0;
    
    tid = create_rt();
    
    if (send_msg(tid, buf) != RTX_OK) {
        printf("task_test_manager: send_msg failed!\r\n");
        mem_dealloc(buf);
        tsk_exit();
    }
    while (1) {
        printf("i = %d, test_result = %d\r\n", i, g_test_result);
        if ( recv_msg(&sender_tid, buf, AE_BUFSIZE) != RTX_OK ) {
            printf("task_test_manager: recv_msg failed!\r\n");
            continue;
        }
       
        msg_hdr = (RTX_MSG_HDR *)buf;
        struct data_rt *ptr = (struct data_rt *)(buf + msg_hdr_size);
        switch (msg_hdr->type) {
            case AE_RT0:
                pdata->seq = ptr->seq; 
                pdata->tick.tc = ptr->tick.tc;
                pdata->tick.pc = ptr->tick.pc;         
#ifdef AE_DEBUG_0
                printf("AE_RT0: seq = %d, tc = %d, pc = %d\r\n", pdata->seq, pdata->tick.tc, pdata->tick.pc); 
#endif // AE_DEBUG_0
                pdata++;
                i++;
                if ( i == AE_NUM_JOBS ) {
                    if ( get_test_result(g_data) == 1) {
                        g_test_result |= BIT(1);          /* test 1 result */
                    }
                }
                break;
            default:
                break;
        }
    }
    
}

/**
 * @brief evaluate a sequence of timing data
 * @param data: test data
 * @return 1 means pass, zero means fail 
 */
int get_test_result(void *data)
{
    /* analyze data, if pass return 1, otherwise return 0 */
    return 1;
}
