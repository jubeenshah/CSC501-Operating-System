/* bufpool.h */

#ifndef _BUFPOOL_H_
#define _BUFPOOL_H_

#include <mark.h>

#ifndef	NBPOOLS
#define	NBPOOLS	20			/* Maximum number of pools	*/
#endif
#ifndef	BPMAXB
#define	BPMAXB	131072 /* 128k */	/* Maximum buffer length	*/
#endif
#define	BPMINB	2			/* Minimum buffer length	*/
#ifndef	BPMAXN
#define	BPMAXN	256			/* Maximum buffers in any pool	*/
#endif
struct	bpool	{			/* Description of a single pool	*/
	int	bpsize;			/* size of buffers in this pool	*/
	int	bpmaxused;		/* max ever in use		*/
	int	bptotal;		/* # buffers this pool		*/
	char	*bpnext;		/* pointer to next free buffer	*/
	int	bpsem;			/* semaphore that counts buffers*/
};					/*  currently in THIS pool	*/

extern  struct  bpool bptab[];		/* Buffer pool table		*/
extern  int     nbpools;		/* current number of pools	*/
#ifdef  MEMMARK
extern  MARKER  bpmark;
#endif

/* ANSI compliant function prototypes */

int freebuf(void *buf);
int *getbuf(int poolid);
int mkpool(int bufsiz, int numbufs);
int *nbgetbuf(int poolid);
int poolinit();

#endif
