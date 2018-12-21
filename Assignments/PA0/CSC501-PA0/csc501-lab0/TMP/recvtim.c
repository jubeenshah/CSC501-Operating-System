/* recvtim.c - recvtim */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

/*------------------------------------------------------------------------
 *  recvtim  -  wait to receive a message or timeout and return result
 *------------------------------------------------------------------------
 */
SYSCALL	recvtim(int maxwait)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	int	msg;
	
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][RECVTIM].systemCallName = "sys_recvtim";
                globalTableSystemCall[currpid][RECVTIM].systemCallFrequency = globalTableSystemCall[currpid][RECVTIM].systemCallFrequency + 1;

        }

	if (maxwait<0 || clkruns == 0){
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][RECVTIM].systemCallAverageExecTime = globalTableSystemCall[currpid][RECVTIM].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait		*/
	        insertd(currpid, clockq, maxwait*1000);
		slnempty = TRUE;
		sltop = (int *)&q[q[clockq].qnext].qkey;
	        pptr->pstate = PRTRECV;
		resched();
	}
	if ( pptr->phasmsg ) {
		msg = pptr->pmsg;	/* msg. arrived => retrieve it	*/
		pptr->phasmsg = FALSE;
	} else {			/* still no message => TIMEOUT	*/
		msg = TIMEOUT;
	}
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][RECVTIM].systemCallAverageExecTime = globalTableSystemCall[currpid][RECVTIM].systemCallAverageExecTime + timeTaken;
	return(msg);
}
