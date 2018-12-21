/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include "lab0.h"


/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
extern int summarizeProcess;

SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
	if(!summarizeProcess) {

                globalTableSystemCall[currpid][CHPRIO].systemCallName = "sys_chprio";
                globalTableSystemCall[currpid][CHPRIO].systemCallFrequency = globalTableSystemCall[currpid][FREEMEM].systemCallFrequency + 1;

        }
	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][CHPRIO].systemCallAverageExecTime = globalTableSystemCall[currpid][CHPRIO].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	globalTableSystemCall[currpid][CHPRIO].systemCallAverageExecTime = globalTableSystemCall[currpid][CHPRIO].systemCallAverageExecTime + timeTaken;
	return(newprio);
}
