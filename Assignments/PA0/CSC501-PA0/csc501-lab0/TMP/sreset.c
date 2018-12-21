/* sreset.c - sreset */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;
/*------------------------------------------------------------------------
 *  sreset  --  reset the count and queue of a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL sreset(int sem, int count)
{
	STATWORD ps;    
	struct	sentry	*sptr;
	int	pid;
	int	slist;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SRESET].systemCallName = "sys_sreset";
                globalTableSystemCall[currpid][SRESET].systemCallFrequency = globalTableSystemCall[currpid][SRESET].systemCallFrequency + 1;

        }

	disable(ps);
	if (isbadsem(sem) || count<0 || semaph[sem].sstate==SFREE) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SRESET].systemCallAverageExecTime = globalTableSystemCall[currpid][SRESET].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	sptr = &semaph[sem];
	slist = sptr->sqhead;
	while ((pid=getfirst(slist)) != EMPTY)
		ready(pid,RESCHNO);
	sptr->semcnt = count;
	resched();
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SRESET].systemCallAverageExecTime = globalTableSystemCall[currpid][SRESET].systemCallAverageExecTime + timeTaken;
	return(OK);
}
