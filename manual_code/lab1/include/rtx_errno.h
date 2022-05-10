/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2022 Yiqing Huang
 *                          All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice and the following disclaimer.
 *
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************
 */

/**************************************************************************//**
 * @file        rtx_errno.h
 * @brief       RTX errno header file
 *
 * @version     V1.2022.05
 * @authors     Yiqing Huang
 * @date        2022 MAY 
 * @see         common.h
 * @see         rtx_ext.h
 *
 * @attention   DO NOT MODIFY
 *
 *****************************************************************************/

#ifndef RTX_ERRNO_H_
#define RTX_ERRNO_H_

extern  int         errno;

#define EPERM       1   /* Operation not permitted */
#define ENOENT      2   /* No such file or directory */
#define EAGAIN      11  /*  Try again */
#define ENOMEM      12  /* Out of memory */
#define EFAULT      14  /* Bad address */
#define EEXIST      17  /* File exists */
#define EINVAL      22  /* Invalid argument */
#define ENOSPC      28  /* No space left on device */
#define ENOMSG      42  /* No message of desired type */
#define EMSGSIZE    90  /* Message too long */


#endif // !RTX_ERRNO_H_
