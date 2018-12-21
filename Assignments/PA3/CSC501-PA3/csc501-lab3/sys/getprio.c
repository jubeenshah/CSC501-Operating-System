/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

#define SETZERO 	0
/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
	STATWORD ps;
	struct	pentry	*pptr;

	disable(ps);
	pptr = &proctab[pid];
	int checkPPTR = pptr->pstate;
	int checkISBadPID = isbadpid(pid);
	if (checkISBadPID || checkPPTR == PRFREE) {
		restore(ps);
		return -1;
	}
	restore(ps);
	if (pptr->pinh == SETZERO) {
		/* code */
		int returnValue = pptr->pprio;
		return(returnValue);
	}
	int returnPinhVal = pptr->pinh;
	return(returnPinhVal);

}
