/* unsleep.c - unsleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

/*------------------------------------------------------------------------
 * unsleep  --  remove  process from the sleep queue prematurely
 *------------------------------------------------------------------------
 */
SYSCALL	unsleep(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	struct	qent	*qptr;
	int	remain;
	int	next;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][UNSLEEP].systemCallName = "sys_unsleep";
                globalTableSystemCall[currpid][UNSLEEP].systemCallFrequency = globalTableSystemCall[currpid][UNSLEEP].systemCallFrequency + 1;

        }
        disable(ps);
	if (isbadpid(pid) ||
	    ( (pptr = &proctab[pid])->pstate != PRSLEEP &&
	     pptr->pstate != PRTRECV) ) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][UNSLEEP].systemCallAverageExecTime = globalTableSystemCall[currpid][UNSLEEP].systemCallAverageExecTime + timeTaken;

		return(SYSERR);
	}
	qptr = &q[pid];
	remain = qptr->qkey;
	if ( (next=qptr->qnext) < NPROC)
		q[next].qkey += remain;
	dequeue(pid);
	if ( (next=q[clockq].qnext) < NPROC)
		sltop = (int *) & q[next].qkey;
	else
		slnempty = FALSE;
        restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][UNSLEEP].systemCallAverageExecTime = globalTableSystemCall[currpid][UNSLEEP].systemCallAverageExecTime + timeTaken;

	return(OK);
}
