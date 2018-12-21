/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include "lab0.h"

extern int summarizeProcess;

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	short	*nxtdev;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SETDEV].systemCallName = "sys_setdev";
                globalTableSystemCall[currpid][SETDEV].systemCallFrequency = globalTableSystemCall[currpid][SETDEV].systemCallFrequency + 1;

        }
	if (isbadpid(pid)) {
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SETDEV].systemCallAverageExecTime = globalTableSystemCall[currpid][SETDEV].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SETDEV].systemCallAverageExecTime = globalTableSystemCall[currpid][SETDEV].systemCallAverageExecTime + timeTaken;
	return(OK);
}
