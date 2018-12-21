/* send.c - send */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;
/*------------------------------------------------------------------------
 *  send  --  send a message to another process
 *------------------------------------------------------------------------
 */
SYSCALL	send(int pid, WORD msg)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SEND].systemCallName = "sys_send";
                globalTableSystemCall[currpid][SEND].systemCallFrequency = globalTableSystemCall[currpid][SEND].systemCallFrequency + 1;

        }

	disable(ps);
	if (isbadpid(pid) || ( (pptr= &proctab[pid])->pstate == PRFREE)
	   || pptr->phasmsg != 0) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SEND].systemCallAverageExecTime = globalTableSystemCall[currpid][SEND].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	pptr->pmsg = msg;
	pptr->phasmsg = TRUE;
	if (pptr->pstate == PRRECV)	/* if receiver waits, start it	*/
		ready(pid, RESCHYES);
	else if (pptr->pstate == PRTRECV) {
		unsleep(pid);
		ready(pid, RESCHYES);
	}
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SEND].systemCallAverageExecTime = globalTableSystemCall[currpid][SEND].systemCallAverageExecTime + timeTaken;
	return(OK);
}
