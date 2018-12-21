/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

/*------------------------------------------------------------------------
 * sdelete  --  delete a semaphore by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL sdelete(int sem)
{
	STATWORD ps;    
	int	pid;
	struct	sentry	*sptr;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SDELETE].systemCallName = "sys_sdelete";
                globalTableSystemCall[currpid][SDELETE].systemCallFrequency = globalTableSystemCall[currpid][SDELETE].systemCallFrequency + 1;

        }
	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SDELETE].systemCallAverageExecTime = globalTableSystemCall[currpid][SDELETE].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	sptr = &semaph[sem];
	sptr->sstate = SFREE;
	if (nonempty(sptr->sqhead)) {
		while( (pid=getfirst(sptr->sqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SDELETE].systemCallAverageExecTime = globalTableSystemCall[currpid][SDELETE].systemCallAverageExecTime + timeTaken;
	return(OK);
}
