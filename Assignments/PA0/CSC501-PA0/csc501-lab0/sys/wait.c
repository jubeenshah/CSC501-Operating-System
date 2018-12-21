/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;
/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL	wait(int sem)
{
	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][WAIT].systemCallName = "sys_wait";
                globalTableSystemCall[currpid][WAIT].systemCallFrequency = globalTableSystemCall[currpid][WAIT].systemCallFrequency + 1;

        }
	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][WAIT].systemCallAverageExecTime = globalTableSystemCall[currpid][WAIT].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][WAIT].systemCallAverageExecTime = globalTableSystemCall[currpid][WAIT].systemCallAverageExecTime + timeTaken;
		return pptr->pwaitret;

	}
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][WAIT].systemCallAverageExecTime = globalTableSystemCall[currpid][WAIT].systemCallAverageExecTime + timeTaken;
	return(OK);
}
