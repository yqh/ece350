## ECE 350 Fall 2020 RTX Project Repository
This branch is for the Fall 2020 Lab. The Hardware is Keil LPC1768 Evaluation Board. The processor is ARM Cortex-M3. 

### `manual`
This directory contains the lab manual. The lab manual github code base links all refer the master brach, which was the main branch in Fall 2020, but no longer the main brach after F2020. You should replace master branch with this branch in the url links in the lab manual for now. 

### `manual_code`

* `util`: printf, uart polling code and debugger script for RAM target.
* `HelloWorld`: a uv5 project that output to UART0 and UART1 by polling.
* `SVC`: lab1 skeleton file. It demonstrates how to use SVC as a gateway to write system calls.
* `Context_Switching`: lab2 skeleton file. It demonstrates how to context switch between two privileged tasks.
* `Context_Switching_UART0_IRQ`: lab3 skeleton file. It demonstrates how to context switching between two privileged tasks by software or hardware interrupts.
* `UART0_irq`: lab3 starter file. It demonstrates how to echo a key press.

### `submission`
This directory contains template files for project submissions.
