/* create.c - create, newpid */

#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <stdio.h>

#define SETZERO   0
#define SETONE    1

LOCAL int newpid();

/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL create(procaddr,ssize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	unsigned long	savsp, *pushsp;
	STATWORD 	ps;
	int		pid;		/* stores new process id	*/
	struct	pentry	*pptr;		/* pointer to proc. table entry */
	int		i;
	unsigned long	*a;		/* points to list of args	*/
	unsigned long	*saddr;		/* stack address		*/
	int		INITRET();

	disable(ps);
	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (int) roundew(ssize);
	if (((saddr = (unsigned long *)getstk(ssize)) ==
	    (unsigned long *)SYSERR ) ||
	    (pid=newpid()) == SYSERR || priority < 1 ) {
		restore(ps);
		return(SYSERR);
	}

	numproc++;
	pptr = &proctab[pid];

	pptr->fildes[0] = SETZERO;	/* stdin set to console */
	pptr->fildes[1] = SETZERO;	/* stdout set to console */
	pptr->fildes[2] = SETZERO;	/* stderr set to console */

	for (i=3; i < _NFILE; i++)	/* others set to unused */
		pptr->fildes[i] = FDFREE;

	pptr->pstate = PRSUSP;
	for (i=SETZERO ; i<PNMLEN && (int)(pptr->pname[i]=name[i])!=0 ; i++)
		;
	pptr->pprio = priority;
	pptr->pbase = (long) saddr;
	pptr->pstklen = ssize;
	pptr->psem = SETZERO;
	pptr->phasmsg = FALSE;
	pptr->plimit = pptr->pbase - ssize + sizeof (long);
	pptr->pirmask[SETZERO] = SETZERO;
	pptr->pnxtkin = BADPID;
	pptr->pdevs[SETZERO] = pptr->pdevs[1] = pptr->ppagedev = BADDEV;

  pptr->pinh    = SETZERO;
  pptr->lockid  = SETONE;
		/* Bottom of stack */
	*saddr = MAGIC;
	savsp = (unsigned long)saddr;

	/* push arguments */
	pptr->pargs = nargs;
	a = (unsigned long *)(&args) + (nargs-1); /* last argument	*/
	for ( ; nargs > SETZERO ; nargs--)	/* machine dependent; copy args	*/
		*--saddr = *a--;	/* onto created process' stack	*/
	*--saddr = (long)INITRET;	/* push on return address	*/

	*--saddr = pptr->paddr = (long)procaddr; /* where we "ret" to	*/
	*--saddr = savsp;		/* fake frame ptr for procaddr	*/
	savsp = (unsigned long) saddr;

/* this must match what ctxsw expects: flags, regs, old SP */
/* emulate 386 "pushal" instruction */
	*--saddr = SETZERO;
	*--saddr = SETZERO;	/* %eax */
	*--saddr = SETZERO;	/* %ecx */
	*--saddr = SETZERO;	/* %edx */
	*--saddr = SETZERO;	/* %ebx */
	*--saddr = SETZERO;	/* %esp; fill in below */
	pushsp = saddr;
	*--saddr = savsp;	/* %ebp */
	*--saddr = SETZERO;		/* %esi */
	*--saddr = SETZERO;		/* %edi */
	*pushsp = pptr->pesp = (unsigned long)saddr;

	restore(ps);
	return(pid);
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL int newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}

// /* create.c - create, newpid */
//
// #include <conf.h>
// #include <i386.h>
// #include <kernel.h>
// #include <proc.h>
// #include <sem.h>
// #include <mem.h>
// #include <io.h>
// #include <stdio.h>
//
// LOCAL int newpid();
//
// /*------------------------------------------------------------------------
//  *  create  -  create a process to start running a procedure
//  *------------------------------------------------------------------------
//  */
// SYSCALL create(procaddr,ssize,priority,name,nargs,args)
// 	int	*procaddr;		/* procedure address		*/
// 	int	ssize;			/* stack size in words		*/
// 	int	priority;		/* process priority > 0		*/
// 	char	*name;			/* name (for debugging)		*/
// 	int	nargs;			/* number of args that follow	*/
// 	long	args;			/* arguments (treated like an	*/
// 					/* array in the code)		*/
// {
// 	unsigned long	savsp, *pushsp;
// 	STATWORD 	ps;
// 	int		pid;		/* stores new process id	*/
// 	struct	pentry	*pptr;		/* pointer to proc. table entry */
// 	int		i;
// 	unsigned long	*a;		/* points to list of args	*/
// 	unsigned long	*saddr;		/* stack address		*/
// 	int		INITRET();
//
// 	disable(ps);
// 	if (ssize < MINSTK)
// 		ssize = MINSTK;
// 	ssize = (int) roundew(ssize);
// 	if (((saddr = (unsigned long *)getstk(ssize)) ==
// 	    (unsigned long *)SYSERR ) ||
// 	    (pid=newpid()) == SYSERR || priority < 1 ) {
// 		restore(ps);
// 		return(SYSERR);
// 	}
//
// 	numproc++;
// 	pptr = &proctab[pid];
//
// 	pptr->fildes[0] = 0;	/* stdin set to console */
// 	pptr->fildes[1] = 0;	/* stdout set to console */
// 	pptr->fildes[2] = 0;	/* stderr set to console */
//
// 	for (i=3; i < _NFILE; i++)	/* others set to unused */
// 		pptr->fildes[i] = FDFREE;
//
// 	pptr->pstate = PRSUSP;
// 	for (i=0 ; i<PNMLEN && (int)(pptr->pname[i]=name[i])!=0 ; i++)
// 		;
// 	pptr->pprio = priority;
// 	pptr->pbase = (long) saddr;
// 	pptr->pstklen = ssize;
// 	pptr->psem = 0;
// 	pptr->phasmsg = FALSE;
// 	pptr->plimit = pptr->pbase - ssize + sizeof (long);
// 	pptr->pirmask[0] = 0;
// 	pptr->pnxtkin = BADPID;
// 	pptr->pdevs[0] = pptr->pdevs[1] = pptr->ppagedev = BADDEV;
//
// 	/*modified*/
// 	pptr->pinh=0;
// 	pptr->lockid=-1;
//
// 		/* Bottom of stack */
// 	*saddr = MAGIC;
// 	savsp = (unsigned long)saddr;
//
// 	/* push arguments */
// 	pptr->pargs = nargs;
// 	a = (unsigned long *)(&args) + (nargs-1); /* last argument	*/
// 	for ( ; nargs > 0 ; nargs--)	/* machine dependent; copy args	*/
// 		*--saddr = *a--;	/* onto created process' stack	*/
// 	*--saddr = (long)INITRET;	/* push on return address	*/
//
// 	*--saddr = pptr->paddr = (long)procaddr; /* where we "ret" to	*/
// 	*--saddr = savsp;		/* fake frame ptr for procaddr	*/
// 	savsp = (unsigned long) saddr;
//
// /* this must match what ctxsw expects: flags, regs, old SP */
// /* emulate 386 "pushal" instruction */
// 	*--saddr = 0;
// 	*--saddr = 0;	/* %eax */
// 	*--saddr = 0;	/* %ecx */
// 	*--saddr = 0;	/* %edx */
// 	*--saddr = 0;	/* %ebx */
// 	*--saddr = 0;	/* %esp; fill in below */
// 	pushsp = saddr;
// 	*--saddr = savsp;	/* %ebp */
// 	*--saddr = 0;		/* %esi */
// 	*--saddr = 0;		/* %edi */
// 	*pushsp = pptr->pesp = (unsigned long)saddr;
// 	restore(ps);
// 	return(pid);
// }
//
// /*------------------------------------------------------------------------
//  * newpid  --  obtain a new (free) process id
//  *------------------------------------------------------------------------
//  */
// LOCAL int newpid()
// {
// 	int	pid;			/* process id to return		*/
// 	int	i;
//
// 	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
// 		if ( (pid=nextproc--) <= 0)
// 			nextproc = NPROC-1;
// 		if (proctab[pid].pstate == PRFREE)
// 			return(pid);
// 	}
// 	return(SYSERR);
// }
//
