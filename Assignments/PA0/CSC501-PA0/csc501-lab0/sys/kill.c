/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include "lab0.h"

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
extern int summarizeProcess;

SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][KILL].systemCallName = "sys_kill";
                globalTableSystemCall[currpid][KILL].systemCallFrequency = globalTableSystemCall[currpid][KILL].systemCallFrequency + 1;
        }
	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		 timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][KILL].systemCallAverageExecTime = globalTableSystemCall[currpid][KILL].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	 timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][KILL].systemCallAverageExecTime = globalTableSystemCall[currpid][KILL].systemCallAverageExecTime + timeTaken;
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	 timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][KILL].systemCallAverageExecTime = globalTableSystemCall[currpid][KILL].systemCallAverageExecTime + timeTaken;
	return(OK);
}
