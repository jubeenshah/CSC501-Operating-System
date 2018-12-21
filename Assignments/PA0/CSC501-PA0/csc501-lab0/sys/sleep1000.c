/* sleep1000.c - sleep1000 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include "lab0.h"
/*------------------------------------------------------------------------
 * sleep1000 --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
extern int summarizeProcess;
SYSCALL sleep1000(int n)
{
	STATWORD ps;    
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
	if(summarizeProcess == FALSE ) {

                globalTableSystemCall[currpid][SLEEP1000].systemCallName = "sys_sleep1000";
                globalTableSystemCall[currpid][SLEEP1000].systemCallFrequency = globalTableSystemCall[currpid][FREEMEM].systemCallFrequency + 1;

        }
	if (n < 0  || clkruns==0){
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SLEEP1000].systemCallAverageExecTime = globalTableSystemCall[currpid][SLEEP1000].systemCallAverageExecTime + timeTaken;
	         return(SYSERR);
	}	
	disable(ps);
	if (n == 0) {		/* sleep1000(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SLEEP1000].systemCallAverageExecTime = globalTableSystemCall[currpid][SLEEP1000].systemCallAverageExecTime + timeTaken;
	return(OK);
}
