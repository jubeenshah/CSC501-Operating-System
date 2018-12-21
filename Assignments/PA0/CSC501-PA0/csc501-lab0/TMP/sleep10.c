/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep10(int n)
{
	STATWORD ps;    
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SLEEP10].systemCallName = "sys_sleep10";
                globalTableSystemCall[currpid][SLEEP10].systemCallFrequency = globalTableSystemCall[currpid][SLEEP10].systemCallFrequency + 1;
		//kprintf("SLEEP10 FREQ");
        }
	if (n < 0  || clkruns==0){
	        timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SLEEP10].systemCallAverageExecTime = globalTableSystemCall[currpid][SLEEP10].systemCallAverageExecTime + timeTaken; 
		return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep10(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*100);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SLEEP10].systemCallAverageExecTime = globalTableSystemCall[currpid][SLEEP10].systemCallAverageExecTime + timeTaken;
	//kprintf("Freq, %d : Time %d, currpid: %d",globalTableSystemCall[currpid][SLEEP10].systemCallFrequency, globalTableSystemCall[currpid][SLEEP10].systemCallAverageExecTime, currpid);
	return(OK);
}
