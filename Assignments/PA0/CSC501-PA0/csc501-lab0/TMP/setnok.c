/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */
SYSCALL	setnok(int nok, int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SETNOK].systemCallName = "sys_setnok";
                globalTableSystemCall[currpid][SETNOK].systemCallFrequency = globalTableSystemCall[currpid][SETNOK].systemCallFrequency + 1;

        }
	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SETNOK].systemCallAverageExecTime = globalTableSystemCall[currpid][SETNOK].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SETNOK].systemCallAverageExecTime = globalTableSystemCall[currpid][SETNOK].systemCallAverageExecTime + timeTaken;
	return(OK);
}
