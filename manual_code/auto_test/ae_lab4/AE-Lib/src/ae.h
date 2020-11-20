/**
 * @brief ECE350 Lab auto testing header file
 */

#include "rtx.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae_timer.h"
#include "ae_usr_tasks.h"
#include "ae_priv_tasks.h"


#ifdef RAM_TARGET
#define IROM_BASE  0x10000000
#else
#define IROM_BASE  0x0
#endif

#define AE_TASKS    5
#define AE_TIMER2   2
#define AE_BUFSIZE 64
#define AE_NUM_JOBS 4     /* number of jobs of a task to examine */


/* AE message types */
#define AE_TEST_REQ 40
#define AE_MSG_U32  50    /* 4 byte data message data*/
#define AE_RT0      100

struct data_rt {
    U32 seq;    /* sequence number */
    struct ae_tick tick;  /* tick */
};

struct ae_test_req {
    U32 magic_word;  /* MAGIC WORD */
    U32 req;         /* test req no. */
};

extern void lcd_task(void);
extern void kcd_task(void);
extern void null_task(void);
extern void wall_clock_task(void);

