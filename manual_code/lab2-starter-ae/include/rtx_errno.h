#ifndef RTX_ERRNO_H_
#define RTX_ERRNO_H_

extern  int         errno;

#define	EPERM       1	/* Operation not permitted */
#define EAGAIN      11  /*  Try again */
#define ENOMEM      12  /* Out of memory */
#define EFAULT      14  /* Bad address */
#define	EINVAL      22	/* Invalid argument */
#define	ENOSPC		28	/* No space left on device */


#endif // !RTX_ERRNO_H_
