/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;
/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep100(int n)
{
	STATWORD ps;    
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SLEEP100].systemCallName = "sys_sleep100";
                globalTableSystemCall[currpid][SLEEP100].systemCallFrequency = globalTableSystemCall[currpid][SLEEP100].systemCallFrequency + 1;

        }

	if (n < 0  || clkruns==0){
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SLEEP100].systemCallAverageExecTime = globalTableSystemCall[currpid][SLEEP100].systemCallAverageExecTime + timeTaken;
	         return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep100(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*10);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SLEEP100].systemCallAverageExecTime = globalTableSystemCall[currpid][SLEEP100].systemCallAverageExecTime + timeTaken;
	return(OK);
}
