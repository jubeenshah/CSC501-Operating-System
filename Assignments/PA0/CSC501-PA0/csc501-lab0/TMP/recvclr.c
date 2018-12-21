/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include "lab0.h"

/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */

extern int summarizeProcess;

SYSCALL	recvclr()
{
	STATWORD ps;    
	WORD	msg;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][RECVCLR].systemCallName = "sys_recvclr";
                globalTableSystemCall[currpid][RECVCLR].systemCallFrequency = globalTableSystemCall[currpid][RECVCLR].systemCallFrequency + 1;

        }
	disable(ps);
	if (proctab[currpid].phasmsg) {
		proctab[currpid].phasmsg = 0;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][RECVCLR].systemCallAverageExecTime = globalTableSystemCall[currpid][RECVCLR].systemCallAverageExecTime + timeTaken;
	return(msg);
}
