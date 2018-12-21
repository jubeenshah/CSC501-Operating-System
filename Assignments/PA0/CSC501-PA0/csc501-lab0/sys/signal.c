/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;
/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
SYSCALL signal(int sem)
{
	STATWORD ps;    
	register struct	sentry	*sptr;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SIGNAL].systemCallName = "sys_signal";
                globalTableSystemCall[currpid][SIGNAL].systemCallFrequency = globalTableSystemCall[currpid][SIGNAL].systemCallFrequency + 1;

        }
	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SIGNAL].systemCallAverageExecTime = globalTableSystemCall[currpid][SIGNAL].systemCallAverageExecTime + timeTaken;

		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SIGNAL].systemCallAverageExecTime = globalTableSystemCall[currpid][SIGNAL].systemCallAverageExecTime + timeTaken;
	return(OK);
}
