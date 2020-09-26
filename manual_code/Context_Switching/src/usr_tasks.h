/**
 * @file:   usr_proc.h
 * @brief:  Two user processes header file
 * @author: Yiqing Huang
 * @date:   2020/08/07
 */
 
#ifndef USR_PROC_H_
#define USR_PROC_H_

#ifdef SIM_TARGET       /* using the simulator is slow */
#define DELAY 500000
#else
#define DELAY 50000000
#endif /* SIM_TARGET */
void set_test_procs(void);
void task1(void);
void task2(void);

#endif /* USR_PROC_H_ */
