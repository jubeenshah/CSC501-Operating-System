/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include "lab0.h"
/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */
extern int summarizeProcess;

SYSCALL	receive()
{
	STATWORD ps;    
	struct	pentry	*pptr;
	WORD	msg;
	
unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][RECEIVE].systemCallName = "sys_receive";
                globalTableSystemCall[currpid][RECEIVE].systemCallFrequency = globalTableSystemCall[currpid][RECEIVE].systemCallFrequency + 1;

        }
	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][RECEIVE].systemCallAverageExecTime = globalTableSystemCall[currpid][RECEIVE].systemCallAverageExecTime + timeTaken;
	return(msg);
}
