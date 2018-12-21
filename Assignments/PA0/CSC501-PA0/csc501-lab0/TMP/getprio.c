/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include "lab0.h"
/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
extern int summarizeProcess;

SYSCALL getprio(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][GETPRIO].systemCallName = "sys_getprio";
                globalTableSystemCall[currpid][GETPRIO].systemCallFrequency = globalTableSystemCall[currpid][GETPRIO].systemCallFrequency + 1;
        }
	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][GETPRIO].systemCallAverageExecTime = globalTableSystemCall[currpid][GETPRIO].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][GETPRIO].systemCallAverageExecTime = globalTableSystemCall[currpid][GETPRIO].systemCallAverageExecTime + timeTaken;
	return(pptr->pprio);
}
