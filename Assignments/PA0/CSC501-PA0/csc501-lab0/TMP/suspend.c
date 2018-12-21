/* suspend.c - suspend */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

/*------------------------------------------------------------------------
 *  suspend  --  suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
SYSCALL	suspend(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority returned		*/
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SUSPEND].systemCallName = "sys_suspend";
                globalTableSystemCall[currpid][SUSPEND].systemCallFrequency = globalTableSystemCall[currpid][SUSPEND].systemCallFrequency + 1;

        }
	disable(ps);
	if (isbadpid(pid) || pid==NULLPROC ||
	 ((pptr= &proctab[pid])->pstate!=PRCURR && pptr->pstate!=PRREADY)) {
		restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SUSPEND].systemCallAverageExecTime = globalTableSystemCall[currpid][SUSPEND].systemCallAverageExecTime + timeTaken;

		return(SYSERR);
	}
	if (pptr->pstate == PRREADY) {
		pptr->pstate = PRSUSP;
		dequeue(pid);
	}
	else {
		pptr->pstate = PRSUSP;
		resched();
	}
	prio = pptr->pprio;
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SUSPEND].systemCallAverageExecTime = globalTableSystemCall[currpid][SUSPEND].systemCallAverageExecTime + timeTaken;

	return(prio);

}
