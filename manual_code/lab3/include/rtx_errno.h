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
