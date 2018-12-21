/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep(int n)
{
	STATWORD ps;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SLEEP].systemCallName = "sys_sleep";
                globalTableSystemCall[currpid][SLEEP].systemCallFrequency = globalTableSystemCall[currpid][FREEMEM].systemCallFrequency + 1;

        }    
	if (n<0 || clkruns==0) {
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SLEEP].systemCallAverageExecTime = globalTableSystemCall[currpid][SLEEP].systemCallAverageExecTime + timeTaken;

		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SLEEP].systemCallAverageExecTime = globalTableSystemCall[currpid][SLEEP].systemCallAverageExecTime + timeTaken;
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0) { 
		sleep10(10*n);
	}
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SLEEP].systemCallAverageExecTime = globalTableSystemCall[currpid][SLEEP].systemCallAverageExecTime + timeTaken;
	return(OK);
}
