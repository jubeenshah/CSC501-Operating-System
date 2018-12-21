/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */
SYSCALL signaln(int sem, int count)
{
	STATWORD ps;    
	struct	sentry	*sptr;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SIGNALN].systemCallName = "sys_signaln";
                globalTableSystemCall[currpid][SIGNALN].systemCallFrequency = globalTableSystemCall[currpid][SIGNALN].systemCallFrequency + 1;

        }
	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE || count<=0) {
		restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SIGNALN].systemCallAverageExecTime = globalTableSystemCall[currpid][SIGNALN].systemCallAverageExecTime + timeTaken;	
	return(SYSERR);
	}
	sptr = &semaph[sem];
	for (; count > 0  ; count--)
		if ((sptr->semcnt++) < 0)
			ready(getfirst(sptr->sqhead), RESCHNO);
	resched();
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SIGNALN].systemCallAverageExecTime = globalTableSystemCall[currpid][SIGNALN].systemCallAverageExecTime + timeTaken;
	return(OK);
}
