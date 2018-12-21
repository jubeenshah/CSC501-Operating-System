/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>

#include "lock.h"

#define SETONE 	1
#define SETZERO	0
/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	int index;
	STATWORD ps;
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	int checkBADPIDSET = isbadpid(pid) ;
	if (checkBADPIDSET || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == SETZERO)
		xdone();

	dev = pptr->pdevs[SETZERO];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[SETONE];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);

	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRLOCK:
				dequeue(pid);
				int checkLockID = pptr->lockid;
				locks[checkLockID].pidheld[pid]=SETZERO;
				newlprio(checkLockID);
				index = SETZERO;
				while (index < NPROC) {
					/* code */
					int LockIDSet = checkLockID;
					int checkIfSet = locks[LockIDSet].pidheld[index];
					if(checkIfSet == SETONE){
						int newPinHVal = index;
						newpinh(newPinHVal);
					}
					index = index + SETONE;
				}
				pptr->pstate=PRFREE;
				break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return 1;
}
